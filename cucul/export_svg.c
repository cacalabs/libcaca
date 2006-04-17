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
 *  This file contains export functions for SVG (Scalable Vector Graphics files
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

static char const svg_header[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<svg width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\""
    " xmlns=\"http://www.w3.org/2000/svg\""
    " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
    " xml:space=\"preserve\" version=\"1.1\"  baseProfile=\"full\">\n";

/*
 *  This function generates and returns an SVG representation of
 *  the current image.
 */
void _cucul_get_svg(cucul_t *qq, cucul_buffer_t *ex)
{
    char *cur;
    unsigned int x, y;

    /* 200 is arbitrary but should be ok */
    ex->size = strlen(svg_header) + (qq->width * qq->height * 200);
    ex->data = malloc(ex->size);

    cur = ex->data;

    /* Header */
    cur += sprintf(cur, svg_header, qq->width * 6, qq->height * 10,
                                    qq->width * 6, qq->height * 10);

    cur += sprintf(cur, " <g id=\"mainlayer\" font-size=\"12\">\n");

    /* Background */
    for(y = 0; y < qq->height; y++)
    {
        uint32_t *lineattr = qq->attr + y * qq->width;

        for(x = 0; x < qq->width; x++)
        {
            cur += sprintf(cur, "<rect style=\"fill:#%.03x\" x=\"%d\" y=\"%d\""
                                " width=\"6\" height=\"10\"/>\n",
                                _cucul_argb32_to_rgb12bg(*lineattr++),
                                x * 6, y * 10);
        }
    }

    /* Text */
    for(y = 0; y < qq->height; y++)
    {
        uint32_t *lineattr = qq->attr + y * qq->width;
        uint32_t *linechar = qq->chars + y * qq->width;

        for(x = 0; x < qq->width; x++)
        {
            uint32_t c = *linechar++;

            cur += sprintf(cur, "<text style=\"fill:#%.03x\" "
                                "x=\"%d\" y=\"%d\">",
                                _cucul_argb32_to_rgb12fg(*lineattr++),
                                x * 6, (y * 10) + 10);
            if(c < 0x00000020)
                cur += sprintf(cur, "?");
            else if(c > 0x0000007f)
            {
                static const uint8_t mark[7] =
                {
                    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
                };

                char buf[10], *parser;
                int bytes = (c < 0x800) ? 2 : (c < 0x10000) ? 3 : 4;

                buf[bytes] = '\0';
                parser = buf + bytes;

                switch(bytes)
                {
                    case 4: *--parser = (c | 0x80) & 0xbf; c >>= 6;
                    case 3: *--parser = (c | 0x80) & 0xbf; c >>= 6;
                    case 2: *--parser = (c | 0x80) & 0xbf; c >>= 6;
                }
                *--parser = c | mark[bytes];

                cur += sprintf(cur, "%s", buf);
            }
            else switch((uint8_t)c)
            {
                case '>': cur += sprintf(cur, "&gt;"); break;
                case '<': cur += sprintf(cur, "&lt;"); break;
                case '&': cur += sprintf(cur, "&amp;"); break;
                default: cur += sprintf(cur, "%c", c); break;
            }
            cur += sprintf(cur, "</text>\n");
        }
    }

    cur += sprintf(cur, " </g>\n");
    cur += sprintf(cur, "</svg>\n");

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

