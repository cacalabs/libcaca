/*
 *   libcaca       ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307  USA
 */

/**  \file blit.c
 *   \version \$Id$
 *   \author Sam Hocevar <sam@zoy.org>
 *   \brief Bitmap functions
 *
 *   This file contains bitmap blitting functions.
 */

#include "config.h"

#ifdef HAVE_INTTYPES_H
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
#endif

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

/* Dithering methods */
static void init_no_dither(int);
static int get_no_dither(void);
static void increment_no_dither(void);

static void init_ordered_dither(int);
static int get_ordered_dither(void);
static void increment_ordered_dither(void);

static void init_random_dither(int);
static int get_random_dither(void);
static void increment_random_dither(void);

/* Current dithering method */
static enum caca_dithering _caca_dithering = CACA_DITHER_NONE;

static void (*_init_dither) (int) = init_no_dither;
static int (*_get_dither) (void) = get_no_dither;
static void (*_increment_dither) (void) = increment_no_dither;

void caca_set_dithering(enum caca_dithering dither)
{
    switch(dither)
    {
    case CACA_DITHER_NONE:
        _init_dither = init_no_dither;
        _get_dither = get_no_dither;
        _increment_dither = increment_no_dither;
        break;

    case CACA_DITHER_ORDERED:
        _init_dither = init_ordered_dither;
        _get_dither = get_ordered_dither;
        _increment_dither = increment_ordered_dither;
        break;

    case CACA_DITHER_RANDOM:
        _init_dither = init_random_dither;
        _get_dither = get_random_dither;
        _increment_dither = increment_random_dither;
        break;

    default:
        return;
    }

    _caca_dithering = dither;
}

struct caca_bitmap
{
    int bpp;
    int w, h, pitch;
    int rmask, gmask, bmask;
};

struct caca_bitmap *caca_create_bitmap(int bpp, int w, int h, int pitch,
                                       int rmask, int gmask, int bmask)
{
    struct caca_bitmap *bitmap;

    /* Currently only this format is supported. Will improve later. */
    if(!w || !h || !pitch || bpp != 32 ||
       rmask != 0x00ff0000 || gmask != 0x0000ff00 || bmask != 0x000000ff)
        return NULL;

    bitmap = malloc(sizeof(struct caca_bitmap));
    if(!bitmap)
        return NULL;

    bitmap->bpp = bpp;

    bitmap->w = w;
    bitmap->h = h;
    bitmap->pitch = pitch;

    bitmap->rmask = rmask;
    bitmap->gmask = gmask;
    bitmap->bmask = bmask;

    return bitmap;
}

void caca_free_bitmap(struct caca_bitmap *bitmap)
{
    if(!bitmap)
        return;

    free(bitmap);
}

void caca_draw_bitmap(int x1, int y1, int x2, int y2,
                      struct caca_bitmap *bitmap, char *pixels)
{
    /* FIXME: this code is shite! */
    static int white_colors[] = {CACA_COLOR_DARKGRAY, CACA_COLOR_LIGHTGRAY, CACA_COLOR_WHITE};
    static int light_colors[] = {CACA_COLOR_LIGHTMAGENTA, CACA_COLOR_LIGHTRED, CACA_COLOR_YELLOW, CACA_COLOR_LIGHTGREEN, CACA_COLOR_LIGHTCYAN, CACA_COLOR_LIGHTBLUE, CACA_COLOR_LIGHTMAGENTA};
    static int dark_colors[] = {CACA_COLOR_MAGENTA, CACA_COLOR_RED, CACA_COLOR_BROWN, CACA_COLOR_GREEN, CACA_COLOR_CYAN, CACA_COLOR_BLUE, CACA_COLOR_MAGENTA};
    static char foo[] = { ' ', '.', ':', ';', '=', '%', '$', 'W', '#', '8', '@' };
    int x, y, w, h, pitch;

    if(!bitmap || !pixels)
        return;

    w = bitmap->w;
    h = bitmap->h;
    pitch = bitmap->pitch;

    if(x1 > x2)
    {
        int tmp = x2; x2 = x1; x1 = tmp;
    }

    if(y1 > y2)
    {
        int tmp = y2; y2 = y1; y1 = tmp;
    }

    for(y = y1 > 0 ? y1 : 0; y <= y2 && y <= (int)caca_get_height(); y++)
    {
        /* Initialize dither tables for the current line */
        _init_dither(y);

        /* Dither the current line */
        for(x = x1 > 0 ? x1 : 0; x <= x2 && x <= (int)caca_get_width(); x++)
        {
            int fromx = w * (x - x1) / (x2 - x1 + 1);
            int fromy = h * (y - y1) / (y2 - y1 + 1);
            /* FIXME: bwahaaa, we don't even respect masks */
            int b = ((unsigned char *)pixels)[4 * fromx + pitch * fromy];
            int g = ((unsigned char *)pixels)[4 * fromx + 1 + pitch * fromy];
            int r = ((unsigned char *)pixels)[4 * fromx + 2 + pitch * fromy];
            int hue, sat, val;

            int min = r, max = r, delta;
            if(min > g) min = g; if(max < g) max = g;
            if(min > b) min = b; if(max < b) max = b;

            delta = max - min;
            val = max; /* 0 - 255 */
            sat = max ? 256 * delta / max : 0; /* 0 - 255 */

            if(sat > (_get_dither() + 24) * 4)
            {
                /* XXX: Values should be between 1 and 6, but since we
                 * are dithering, there may be overflows, hence our bigger
                 * *_colors[] tables. */
                if( r == max )
                    hue = 256 + 256 * (g - b) / delta;
                else if( g == max )
                    hue = 768 + 256 * (b - r) / delta;
                else
                    hue = 1280 + 256 * (r - g) / delta;

                hue = (hue + 128 + 16 * _get_dither()) / 256;

                if(val > (_get_dither() + 40) * 4)
                    caca_set_color(light_colors[hue]);
                else
                    caca_set_color(dark_colors[hue]);
            }
            else
            {
                caca_set_color(white_colors[max * 3 / 256]);
            }

            caca_putchar(x, y, foo[(r + g + b + 2 * _get_dither()) / 3 / 25]);

            _increment_dither();
        }
    }
}

/*
 * XXX: The following functions are local.
 */

/*
 * No dithering
 */
static void init_no_dither(int line)
{
    ;
}

static int get_no_dither(void)
{
    return 0;
}

static void increment_no_dither(void)
{
    return;
}

/*
 * Ordered dithering
 */
static int dither4x4[] = {-8,  0, -6,  2,
                           4, -4,  6, -2,
                          -5,  3, -7,  1,
                           7, -1,  5, -3};
static int *dither_table;
static int dither_index;

static void init_ordered_dither(int line)
{
    dither_table = dither4x4 + (line % 4) * 4;
    dither_index = 0;
}

static int get_ordered_dither(void)
{
    return dither_table[dither_index];
}

static void increment_ordered_dither(void)
{
    dither_index = (dither_index + 1) % 4;
}

/*
 * Random dithering
 */
static void init_random_dither(int line)
{
    ;
}

static int get_random_dither(void)
{
    return caca_rand(-8, 7);
}

static void increment_random_dither(void)
{
    return;
}

