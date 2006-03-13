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
char * cucul_get_ansi(cucul_t *qq, int trailing, int *size)
{
    static int const palette[] =
    {
        30, 34, 32, 36, 31, 35, 33, 37, /* Both lines (light and dark) are the same, */
        30, 34, 32, 36, 31, 35, 33, 37, /* light colors handling is done later */
    };

    char *cur;
    unsigned int x, y;

    /* 20 bytes assumed for max length per pixel.
     * Add height*9 to that (zeroes color at the end and jump to next line) */
    if(qq->ansi_buffer)
        free(qq->ansi_buffer);
    qq->ansi_buffer = malloc(((qq->height*9) + (qq->width * qq->height * 20)) * sizeof(char));
    if(qq->ansi_buffer == NULL)
        return NULL;

    cur = qq->ansi_buffer;

    // *cur++ = '';

    for(y = 0; y < qq->height; y++)
    {
        uint8_t *lineattr = qq->attr + y * qq->width;
        uint32_t *linechar = qq->chars + y * qq->width;

        uint8_t prevfg = -1;
        uint8_t prevbg = -1;

        for(x = 0; x < qq->width; x++)
        {
            uint8_t fg = palette[lineattr[x] & 0x0f];
            uint8_t bg = (palette[lineattr[x] >> 4])+10;
            uint32_t c = linechar[x];

            if(!trailing)
                cur += sprintf(cur, "\033[");
            else
                cur += sprintf(cur, "\\033[");

            if(fg > 7)
                cur += sprintf(cur, "1;%d;%dm",fg,bg);
            else
                cur += sprintf(cur, "0;%d;%dm",fg,bg);
            *cur++ = c & 0x7f;
            if((c == '%') && trailing)
                *cur++ = c & 0x7f;
            prevfg = fg;
            prevbg = bg;
        }
        if(!trailing)
            cur += sprintf(cur, "\033[0m\r\n");
        else
            cur += sprintf(cur, "\\033[0m\\n\n");
    }

    /* Crop to really used size */
    *size = (strlen(qq->ansi_buffer) + 1)* sizeof(char);
    qq->ansi_buffer = realloc(qq->ansi_buffer, *size);

    return qq->ansi_buffer;
}

