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

/** \file export_ansi.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief Export function
 *
 *  This file contains export functions for ANSI
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

/** \brief Generate ANSI representation of current image.
 *
 *  This function generates and returns an ANSI representation of
 *  the current image.
 *  \param trailing if 0, raw ANSI will be generated. Otherwise, you'll be
 *                  able to cut/paste the result to a function like printf
 *  \return buffer containing generated ANSI codes as a big string
 */
void _cucul_get_ansi(cucul_t *qq, struct cucul_buffer *ex)
{
    static int const palette[] =
    {
        0,  4,  2,  6, 1,  5,  3,  7,
        8, 12, 10, 14, 9, 13, 11, 15
    };

    char *cur;
    unsigned int x, y;

    /* 23 bytes assumed for max length per pixel ('\e[5;1;3x;4y;9x;10ym' plus
     * 4 max bytes for a UTF-8 character).
     * Add height*9 to that (zeroes color at the end and jump to next line) */
    ex->size = (qq->height * 9) + (qq->width * qq->height * 23);
    ex->data = malloc(ex->size);

    cur = ex->data;

    for(y = 0; y < qq->height; y++)
    {
        uint32_t *lineattr = qq->attr + y * qq->width;
        uint32_t *linechar = qq->chars + y * qq->width;

        uint8_t prevfg = -1;
        uint8_t prevbg = -1;

        for(x = 0; x < qq->width; x++)
        {
            uint8_t fg = palette[_cucul_argb32_to_ansi4fg(lineattr[x])];
            uint8_t bg = palette[_cucul_argb32_to_ansi4bg(lineattr[x])];
            uint32_t c = linechar[x];

            if(fg != prevfg || bg != prevbg)
            {
                cur += sprintf(cur, "\033[0;");

                if(fg < 8)
                    if(bg < 8)
                        cur += sprintf(cur, "3%d;4%dm", fg, bg);
                    else
                        cur += sprintf(cur, "5;3%d;4%d;10%dm",
                                            fg, bg - 8, bg - 8);
                else
                    if(bg < 8)
                        cur += sprintf(cur, "1;3%d;4%d;9%dm",
                                            fg - 8, bg, fg - 8);
                    else
                        cur += sprintf(cur, "5;1;3%d;4%d;9%d;10%dm",
                                            fg - 8, bg - 8, fg - 8, bg - 8);
            }

            *cur++ = c & 0x7f;

            prevfg = fg;
            prevbg = bg;
        }

        cur += sprintf(cur, "\033[0m\r\n");
    }

    /* Crop to really used size */
    ex->size = (uintptr_t)(cur - ex->data);
    ex->data = realloc(ex->data, ex->size);
}

