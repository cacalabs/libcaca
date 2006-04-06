/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file export.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief Export function
 *
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
void _cucul_get_ps(cucul_t *qq, struct cucul_export *ex)
{
    static char const * const palette[] =
    {
        "0.0 0.0 0.0", "0.0 0.0 0.5", "0.0 0.5 0.0", "0.0 0.5 0.5",
        "0.5 0.0 0.0", "0.5 0.0 0.5", "0.5 0.5 0.0", "0.5 0.5 0.5",

        "0.2 0.2 0.2", "0.2 0.2 1.0", "0.2 1.0 0.2", "0.2 1.0 1.0",
        "1.0 0.2 0.2", "1.0 0.2 1.0", "1.0 1.0 0.2", "1.0 1.0 1.0",
    };

    char *cur;
    unsigned int x, y;

    /* 200 is arbitrary but should be ok */
    ex->size = strlen(ps_header) + (qq->width * qq->height * 200);
    ex->buffer = malloc(ex->size);

    cur = ex->buffer;

    /* Header */
    cur += sprintf(cur, "%s", ps_header);

    /* Background, drawn using csquare macro defined in header */
    for(y = qq->height; y--; )
    {
        uint32_t *lineattr = qq->attr + y * qq->width;

        for(x = 0; x < qq->width; x++)
        {
            cur += sprintf(cur, "1 0 translate\n %s csquare\n",
                           palette[_cucul_rgba32_to_ansi4bg(*lineattr++)]);
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
            uint32_t c = *linechar++;

            cur += sprintf(cur, "newpath\n");
            cur += sprintf(cur, "%d %d moveto\n", (x + 1) * 6, y * 10);
            cur += sprintf(cur, "%s setrgbcolor\n",
                           palette[_cucul_rgba32_to_ansi4fg(*lineattr++)]);

            if(c < 0x00000020)
                cur += sprintf(cur, "(?) show\n");
            else if(c >= 0x00000080)
                cur += sprintf(cur, "(?) show\n");
            else switch((uint8_t)(c & 0x7f))
            {
                case '\\': cur += sprintf(cur, "(\\\\) show\n"); break;
                case '(': cur += sprintf(cur, "(\\() show\n"); break;
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
    ex->size = strlen(ex->buffer) + 1;
    ex->buffer = realloc(ex->buffer, ex->size);
}

