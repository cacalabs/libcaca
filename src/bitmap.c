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

/**  \file bitmap.c
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
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

static void mask2shift(int, int *, int *);

static void get_rgb_default(struct caca_bitmap *, unsigned char *,
                            int, int, int *, int *, int *);
static void rgb2hsv_default(int, int, int, int *, int *, int *);

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
    int bpp, palette;
    int w, h, pitch;
    int rmask, gmask, bmask;
    int rright, gright, bright;
    int rleft, gleft, bleft;
    void (*get_hsv)(struct caca_bitmap *, char *, int, int);
    int red[256], green[256], blue[256];
};

static void mask2shift(int mask, int *right, int *left)
{
    int rshift = 0, lshift = 0;
    *right = *left = 0;

    if(!mask)
        return;

    while(!(mask & 1))
    {
        mask >>= 1;
        rshift++;
    }
    *right = rshift;

    while(mask & 1)
    {
        mask >>= 1;
        lshift++;
    }
    *left = 16 - lshift;
}

struct caca_bitmap *caca_create_bitmap(int bpp, int w, int h, int pitch,
                                       int rmask, int gmask, int bmask)
{
    struct caca_bitmap *bitmap;

    /* Currently only this format is supported. Will improve later. */
    if(!w || !h || !pitch || bpp > 32 || bpp < 8)
        return NULL;

    bitmap = malloc(sizeof(struct caca_bitmap));
    if(!bitmap)
        return NULL;

    bitmap->bpp = bpp;
    bitmap->palette = 0;

    bitmap->w = w;
    bitmap->h = h;
    bitmap->pitch = pitch;

    bitmap->rmask = rmask;
    bitmap->gmask = gmask;
    bitmap->bmask = bmask;

    /* Load bitmasks */
    if(rmask || gmask || bmask)
    {
        mask2shift(rmask, &bitmap->rright, &bitmap->rleft);
        mask2shift(gmask, &bitmap->gright, &bitmap->gleft);
        mask2shift(bmask, &bitmap->bright, &bitmap->bleft);
    }

    /* In 8 bpp mode, default to a grayscale palette */
    if(bpp == 8)
    {
        int i;
        bitmap->palette = 1;
        for(i = 0; i < 256; i++)
        {
            bitmap->red[i] = i * 0x100;
            bitmap->green[i] = i * 0x100;
            bitmap->blue[i] = i * 0x100;
        }
    }

    return bitmap;
}

void caca_set_bitmap_palette(struct caca_bitmap *bitmap,
                             int red[], int green[], int blue[])
{
    int i;

    if(bitmap->bpp != 8)
        return;

    for(i = 0; i < 256; i++)
    {
        if(red[i] >= 0 && red[i] < 65536 &&
           green[i] >= 0 && green[i] < 65536 &&
           blue[i] >= 0 && blue[i] < 65536)
        {
            bitmap->red[i] = red[i];
            bitmap->green[i] = green[i];
            bitmap->blue[i] = blue[i];
        }
    }
}

void caca_free_bitmap(struct caca_bitmap *bitmap)
{
    if(!bitmap)
        return;

    free(bitmap);
}

static void get_rgb_default(struct caca_bitmap *bitmap, unsigned char *pixels,
                            int x, int y, int *r, int *g, int *b)
{
    int bits;

    pixels += (bitmap->bpp / 8) * x + bitmap->pitch * y;

    switch(bitmap->bpp / 8)
    {
        case 4:
            bits = *(uint32_t *)(pixels + 0);
            break;
        case 3:
            bits = (pixels[0] << 16)
                    | (pixels[1] << 8)
                    | (pixels[2]);
            break;
        case 2:
            bits = *(uint16_t *)(pixels + 0);
            break;
        case 1:
        default:
            bits = pixels[0];
            break;
    }

    if(bitmap->palette)
    {
        *r = bitmap->red[bits];
        *g = bitmap->green[bits];
        *b = bitmap->blue[bits];
    }
    else
    {
        *r = ((bits & bitmap->rmask) >> bitmap->rright) << bitmap->rleft;
        *g = ((bits & bitmap->gmask) >> bitmap->gright) << bitmap->gleft;
        *b = ((bits & bitmap->bmask) >> bitmap->bright) << bitmap->bleft;
    }
}

