/*
 *  sortchars     analyse ASCII characters
 *  Copyright (c) 2007 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"
#include "common.h"
#if !defined(__KERNEL__)
#   if defined(HAVE_INTTYPES_H)
#      include <inttypes.h>
#   endif
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif
#include "cucul.h"

#define GLYPHS 0x7f
#define FONT 1 /* 0 or 1 */
#define DX 2
#define DY 3
#define RANGE 4

int total[GLYPHS][DX][DY];

static int curve[17] = /* 17 instead of 16 */
{
    0, 4, 6, 8, 9, 10, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15, 15
};

int main(int argc, char *argv[])
{
    int count[DX][DY];
    char utf8[7];
    cucul_canvas_t *cv;
    cucul_font_t *f;
    char const * const * fonts;
    uint8_t *img;
    unsigned int w, h, x, y;
    int ret, i, max;

    /* Load a libcucul internal font */
    fonts = cucul_get_font_list();
    if(fonts[FONT] == NULL)
    {
        fprintf(stderr, "error: libcucul was compiled without any fonts\n");
        return -1;
    }
    f = cucul_load_font(fonts[FONT], 0);
    if(f == NULL)
    {
        fprintf(stderr, "error: could not load font \"%s\"\n", fonts[0]);
        return -1;
    }

    cv = cucul_create_canvas(1, 1);
    cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLACK);

    /* Create our bitmap buffer (32-bit ARGB) */
    w = cucul_get_font_width(f);
    h = cucul_get_font_height(f);
    img = malloc(4 * w * h);

    /* Zero our structures */
    for(y = 0; y < DY; y++)
        for(x = 0; x < DX; x++)
            count[x][y] = 0;

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
            count[x * DX / w][y * DY / h]++;

    for(i = 0x20; i < GLYPHS; i++)
        for(y = 0; y < DY; y++)
            for(x = 0; x < DX; x++)
                total[GLYPHS][x][y] = 0;

    /* Draw all glyphs and count their pixels */
    for(i = 0x20; i < GLYPHS; i++)
    {
        cucul_put_char(cv, 0, 0, i);

        /* Render the canvas onto our image buffer */
        cucul_render_canvas(cv, f, img, w, h, 4 * w);

        for(y = 0; y < h * DY; y++)
            for(x = 0; x < w * DX; x++)
                total[i][x / w][y / h]
                  += img[(w * (y / DY) + (x / DX)) * 4 + 1];
    }

    /* Compute max value */
    max = 0;
    for(i = 0x20; i < GLYPHS; i++)
        for(y = 0; y < DY; y++)
            for(x = 0; x < DX; x++)
            {
                int val = total[i][x][y] * 256 / count[x][y];
                if(val > max)
                    max = val;
            }

    for(i = 0x20; i < GLYPHS; i++)
    {
        int bits = 0;

        if(i >= 0x7f && i <= 0x9f)
            continue;

        ret = cucul_utf32_to_utf8(utf8, i);
        utf8[ret] = '\0';

        for(y = 0; y < DY; y++)
        {
            for(x = 0; x < DX; x++)
            {
                int t = total[i][x][y] * 16 * 256 / (count[x][y] * max);
                t = curve[t] / (16 / RANGE);

                printf("%d ", t);
                bits *= RANGE;
                bits |= t;
            }
        }

        printf(" 0x%04d (%s)\n", bits, utf8);
    }

    cucul_free_canvas(cv);

    return 0;
}

