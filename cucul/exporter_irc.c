/*
 *  libcucul      Unicode canvas library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file export.c
 *  \version \$Id: export.c 361 2006-03-09 13:24:06Z jylam $
 *  \author Sam Hocevar <sam@zoy.org>
 *  \author Jean-Yves Lamoureux <jylam@lnxscene.org>
 *  \brief Export function
 *
 *  This file contains export functions for IRC
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"



/** \brief Generate IRC representation of current image.
 *
 *  This function generates and returns an IRC representation of
 *  the current image.
 */
char* cucul_get_irc(cucul_t *qq, int *size)
{
    static int const palette[] =
    {
        1, 2, 3, 10, 5, 6, 7, 15, /* Dark */
        14, 12, 9, 11, 4, 13, 8, 0, /* Light */
    };

    char *cur;
    unsigned int x, y;

    /* 11 bytes assumed for max length per pixel. Worst case scenario:
     * ^Cxx,yy   6 bytes
     * ^B^B      2 bytes
     * c         1 byte
     * \r\n      2 bytes
     * In real life, the average bytes per pixel value will be around 5.
     */

    if(qq->irc_buffer)
        free(qq->irc_buffer);

    qq->irc_buffer = malloc((2 + (qq->width * qq->height * 11)) * sizeof(char));
    if(qq->irc_buffer == NULL)
        return NULL;

    cur = qq->irc_buffer;

    *cur++ = '\x0f';

    for(y = 0; y < qq->height; y++)
    {
        uint8_t *lineattr = qq->attr + y * qq->width;
        uint32_t *linechar = qq->chars + y * qq->width;

        uint8_t prevfg = -1;
        uint8_t prevbg = -1;

        for(x = 0; x < qq->width; x++)
        {
            uint8_t fg = palette[lineattr[x] & 0x0f];
            uint8_t bg = palette[lineattr[x] >> 4];
            uint32_t c = linechar[x];

            if(bg == prevbg)
            {
                if(fg == prevfg)
                    ; /* Same fg/bg, do nothing */
                else if(c == (uint32_t)' ')
                    fg = prevfg; /* Hackety hack */
                else
                {
                    cur += sprintf(cur, "\x03%d", fg);
                    if(c >= (uint32_t)'0' && c <= (uint32_t)'9')
                        cur += sprintf(cur, "\x02\x02");
                }
            }
            else
            {
                if(fg == prevfg)
                    cur += sprintf(cur, "\x03,%d", bg);
                else
                    cur += sprintf(cur, "\x03%d,%d", fg, bg);

                if(c >= (uint32_t)'0' && c <= (uint32_t)'9')
                    cur += sprintf(cur, "\x02\x02");
            }
            *cur++ = c & 0x7f;
            prevfg = fg;
            prevbg = bg;
        }
        *cur++ = '\r';
        *cur++ = '\n';
    }

    *cur++ = '\x0f';

    /* Crop to really used size */
    *size = (strlen(qq->irc_buffer) + 1) * sizeof(char);
    qq->irc_buffer = realloc(qq->irc_buffer, *size);
    
    return qq->irc_buffer;
}