static void rgb2hsv_default(int r, int g, int b, int *hue, int *sat, int *val)
{
    int min, max, delta;

    min = r; max = r;
    if(min > g) min = g; if(max < g) max = g;
    if(min > b) min = b; if(max < b) max = b;

    delta = max - min; /* 0 - 65535 */
    *val = max; /* 0 - 65535 */
    *sat = max ? 0x100 * delta / max * 0x100: 0; /* 0 - 65536 */

    if(*sat > (_get_dither() + 24) * 0x400)
    {
        /* XXX: Values should be between 1 and 6, but since we
         * are dithering, there may be overflows, hence our bigger
         * *_colors[] tables. */
        if( r == max )
            *hue = 0x10000 + 0x100 * (g - b) / delta * 0x100;
        else if( g == max )
            *hue = 0x30000 + 0x100 * (b - r) / delta * 0x100;
        else
            *hue = 0x50000 + 0x100 * (r - g) / delta * 0x100;

        *hue = (*hue + 0x8000 + 0x1000 * _get_dither()) / 0x10000;
    }
    else
    {
        *sat = 0;
    }
}

void caca_draw_bitmap(int x1, int y1, int x2, int y2,
                      struct caca_bitmap *bitmap, char *pixels)
{
    /* FIXME: this code is shite! */
    static int white_colors[] =
    {
        CACA_COLOR_DARKGRAY,
        CACA_COLOR_LIGHTGRAY,
        CACA_COLOR_WHITE
    };

    static int light_colors[] =
    {
        CACA_COLOR_LIGHTMAGENTA,
        CACA_COLOR_LIGHTRED,
        CACA_COLOR_YELLOW,
        CACA_COLOR_LIGHTGREEN,
        CACA_COLOR_LIGHTCYAN,
        CACA_COLOR_LIGHTBLUE,
        CACA_COLOR_LIGHTMAGENTA
    };

    static int dark_colors[] =
    {
        CACA_COLOR_MAGENTA,
        CACA_COLOR_RED,
        CACA_COLOR_BROWN,
        CACA_COLOR_GREEN,
        CACA_COLOR_CYAN,
        CACA_COLOR_BLUE,
        CACA_COLOR_MAGENTA
    };

    static char foo[] =
    {
        ' ', ' ', ' ', ' ',
        ',', '`', '.', '\'',
        'i', '-', ':', '^',
        '|', '/', ';', '\\',
        '=', '+', 'o', 'x',
        '<', 'x', '%', '>',
        '&', 'z', '$', 'w',
        'W', 'X', 'K', 'M',
        '#', '8', '#', '#',
        '8', '@', '8', '#',
        '@', '8', '@', '8',
    };

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
            int ch;
            int hue, sat, val, r, g, b, R, G, B;
            int fromx = w * (x - x1) / (x2 - x1 + 1);
            int fromy = h * (y - y1) / (y2 - y1 + 1);

            /* First get RGB */
            R = 0, G = 0, B = 0;
            get_rgb_default(bitmap, pixels, fromx, fromy, &r, &g, &b);
            R += r; G += g; B += b;
            get_rgb_default(bitmap, pixels, fromx - 1, fromy, &r, &g, &b);
            R += r; G += g; B += b;
            get_rgb_default(bitmap, pixels, fromx, fromy - 1, &r, &g, &b);
            R += r; G += g; B += b;
            get_rgb_default(bitmap, pixels, fromx + 1, fromy, &r, &g, &b);
            R += r; G += g; B += b;
            get_rgb_default(bitmap, pixels, fromx, fromy + 1, &r, &g, &b);
            R += r; G += g; B += b;
            R /= 5; G /= 5; B /= 5;

            /* Now get HSV from RGB */
            rgb2hsv_default(R, G, B, &hue, &sat, &val);

            if(!sat)
                caca_set_color(white_colors[val * 3 / 0x10000], CACA_COLOR_BLACK);
            else if(val > (_get_dither() + 40) * 0x400)
                caca_set_color(light_colors[hue], CACA_COLOR_BLACK);
            else
                caca_set_color(dark_colors[hue], CACA_COLOR_BLACK);

            /* FIXME: choose better characters! */
            ch = (val + 0x200 * _get_dither()) * 10 / 0x10000;
            ch = 4 * ch + (_get_dither() + 8) / 4;
            caca_putchar(x, y, foo[ch]);

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

