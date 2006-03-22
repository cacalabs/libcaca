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
    " xml:space=\"preserve\" version=\"1.1\"  baseProfile=\"full\">\n"
    " <defs>\n"
    "  <style type=\"text/css\">\n"
    "    <![CDATA[\n";

/** \brief Generate SVG representation of current image.
 *
 *  This function generates and returns a SVG representation of
 *  the current image.
 */
void _cucul_get_svg(cucul_t *qq, struct cucul_export *ex)
{
    static int const palette[] =
    {
        0x000000, 0x000088, 0x008800, 0x008888,
        0x880000, 0x880088, 0x888800, 0x888888,
        0x444444, 0x4444ff, 0x44ff44, 0x44ffff,
        0xff4444, 0xff44ff, 0xffff44, 0xffffff,
    };

    char *cur;
    unsigned int x, y;

    /* 200 is arbitrary but should be ok */
    ex->size = strlen(svg_header) + (qq->width * qq->height * 200);
    ex->buffer = malloc(ex->size);

    cur = ex->buffer;

    /* Header */
    cur += sprintf(cur, svg_header, qq->width * 6, qq->height * 10,
                                    qq->width * 6, qq->height * 10);

    /* Precalc of colors in CSS style  */
    for(x = 0; x < 0x100; x++)
    {
        cur += sprintf(cur, ".b%02x {fill:#%06X}\n", x, palette[x >> 4]);
        cur += sprintf(cur, ".f%02x {fill:#%06X}\n", x, palette[x & 0xf]);
    }

    cur += sprintf(cur, "]]>\n");
    cur += sprintf(cur, "  </style>\n");
    cur += sprintf(cur, " </defs>\n");
    cur += sprintf(cur, " <g id=\"mainlayer\" font-size=\"12\">\n");

    /* Background */
    for(y = 0; y < qq->height; y++)
    {
        uint8_t *lineattr = qq->attr + y * qq->width;

        for(x = 0; x < qq->width; x++)
        {
            cur += sprintf(cur, "<rect class=\"b%02x\" x=\"%d\" y=\"%d\""
                                " width=\"6\" height=\"10\"/>\n",
                                *lineattr++, x * 6, y * 10);
        }
    }

    /* Text */
    for(y = 0; y < qq->height; y++)
    {
        uint8_t *lineattr = qq->attr + y * qq->width;
        uint32_t *linechar = qq->chars + y * qq->width;

        for(x = 0; x < qq->width; x++)
        {
            uint32_t c = *linechar++;

            cur += sprintf(cur, "<text class=\"f%02x\" x=\"%d\" y=\"%d\">",
                                *lineattr++, x * 6, (y * 10) + 10);
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
    ex->size = strlen(ex->buffer) + 1;
    ex->buffer = realloc(ex->buffer, ex->size);
}

