/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

/** \file bitmap.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Bitmap blitting
 *
 *  This file contains bitmap blitting functions.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H) || defined(_DOXYGEN_SKIP_ME)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#if defined(HAVE_ENDIAN_H)
#   include <endian.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "caca.h"
#include "caca_internals.h"

/*
 * Global variables
 */
#if !defined(_DOXYGEN_SKIP_ME)
enum caca_feature _caca_background;
enum caca_feature _caca_dithering;
enum caca_feature _caca_antialiasing;
#endif

/*
 * Local variables
 */
#if !defined(_DOXYGEN_SKIP_ME)
#   define LOOKUP_VAL 32
#   define LOOKUP_SAT 32
#   define LOOKUP_HUE 16
#endif
static unsigned char hsv_distances[LOOKUP_VAL][LOOKUP_SAT][LOOKUP_HUE];
static enum caca_color lookup_colors[8];

static int const hsv_palette[] =
{
    /* weight, hue, saturation, value */
    4,    0x0,    0x0,    0x0,   /* black */
    5,    0x0,    0x0,    0x5ff, /* 30% */
    5,    0x0,    0x0,    0x9ff, /* 70% */
    4,    0x0,    0x0,    0xfff, /* white */
    3,    0x1000, 0xfff,  0x5ff, /* dark yellow */
    2,    0x1000, 0xfff,  0xfff, /* light yellow */
    3,    0x0,    0xfff,  0x5ff, /* dark red */
    2,    0x0,    0xfff,  0xfff  /* light red */
};

/* RGB palette for the new colour picker */
static int rgb_palette[] =
{
    0x0,   0x0,   0x0,
    0x0,   0x0,   0x7ff,
    0x0,   0x7ff, 0x0,
    0x0,   0x7ff, 0x7ff,
    0x7ff, 0x0,   0x0,
    0x7ff, 0x0,   0x7ff,
    0x7ff, 0x7ff, 0x0,
    0x7ff, 0x7ff, 0x7ff,
    0x3ff, 0x3ff, 0x3ff,
    0x000, 0x000, 0xfff,
    0x000, 0xfff, 0x000,
    0x000, 0xfff, 0xfff,
    0xfff, 0x000, 0x000,
    0xfff, 0x000, 0xfff,
    0xfff, 0xfff, 0x000,
    0xfff, 0xfff, 0xfff,
};

