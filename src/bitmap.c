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

#define NEW_RENDERER 1

static void mask2shift(unsigned int, int *, int *);

static void get_rgb_default(struct caca_bitmap *, uint8_t *, int, int,
                            unsigned int *, unsigned int *, unsigned int *);
static void rgb2hsv_default(int, int, int, int *, int *, int *);

/* Dithering methods */
static void init_no_dither(int);
static unsigned int get_no_dither(void);
static void increment_no_dither(void);

static void init_ordered2_dither(int);
static unsigned int get_ordered2_dither(void);
static void increment_ordered2_dither(void);

static void init_ordered4_dither(int);
static unsigned int get_ordered4_dither(void);
static void increment_ordered4_dither(void);

static void init_ordered8_dither(int);
static unsigned int get_ordered8_dither(void);
static void increment_ordered8_dither(void);

static void init_random_dither(int);
static unsigned int get_random_dither(void);
static void increment_random_dither(void);

/* Current dithering method */
static enum caca_dithering _caca_dithering = CACA_DITHERING_NONE;

static void (*_init_dither) (int) = init_no_dither;
static unsigned int (*_get_dither) (void) = get_no_dither;
static void (*_increment_dither) (void) = increment_no_dither;

void caca_set_dithering(enum caca_dithering dither)
{
    switch(dither)
    {
    case CACA_DITHERING_NONE:
        _init_dither = init_no_dither;
        _get_dither = get_no_dither;
        _increment_dither = increment_no_dither;
        break;

    case CACA_DITHERING_ORDERED2:
        _init_dither = init_ordered2_dither;
        _get_dither = get_ordered2_dither;
        _increment_dither = increment_ordered2_dither;
        break;

    case CACA_DITHERING_ORDERED4:
        _init_dither = init_ordered4_dither;
        _get_dither = get_ordered4_dither;
        _increment_dither = increment_ordered4_dither;
        break;

    case CACA_DITHERING_ORDERED8:
        _init_dither = init_ordered8_dither;
        _get_dither = get_ordered8_dither;
        _increment_dither = increment_ordered8_dither;
        break;

    case CACA_DITHERING_RANDOM:
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

static void mask2shift(unsigned int mask, int *right, int *left)
{
    int rshift = 0, lshift = 0;

    if(!mask)
    {
        *right = *left = 0;
        return;
    }

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
    *left = 12 - lshift;
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
            bitmap->red[i] = i * 0x10;
            bitmap->green[i] = i * 0x10;
            bitmap->blue[i] = i * 0x10;
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
        if(red[i] >= 0 && red[i] < 0x1000 &&
           green[i] >= 0 && green[i] < 0x1000 &&
           blue[i] >= 0 && blue[i] < 0x1000)
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

static void get_rgb_default(struct caca_bitmap *bitmap, uint8_t *pixels,
                            int x, int y, unsigned int *r,
                            unsigned int *g, unsigned int *b)
{
    unsigned int bits;

    pixels += (bitmap->bpp / 8) * x + bitmap->pitch * y;

    switch(bitmap->bpp / 8)
    {
        case 4:
            bits = ((uint32_t)pixels[0] << 24) |
                   ((uint32_t)pixels[1] << 16) |
                   ((uint32_t)pixels[2] << 8) |
                   ((uint32_t)pixels[3]);
            break;
        case 3:
            bits = ((uint32_t)pixels[0] << 16) |
                   ((uint32_t)pixels[1] << 8) |
                   ((uint32_t)pixels[2]);
            break;
        case 2:
            bits = ((uint16_t)pixels[0] << 8) |
                   ((uint16_t)pixels[1]);
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

    delta = max - min; /* 0 - 0xffff */
    *val = max; /* 0 - 0xffff */

    if(delta)
    {
        *sat = 0x1000 * delta / max; /* 0 - 0xfff */

        /* Generate *hue between 0 and 0x5fff */
        if( r == max )
            *hue = 0x1000 + 0x1000 * (g - b) / delta;
        else if( g == max )
            *hue = 0x3000 + 0x1000 * (b - r) / delta;
        else
            *hue = 0x5000 + 0x1000 * (r - g) / delta;
    }
    else
    {
        *sat = 0;
        *hue = 0;
    }
}

void caca_draw_bitmap(int x1, int y1, int x2, int y2,
                      struct caca_bitmap *bitmap, char *pixels)
{
#if !NEW_RENDERER
    static const int white_colors[] =
    {
        CACA_COLOR_BLACK,
        CACA_COLOR_DARKGRAY,
        CACA_COLOR_LIGHTGRAY,
        CACA_COLOR_WHITE
    };
#endif

    static const int light_colors[] =
    {
        CACA_COLOR_LIGHTMAGENTA,
        CACA_COLOR_LIGHTRED,
        CACA_COLOR_YELLOW,
        CACA_COLOR_LIGHTGREEN,
        CACA_COLOR_LIGHTCYAN,
        CACA_COLOR_LIGHTBLUE,
        CACA_COLOR_LIGHTMAGENTA
    };

    static const int dark_colors[] =
    {
        CACA_COLOR_MAGENTA,
        CACA_COLOR_RED,
        CACA_COLOR_BROWN,
        CACA_COLOR_GREEN,
        CACA_COLOR_CYAN,
        CACA_COLOR_BLUE,
        CACA_COLOR_MAGENTA
    };

    /* FIXME: choose better characters! */
#   define DENSITY_CHARS 13
    static const char density_chars[] =
        "    "
        "    "
        ".`  "
        ",`.'"
        "i:-^"
        "|=+;"
        "ox/\\"
        "<>x%"
        "&$zw"
        "WXKM"
        "#8##"
        "8@8#"
        "@8@8"
        "????";

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
        for(x = x1 > 0 ? x1 : 0, _init_dither(y);
            x <= x2 && x <= (int)caca_get_width();
            x++)
    {
        int ch;
        unsigned int r, g, b, R, G, B;
        int hue, sat, val;
        int fromx = w * (x - x1) / (x2 - x1 + 1);
        int fromy = h * (y - y1) / (y2 - y1 + 1);
        enum caca_color outfg, outbg;
        char outch;
#if NEW_RENDERER
        int distbg, distfg, dist, hue1, hue2;
#endif

        /* Clip values (yuck) */
        if(fromx == 0) fromx = 1;
        if(fromy == 0) fromy = 1;

        /* First get RGB */
        R = 0, G = 0, B = 0;
        get_rgb_default(bitmap, pixels, fromx, fromy, &r, &g, &b);
#if 1
        R += r; G += g; B += b;
#else
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
#endif

        /* Now get HSV from RGB */
        rgb2hsv_default(R, G, B, &hue, &sat, &val);

        /* The hard work: calculate foreground and background colours,
         * as well as the most appropriate character to output. */
#if NEW_RENDERER
#       define XRATIO 5*5
#       define YRATIO 3*3
#       define HRATIO 2*2
#       define FUZZINESS XRATIO * 0x800

        /* distance to black */
        distbg = XRATIO * val * val;
        distbg += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        outbg = CACA_COLOR_BLACK;

        /* distance to 30% */
        dist = XRATIO * (val - 0x600) * (val - 0x600)
             + YRATIO * sat * sat;
        dist += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        dist = dist * 3 / 2;
        if(dist <= distbg)
        {
            outfg = outbg;
            distfg = distbg;
            outbg = CACA_COLOR_DARKGRAY;
            distbg = dist;
        }
        else
        {
            outfg = CACA_COLOR_DARKGRAY;
            distfg = dist;
        }

        /* check dist to 70% */
        dist = XRATIO * (val - 0xa00) * (val - 0xa00)
             + YRATIO * sat * sat;
        dist += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        dist = dist * 3 / 2;
        if(dist <= distbg)
        {
            outfg = outbg;
            distfg = distbg;
            outbg = CACA_COLOR_LIGHTGRAY;
            distbg = dist;
        }
        else if(dist <= distfg)
        {
            outfg = CACA_COLOR_LIGHTGRAY;
            distfg = dist;
        }

        /* check dist to white */
        dist = XRATIO * (val - 0x1000) * (val - 0x1000)
             + YRATIO * sat * sat;
        dist += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        if(dist <= distbg)
        {
            outfg = outbg;
            distfg = distbg;
            outbg = CACA_COLOR_WHITE;
            distbg = dist;
        }
        else if(dist <= distfg)
        {
            outfg = CACA_COLOR_WHITE;
            distfg = dist;
        }

        hue1 = (hue + 0x800) & ~0xfff;
        if(hue > hue1)
            hue2 = (hue + 0x1800) & ~0xfff;
        else
            hue2 = (hue - 0x800) & ~0xfff;

        /* check dist to 2nd closest dark color */
        dist = XRATIO * (val - 0x600) * (val - 0x600)
             + YRATIO * (sat - 0x1000) * (sat - 0x1000)
             + HRATIO * (hue - hue2) * (hue - hue2);
        dist += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        dist = dist * 3 / 4;
        if(dist <= distbg)
        {
            outfg = outbg;
            distfg = distbg;
            outbg = dark_colors[hue2 / 0x1000];
            distbg = dist;
        }
        else if(dist <= distfg)
        {
            outfg = dark_colors[hue2 / 0x1000];
            distfg = dist;
        }

        /* check dist to 2nd closest light color */
        dist = XRATIO * (val - 0x1000) * (val - 0x1000)
             + YRATIO * (sat - 0x1000) * (sat - 0x1000)
             + HRATIO * (hue - hue2) * (hue - hue2);
        dist += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        dist = dist / 2;
        if(dist <= distbg)
        {
            outfg = outbg;
            distfg = distbg;
            outbg = light_colors[hue2 / 0x1000];
            distbg = dist;
        }
        else if(dist <= distfg)
        {
            outfg = light_colors[hue2 / 0x1000];
            distfg = dist;
        }

        /* check dist to closest dark color */
        dist = XRATIO * (val - 0x600) * (val - 0x600)
             + YRATIO * (sat - 0x1000) * (sat - 0x1000)
             + HRATIO * (hue - hue1) * (hue - hue1);
        dist += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        dist = dist * 3 / 4;
        if(dist <= distbg)
        {
            outfg = outbg;
            distfg = distbg;
            outbg = dark_colors[hue1 / 0x1000];
            distbg = dist;
        }
        else if(dist <= distfg)
        {
            outfg = dark_colors[hue1 / 0x1000];
            distfg = dist;
        }

        /* check dist to closest light color */
        dist = XRATIO * (val - 0x1000) * (val - 0x1000)
             + YRATIO * (sat - 0x1000) * (sat - 0x1000)
             + HRATIO * (hue - hue1) * (hue - hue1);
        dist += FUZZINESS * _get_dither() - 0x80 * FUZZINESS;
        dist = dist / 2;
        if(dist <= distbg)
        {
            outfg = outbg;
            distfg = distbg;
            outbg = light_colors[hue1 / 0x1000];
            distbg = dist;
        }
        else if(dist <= distfg)
        {
            outfg = light_colors[hue1 / 0x1000];
            distfg = dist;
        }

        if(distbg <= 0) distbg = 1;
        if(distfg <= 0) distfg = 1;

        /* distbg can be > distfg because of dithering fuzziness */
        ch = distbg * 2 * (DENSITY_CHARS - 1) / (distbg + distfg);
        ch = 4 * ch + _get_dither() / 0x40;
        outch = density_chars[ch];

#else
        outbg = CACA_COLOR_BLACK;
        if((unsigned int)sat < 0x200 + _get_dither() * 0x8)
            outfg = white_colors[1 + (val * 2 + _get_dither() * 0x10) / 0x1000];
        else if((unsigned int)val > 0x800 + _get_dither() * 0x4)
            outfg = light_colors[(hue + _get_dither() * 0x10) / 0x1000];
        else
            outfg = dark_colors[(hue + _get_dither() * 0x10) / 0x1000];

        ch = (val + 0x2 * _get_dither()) * 10 / 0x1000;
        ch = 4 * ch + _get_dither() / 0x40;
        outch = density_chars[ch];

#endif

        /* Now output the character */
        caca_set_color(outfg, outbg);
        caca_putchar(x, y, outch);

        _increment_dither();
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

static unsigned int get_no_dither(void)
{
    return 0x80;
}

static void increment_no_dither(void)
{
    return;
}

/*
 * Ordered 2 dithering
 */
static unsigned int *ordered2_table;
static unsigned int ordered2_index;

static void init_ordered2_dither(int line)
{
    static unsigned int dither2x2[] =
    {
        0x00, 0x80,
        0xc0, 0x40,
    };

    ordered2_table = dither2x2 + (line % 2) * 2;
    ordered2_index = 0;
}

static unsigned int get_ordered2_dither(void)
{
    return ordered2_table[ordered2_index];
}

static void increment_ordered2_dither(void)
{
    ordered2_index = (ordered2_index + 1) % 2;
}

/*
 * Ordered 4 dithering
 */
/*static int dither4x4[] = { 5,  0,  1,  6,
                          -1, -6, -5,  2,
                          -2, -7, -8,  3,
                           4, -3, -4, -7};*/
static unsigned int *ordered4_table;
static unsigned int ordered4_index;

static void init_ordered4_dither(int line)
{
    static unsigned int dither4x4[] =
    {
        0x00, 0x80, 0x20, 0xa0,
        0xc0, 0x40, 0xe0, 0x60,
        0x30, 0xb0, 0x10, 0x90,
        0xf0, 0x70, 0xd0, 0x50
    };

    ordered4_table = dither4x4 + (line % 4) * 4;
    ordered4_index = 0;
}

static unsigned int get_ordered4_dither(void)
{
    return ordered4_table[ordered4_index];
}

static void increment_ordered4_dither(void)
{
    ordered4_index = (ordered4_index + 1) % 4;
}

/*
 * Ordered 8 dithering
 */
static unsigned int *ordered8_table;
static unsigned int ordered8_index;

static void init_ordered8_dither(int line)
{
    static unsigned int dither8x8[] =
    {
        0x00, 0x80, 0x20, 0xa0, 0x08, 0x88, 0x28, 0xa8,
        0xc0, 0x40, 0xe0, 0x60, 0xc8, 0x48, 0xe8, 0x68,
        0x30, 0xb0, 0x10, 0x90, 0x38, 0xb8, 0x18, 0x98,
        0xf0, 0x70, 0xd0, 0x50, 0xf8, 0x78, 0xd8, 0x58,
        0x0c, 0x8c, 0x2c, 0xac, 0x04, 0x84, 0x24, 0xa4,
        0xcc, 0x4c, 0xec, 0x6c, 0xc4, 0x44, 0xe4, 0x64,
        0x3c, 0xbc, 0x1c, 0x9c, 0x34, 0xb4, 0x14, 0x94,
        0xfc, 0x7c, 0xdc, 0x5c, 0xf4, 0x74, 0xd4, 0x54,
    };

    ordered8_table = dither8x8 + (line % 8) * 8;
    ordered8_index = 0;
}

static unsigned int get_ordered8_dither(void)
{
    return ordered8_table[ordered8_index];
}

static void increment_ordered8_dither(void)
{
    ordered8_index = (ordered8_index + 1) % 8;
}

/*
 * Random dithering
 */
static void init_random_dither(int line)
{
    ;
}

static unsigned int get_random_dither(void)
{
    return caca_rand(0x00, 0xff);
}

static void increment_random_dither(void)
{
    return;
}

