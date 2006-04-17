/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains export functions for Postscript files
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static char const *ps_header =
    "%!\n"
    "%% libcaca PDF export\n"
    "%%LanguageLevel: 2\n"
    "%%Pages: 1\n"
    "%%DocumentData: Clean7Bit\n"
    "/csquare {\n"
    "  newpath\n"
    "  0 0 moveto\n"
    "  0 1 rlineto\n"
    "  1 0 rlineto\n"
    "  0 -1 rlineto\n"
    "  closepath\n"
    "  setrgbcolor\n"
    "  fill\n"
    "} def\n"
    "/S {\n"
    "  Show\n"
    "} bind def\n"
    "/Courier-Bold findfont\n"
    "8 scalefont\n"
    "setfont\n"
    "gsave\n"
    "6 10 scale\n";

/** \brief Generate Postscript representation of current image.
 *
 *  This function generates and returns a Postscript representation of
 *  the current image.
 */
void _cucul_get_ps(cucul_t *qq, cucul_buffer_t *ex)
{
    char *cur;
    unsigned int x, y;

    /* 200 is arbitrary but should be ok */
    ex->size = strlen(ps_header) + (qq->width * qq->height * 200);
    ex->data = malloc(ex->size);

    cur = ex->data;

    /* Header */
    cur += sprintf(cur, "%s", ps_header);

    /* Background, drawn using csquare macro defined in header */
    for(y = qq->height; y--; )
    {
        uint32_t *lineattr = qq->attr + y * qq->width;

        for(x = 0; x < qq->width; x++)
        {
            uint8_t argb[8];
            _cucul_argb32_to_argb4(*lineattr++, argb);
            cur += sprintf(cur, "1 0 translate\n %f %f %f csquare\n",
                           (float)argb[1] * (1.0 / 0xf),
                           (float)argb[2] * (1.0 / 0xf),
                           (float)argb[3] * (1.0 / 0xf));
        }

        /* Return to beginning of the line, and jump to the next one */
        cur += sprintf(cur, "-%d 1 translate\n", qq->width);
    }

    cur += sprintf(cur, "grestore\n"); /* Restore transformation matrix */

    for(y = qq->height; y--; )
    {
        uint32_t *lineattr = qq->attr + (qq->height - y - 1) * qq->width;
        uint32_t *linechar = qq->chars + (qq->height - y - 1) * qq->width;

        for(x = 0; x < qq->width; x++)
        {
            uint8_t argb[8];
            uint32_t c = *linechar++;

            _cucul_argb32_to_argb4(*lineattr++, argb);

            cur += sprintf(cur, "newpath\n");
            cur += sprintf(cur, "%d %d moveto\n", (x + 1) * 6, y * 10 + 2);
            cur += sprintf(cur, "%f %f %f setrgbcolor\n",
                           (float)argb[5] * (1.0 / 0xf),
                           (float)argb[6] * (1.0 / 0xf),
                           (float)argb[7] * (1.0 / 0xf));

            if(c < 0x00000020)
                cur += sprintf(cur, "(?) show\n");
            else if(c >= 0x00000080)
                cur += sprintf(cur, "(?) show\n");
            else switch((uint8_t)(c & 0x7f))
            {
                case '\\':
                case '(':
                case ')':
                    cur += sprintf(cur, "(\\%c) show\n", c);
                    break;
                default:
                    cur += sprintf(cur, "(%c) show\n", c);
                    break;
            }
        }
    }

    cur += sprintf(cur, "showpage\n");

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