static int rgb_weight[] =
{
    //2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

#if !defined(_DOXYGEN_SKIP_ME)
#define HSV_XRATIO 6
#define HSV_YRATIO 3
#define HSV_HRATIO 3

#define HSV_DISTANCE(h, s, v, index) \
    (hsv_palette[index * 4] \
     * ((HSV_XRATIO * ((v) - hsv_palette[index * 4 + 3]) \
                    * ((v) - hsv_palette[index * 4 + 3])) \
       + (hsv_palette[index * 4 + 3] \
           ? (HSV_YRATIO * ((s) - hsv_palette[index * 4 + 2]) \
                         * ((s) - hsv_palette[index * 4 + 2])) \
           : 0) \
       + (hsv_palette[index * 4 + 2] \
           ? (HSV_HRATIO * ((h) - hsv_palette[index * 4 + 1]) \
                         * ((h) - hsv_palette[index * 4 + 1])) \
           : 0)))
#endif

/*
 * Local prototypes
 */
static void mask2shift(unsigned int, int *, int *);

static void get_rgba_default(struct caca_bitmap const *, uint8_t *, int, int,
                             unsigned int *, unsigned int *, unsigned int *,
                             unsigned int *);
static inline void rgb2hsv_default(int, int, int, int *, int *, int *);
static inline int sq(int);

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

#if !defined(_DOXYGEN_SKIP_ME)
struct caca_bitmap
{
    int bpp, has_palette, has_alpha;
    int w, h, pitch;
    int rmask, gmask, bmask, amask;
    int rright, gright, bright, aright;
    int rleft, gleft, bleft, aleft;
    void (*get_hsv)(struct caca_bitmap *, char *, int, int);
    int red[256], green[256], blue[256], alpha[256];
};
#endif

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

/**
 * \brief Create an internal bitmap object.
 *
 * Create a bitmap structure from its coordinates (depth, width, height and
 * pitch) and pixel mask values. If the depth is 8 bits per pixel, the mask
 * values are ignored and the colour palette should be set using the
 * caca_set_bitmap_palette() function. For depths greater than 8 bits per
 * pixel, a zero alpha mask causes the alpha values to be ignored.
 *
 * \param bpp Bitmap depth in bits per pixel.
 * \param w Bitmap width in pixels.
 * \param h Bitmap height in pixels.
 * \param pitch Bitmap pitch in bytes.
 * \param rmask Bitmask for red values.
 * \param gmask Bitmask for green values.
 * \param bmask Bitmask for blue values.
 * \param amask Bitmask for alpha values.
 * \return Bitmap object, or NULL upon error.
 */
struct caca_bitmap *caca_create_bitmap(unsigned int bpp, unsigned int w,
                                       unsigned int h, unsigned int pitch,
                                       unsigned int rmask, unsigned int gmask,
                                       unsigned int bmask, unsigned int amask)
{
    struct caca_bitmap *bitmap;

    /* Minor sanity test */
    if(!w || !h || !pitch || bpp > 32 || bpp < 8)
        return NULL;

    bitmap = malloc(sizeof(struct caca_bitmap));
    if(!bitmap)
        return NULL;

    bitmap->bpp = bpp;
    bitmap->has_palette = 0;
    bitmap->has_alpha = amask ? 1 : 0;

    bitmap->w = w;
    bitmap->h = h;
    bitmap->pitch = pitch;

    bitmap->rmask = rmask;
    bitmap->gmask = gmask;
    bitmap->bmask = bmask;
    bitmap->amask = amask;

    /* Load bitmasks */
    if(rmask || gmask || bmask || amask)
    {
        mask2shift(rmask, &bitmap->rright, &bitmap->rleft);
        mask2shift(gmask, &bitmap->gright, &bitmap->gleft);
        mask2shift(bmask, &bitmap->bright, &bitmap->bleft);
        mask2shift(amask, &bitmap->aright, &bitmap->aleft);
    }

    /* In 8 bpp mode, default to a grayscale palette */
    if(bpp == 8)
    {
        int i;
        bitmap->has_palette = 1;
        bitmap->has_alpha = 0;
        for(i = 0; i < 256; i++)
        {
            bitmap->red[i] = i * 0xfff / 256;
            bitmap->green[i] = i * 0xfff / 256;
            bitmap->blue[i] = i * 0xfff / 256;
        }
    }

    return bitmap;
}

/**
 * \brief Set the palette of an 8bpp bitmap object.
 *
 * Set the palette of an 8 bits per pixel bitmap. Values should be between
 * 0 and 4095 (0xfff).
 *
 * \param bitmap Bitmap object.
 * \param red Array of 256 red values.
 * \param green Array of 256 green values.
 * \param blue Array of 256 blue values.
 * \param alpha Array of 256 alpha values.
 */
void caca_set_bitmap_palette(struct caca_bitmap *bitmap,
                             unsigned int red[], unsigned int green[],
                             unsigned int blue[], unsigned int alpha[])
{
    int i, has_alpha = 0;

    if(bitmap->bpp != 8)
        return;

    for(i = 0; i < 256; i++)
    {
        if(red[i] >= 0 && red[i] < 0x1000 &&
           green[i] >= 0 && green[i] < 0x1000 &&
           blue[i] >= 0 && blue[i] < 0x1000 &&
           alpha[i] >= 0 && alpha[i] < 0x1000)
        {
            bitmap->red[i] = red[i];
            bitmap->green[i] = green[i];
            bitmap->blue[i] = blue[i];
            if(alpha[i])
            {
                bitmap->alpha[i] = alpha[i];
                has_alpha = 1;
            }
        }
    }

    bitmap->has_alpha = has_alpha;
}

/**
 * \brief Free the memory associated with a bitmap.
 *
 * Free the memory allocated by caca_create_bitmap().
 *
 * \param bitmap Bitmap object.
 */
void caca_free_bitmap(struct caca_bitmap *bitmap)
{
    if(!bitmap)
        return;

    free(bitmap);
}

static void get_rgba_default(struct caca_bitmap const *bitmap, uint8_t *pixels,
                             int x, int y, unsigned int *r, unsigned int *g,
                             unsigned int *b, unsigned int *a)
{
    uint32_t bits;

    pixels += (bitmap->bpp / 8) * x + bitmap->pitch * y;

    switch(bitmap->bpp / 8)
    {
        case 4:
            bits = *(uint32_t *)pixels;
            break;
        case 3:
        {
#if defined(HAVE_ENDIAN_H)
            if(__BYTE_ORDER == __BIG_ENDIAN)
#else
            /* This is compile-time optimised with at least -O1 or -Os */
            uint32_t const rmask = 0x12345678;
            if(*(uint8_t const *)&rmask == 0x12)
#endif
                bits = ((uint32_t)pixels[0] << 16) |
                       ((uint32_t)pixels[1] << 8) |
                       ((uint32_t)pixels[2]);
            else
                bits = ((uint32_t)pixels[2] << 16) |
                       ((uint32_t)pixels[1] << 8) |
                       ((uint32_t)pixels[0]);
            break;
        }
        case 2:
            bits = *(uint16_t *)pixels;
            break;
        case 1:
        default:
            bits = pixels[0];
            break;
    }

    if(bitmap->has_palette)
    {
        *r += bitmap->red[bits];
        *g += bitmap->green[bits];
        *b += bitmap->blue[bits];
        *a += bitmap->alpha[bits];
    }
    else
    {
        *r += ((bits & bitmap->rmask) >> bitmap->rright) << bitmap->rleft;
        *g += ((bits & bitmap->gmask) >> bitmap->gright) << bitmap->gleft;
        *b += ((bits & bitmap->bmask) >> bitmap->bright) << bitmap->bleft;
        *a += ((bits & bitmap->amask) >> bitmap->aright) << bitmap->aleft;
    }
}

static inline void rgb2hsv_default(int r, int g, int b,
                                   int *hue, int *sat, int *val)
{
    int min, max, delta;

    min = r; max = r;
    if(min > g) min = g; if(max < g) max = g;
    if(min > b) min = b; if(max < b) max = b;

    delta = max - min; /* 0 - 0xfff */
    *val = max; /* 0 - 0xfff */

    if(delta)
    {
        *sat = 0xfff * delta / max; /* 0 - 0xfff */

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

static inline int sq(int x)
{
    return x * x;
}

/**
 * \brief Draw a bitmap on the screen.
 *
 * Draw a bitmap at the given coordinates. The bitmap can be of any size and
 * will be stretched to the text area.
 *
 * \param x1 X coordinate of the upper-left corner of the drawing area.
 * \param y1 Y coordinate of the upper-left corner of the drawing area.
 * \param x2 X coordinate of the lower-right corner of the drawing area.
 * \param y2 Y coordinate of the lower-right corner of the drawing area.
 * \param bitmap Bitmap object to be drawn.
 * \param pixels Bitmap's pixels.
 */
void caca_draw_bitmap(int x1, int y1, int x2, int y2,
                      struct caca_bitmap const *bitmap, void *pixels)
{
    /* Current dithering method */
    void (*_init_dither) (int);
    unsigned int (*_get_dither) (void);
    void (*_increment_dither) (void);

    int *floyd_steinberg, *fs_r, *fs_g, *fs_b;
    int fs_length;

    /* Only used when background is black */
    static int const white_colors[] =
    {
        CACA_COLOR_BLACK,
        CACA_COLOR_DARKGRAY,
        CACA_COLOR_LIGHTGRAY,
        CACA_COLOR_WHITE
    };

    static int const light_colors[] =
    {
        CACA_COLOR_LIGHTMAGENTA,
        CACA_COLOR_LIGHTRED,
        CACA_COLOR_YELLOW,
        CACA_COLOR_LIGHTGREEN,
        CACA_COLOR_LIGHTCYAN,
        CACA_COLOR_LIGHTBLUE,
        CACA_COLOR_LIGHTMAGENTA
    };

    static int const dark_colors[] =
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
#if !defined(_DOXYGEN_SKIP_ME)
#   define DCHMAX ((sizeof(density_chars)/sizeof(char const)/4)-1)
#endif
    static char const density_chars[] =
        "    "
        "...."
        "::::"
        ";=;="
        "tftf"
        "%$%$"
        "SK&Z"
        "XWGM"
        "@@@@"
        "8888"
        "####"
        "????";

    int x, y, w, h, pitch, deltax, deltay;

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

    deltax = x2 - x1 + 1;
    deltay = y2 - y1 + 1;

    switch(_caca_dithering)
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

    case CACA_DITHERING_FSTEIN:
        _init_dither = init_no_dither;
        _get_dither = get_no_dither;
        _increment_dither = increment_no_dither;
        break;

    default:
        /* Something wicked happened! */
        return;
    }

    fs_length = ((int)_caca_width <= x2 ? (int)_caca_width : x2) + 1;
    floyd_steinberg = malloc(3 * (fs_length + 2) * sizeof(int));
    memset(floyd_steinberg, 0, 3 * (fs_length + 2) * sizeof(int));
    fs_r = floyd_steinberg + 1;
    fs_g = fs_r + fs_length + 2;
    fs_b = fs_g + fs_length + 2;

    for(y = y1 > 0 ? y1 : 0; y <= y2 && y <= (int)_caca_height; y++)
    {
        int remain_r = 0, remain_g = 0, remain_b = 0;
        int remain_r_next = 0, remain_g_next = 0, remain_b_next = 0;

        for(x = x1 > 0 ? x1 : 0, _init_dither(y);
            x <= x2 && x <= (int)_caca_width;
            x++)
    {
        unsigned int i;
        int ch = 0, distmin;
        int r, g, b, a, fg_r, fg_g, fg_b, bg_r, bg_g, bg_b;
        int fromx, fromy, tox, toy, myx, myy, dots, dist;

        enum caca_color outfg = 0, outbg = 0;
        char outch;

        r = g = b = a = 0;

        /* First get RGB */
        if(_caca_antialiasing == CACA_ANTIALIASING_PREFILTER)
        {
            fromx = (x - x1) * w / deltax;
            fromy = (y - y1) * h / deltay;
            tox = (x - x1 + 1) * w / deltax;
            toy = (y - y1 + 1) * h / deltay;

            /* We want at least one pixel */
            if(tox == fromx) tox++;
            if(toy == fromy) toy++;

            dots = 0;

            for(myx = fromx; myx < tox; myx++)
                for(myy = fromy; myy < toy; myy++)
            {
                dots++;
                get_rgba_default(bitmap, pixels, myx, myy, &r, &g, &b, &a);
            }

            /* Normalize */
            r /= dots;
            g /= dots;
            b /= dots;
            a /= dots;
        }
        else
        {
            fromx = (x - x1) * w / deltax;
            fromy = (y - y1) * h / deltay;
            tox = (x - x1 + 1) * w / deltax;
            toy = (y - y1 + 1) * h / deltay;

            /* tox and toy can overflow the screen, but they cannot overflow
             * when averaged with fromx and fromy because these are guaranteed
             * to be within the pixel boundaries. */
            myx = (fromx + tox) / 2;
            myy = (fromy + toy) / 2;

            get_rgba_default(bitmap, pixels, myx, myy, &r, &g, &b, &a);
        }

        if(bitmap->has_alpha && a < 0x800)
        {
            remain_r = remain_g = remain_b = 0;
            fs_r[x] = 0;
            fs_g[x] = 0;
            fs_b[x] = 0;
            continue;
        }

        if(_caca_dithering == CACA_DITHERING_FSTEIN)
        {
            r += remain_r;
            g += remain_g;
            b += remain_b;
            r += remain_r_next;
            g += remain_g_next;
            b += remain_b_next;
            remain_r_next = fs_r[x+1];
            remain_g_next = fs_g[x+1];
            remain_b_next = fs_b[x+1];
        }
        else
        {
            r += (_get_dither() - 0x80) * 4;
            g += (_get_dither() - 0x80) * 4;
            b += (_get_dither() - 0x80) * 4;
        }

        distmin = INT_MAX;
        for(i = 0; i < 16; i++)
        {
            dist = sq(r - rgb_palette[i * 3])
                 + sq(g - rgb_palette[i * 3 + 1])
                 + sq(b - rgb_palette[i * 3 + 2]);
            dist *= rgb_weight[i];
            if(dist < distmin)
            {
                outbg = i;
                distmin = dist;
            }
        }
        bg_r = rgb_palette[outbg * 3];
        bg_g = rgb_palette[outbg * 3 + 1];
        bg_b = rgb_palette[outbg * 3 + 2];

        distmin = INT_MAX;
        for(i = 0; i < 16; i++)
        {
            if(i == outbg)
                continue;
            dist = sq(r - rgb_palette[i * 3])
                 + sq(g - rgb_palette[i * 3 + 1])
                 + sq(b - rgb_palette[i * 3 + 2]);
            dist *= rgb_weight[i];
            if(dist < distmin)
            {
                outfg = i;
                distmin = dist;
            }
        }
        fg_r = rgb_palette[outfg * 3];
        fg_g = rgb_palette[outfg * 3 + 1];
        fg_b = rgb_palette[outfg * 3 + 2];

        distmin = INT_MAX;
        for(i = 0; i < DCHMAX - 1; i++)
	{
            int newr = i * fg_r + ((2*DCHMAX-1) - i) * bg_r;
            int newg = i * fg_g + ((2*DCHMAX-1) - i) * bg_g;
            int newb = i * fg_b + ((2*DCHMAX-1) - i) * bg_b;
            dist = abs(r * (2*DCHMAX-1) - newr)
                 + abs(g * (2*DCHMAX-1) - newg)
                 + abs(b * (2*DCHMAX-1) - newb);

            if(dist < distmin)
            {
                ch = i;
                distmin = dist;
            }
        }
        outch = density_chars[4 * ch];

        if(_caca_dithering == CACA_DITHERING_FSTEIN)
        {
            remain_r = r - (fg_r * ch + bg_r * ((2*DCHMAX-1) - ch)) / (2*DCHMAX-1);
            remain_g = g - (fg_g * ch + bg_g * ((2*DCHMAX-1) - ch)) / (2*DCHMAX-1);
            remain_b = b - (fg_b * ch + bg_b * ((2*DCHMAX-1) - ch)) / (2*DCHMAX-1);
            remain_r_next = fs_r[x+1];
            remain_g_next = fs_g[x+1];
            remain_b_next = fs_b[x+1];
            fs_r[x-1] += 3 * remain_r / 16;
            fs_g[x-1] += 3 * remain_g / 16;
            fs_b[x-1] += 3 * remain_b / 16;
            fs_r[x] = 5 * remain_r / 16;
            fs_g[x] = 5 * remain_g / 16;
            fs_b[x] = 5 * remain_b / 16;
            fs_r[x+1] = 1 * remain_r / 16;
            fs_g[x+1] = 1 * remain_g / 16;
            fs_b[x+1] = 1 * remain_b / 16;
            remain_r = 7 * remain_r / 16;
            remain_g = 7 * remain_g / 16;
            remain_b = 7 * remain_b / 16;
        }

        /* Now output the character */
        caca_set_color(outfg, outbg);
        caca_putchar(x, y, outch);

        _increment_dither();
    }
        /* end loop */
    }

    free(floyd_steinberg);
}

#if !defined(_DOXYGEN_SKIP_ME)
int _caca_init_bitmap(void)
{
    unsigned int v, s, h;

    /* These ones are constant */
    lookup_colors[0] = CACA_COLOR_BLACK;
    lookup_colors[1] = CACA_COLOR_DARKGRAY;
    lookup_colors[2] = CACA_COLOR_LIGHTGRAY;
    lookup_colors[3] = CACA_COLOR_WHITE;

    /* These ones will be overwritten */
    lookup_colors[4] = CACA_COLOR_MAGENTA;
    lookup_colors[5] = CACA_COLOR_LIGHTMAGENTA;
    lookup_colors[6] = CACA_COLOR_RED;
    lookup_colors[7] = CACA_COLOR_LIGHTRED;

    for(v = 0; v < LOOKUP_VAL; v++)
        for(s = 0; s < LOOKUP_SAT; s++)
            for(h = 0; h < LOOKUP_HUE; h++)
    {
        int i, distbg, distfg, dist;
        int val, sat, hue;
        unsigned char outbg, outfg;

        val = 0xfff * v / (LOOKUP_VAL - 1);
        sat = 0xfff * s / (LOOKUP_SAT - 1);
        hue = 0xfff * h / (LOOKUP_HUE - 1);

        /* Initialise distances to the distance between pure black HSV
         * coordinates and our white colour (3) */
        outbg = outfg = 3;
        distbg = distfg = HSV_DISTANCE(0, 0, 0, 3);

        /* Calculate distances to eight major colour values and store the
         * two nearest points in our lookup table. */
        for(i = 0; i < 8; i++)
        {
            dist = HSV_DISTANCE(hue, sat, val, i);
            if(dist <= distbg)
            {
                outfg = outbg;
                distfg = distbg;
                outbg = i;
                distbg = dist;
            }
            else if(dist <= distfg)
            {
                outfg = i;
                distfg = dist;
            }
        }

        hsv_distances[v][s][h] = (outfg << 4) | outbg;
    }

    return 0;
}

int _caca_end_bitmap(void)
{
    return 0;
}
#endif /* _DOXYGEN_SKIP_ME */

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

