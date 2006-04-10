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

/** \file bitmap.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Bitmap blitting
 *
 *  This file contains bitmap blitting functions.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   if defined(HAVE_ENDIAN_H)
#       include <endian.h>
#   endif
#   include <stdio.h>
#   include <stdlib.h>
#   include <limits.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

#define CP437 0

/*
 * Local variables
 */
#if !defined(_DOXYGEN_SKIP_ME)
#   define LOOKUP_VAL 32
#   define LOOKUP_SAT 32
#   define LOOKUP_HUE 16
#endif
static unsigned char hsv_distances[LOOKUP_VAL][LOOKUP_SAT][LOOKUP_HUE];
static uint16_t lookup_colors[8];

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
static int const rgb_palette[] =
{
    0x0,   0x0,   0x0,
    0x0,   0x0,   0x7ff,
    0x0,   0x7ff, 0x0,
    0x0,   0x7ff, 0x7ff,
    0x7ff, 0x0,   0x0,
    0x7ff, 0x0,   0x7ff,
    0x7ff, 0x7ff, 0x0,
    0xaaa, 0xaaa, 0xaaa,
    0x555, 0x555, 0x555,
    0x000, 0x000, 0xfff,
    0x000, 0xfff, 0x000,
    0x000, 0xfff, 0xfff,
    0xfff, 0x000, 0x000,
    0xfff, 0x000, 0xfff,
    0xfff, 0xfff, 0x000,
    0xfff, 0xfff, 0xfff,
};

static int const rgb_weight[] =
{
    //2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* List of glyphs */
static char const * ascii_glyphs[] =
{
    " ", ".", ":", ";", "t", "%", "S", "X", "@", "8", "?"
};

static char const * shades_glyphs[] =
{
    " ", ":", "░", "▒", "?"
};

static char const * blocks_glyphs[] =
{
    " ", "▘", "▚", "?"
};

#if !defined(_DOXYGEN_SKIP_ME)
enum color_mode
{
    COLOR_MODE_MONO,
    COLOR_MODE_GRAY,
    COLOR_MODE_8,
    COLOR_MODE_16,
    COLOR_MODE_FULLGRAY,
    COLOR_MODE_FULL8,
    COLOR_MODE_FULL16,
};

struct cucul_bitmap
{
    int bpp, has_palette, has_alpha;
    int w, h, pitch;
    int rmask, gmask, bmask, amask;
    int rright, gright, bright, aright;
    int rleft, gleft, bleft, aleft;
    void (*get_hsv)(struct cucul_bitmap *, char *, int, int);
    int red[256], green[256], blue[256], alpha[256];
    float gamma;
    int gammatab[4097];

    /* Bitmap features */
    int invert, antialias;

    /* Colour mode used for rendering */
    enum color_mode color_mode;

    /* Glyphs used for rendering */
    char const * const * glyphs;
    unsigned glyph_count;

    /* Current dithering method */
    void (*init_dither) (int);
    unsigned int (*get_dither) (void);
    void (*increment_dither) (void);
};

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
static float gammapow(float x, float y);

static void get_rgba_default(struct cucul_bitmap const *, uint8_t *, int, int,
                             unsigned int *);

/* Dithering methods */
static void init_no_dither(int);
static unsigned int get_no_dither(void);
static void increment_no_dither(void);

static void init_fstein_dither(int);
static unsigned int get_fstein_dither(void);
static void increment_fstein_dither(void);

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

static inline int sq(int x)
{
    return x * x;
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

/**
 * \brief Create an internal bitmap object.
 *
 * Create a bitmap structure from its coordinates (depth, width, height and
 * pitch) and pixel mask values. If the depth is 8 bits per pixel, the mask
 * values are ignored and the colour palette should be set using the
 * cucul_set_bitmap_palette() function. For depths greater than 8 bits per
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
struct cucul_bitmap *cucul_create_bitmap(unsigned int bpp, unsigned int w,
                                         unsigned int h, unsigned int pitch,
                                         unsigned int rmask, unsigned int gmask,
                                         unsigned int bmask, unsigned int amask)
{
    struct cucul_bitmap *bitmap;
    int i;

    /* Minor sanity test */
    if(!w || !h || !pitch || bpp > 32 || bpp < 8)
        return NULL;

    bitmap = malloc(sizeof(struct cucul_bitmap));
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
        bitmap->has_palette = 1;
        bitmap->has_alpha = 0;
        for(i = 0; i < 256; i++)
        {
            bitmap->red[i] = i * 0xfff / 256;
            bitmap->green[i] = i * 0xfff / 256;
            bitmap->blue[i] = i * 0xfff / 256;
        }
    }

    /* Default features */
    bitmap->invert = 0;
    bitmap->antialias = 1;

    /* Default gamma value */
    for(i = 0; i < 4096; i++)
        bitmap->gammatab[i] = i;

    /* Default colour mode */
    bitmap->color_mode = COLOR_MODE_FULL16;

    /* Default character set */
    bitmap->glyphs = ascii_glyphs;
    bitmap->glyph_count = sizeof(ascii_glyphs) / sizeof(*ascii_glyphs);

    /* Default dithering mode */
    bitmap->init_dither = init_fstein_dither;
    bitmap->get_dither = get_fstein_dither;
    bitmap->increment_dither = increment_fstein_dither;

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
void cucul_set_bitmap_palette(struct cucul_bitmap *bitmap,
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
 * \brief Set the brightness of a bitmap object.
 *
 * Set the brightness of bitmap.
 *
 * \param bitmap Bitmap object.
 * \param brightness brightness value.
 */
void cucul_set_bitmap_brightness(struct cucul_bitmap *bitmap, float brightness)
{
    /* FIXME */
}

/**
 * \brief Set the gamma of a bitmap object.
 *
 * Set the gamma of bitmap.
 *
 * \param bitmap Bitmap object.
 * \param gamma Gamma value.
 */
void cucul_set_bitmap_gamma(struct cucul_bitmap *bitmap, float gamma)
{
    /* FIXME: we don't need 4096 calls to gammapow(), we can just compute
     * 128 of them and do linear interpolation for the rest. This will
     * probably speed up things a lot. */
    int i;

    if(gamma <= 0.0)
        return;

    bitmap->gamma = gamma;

    for(i = 0; i < 4096; i++)
        bitmap->gammatab[i] = 4096.0 * gammapow((float)i / 4096.0, 1.0 / gamma);
}

/**
 * \brief Invert colors of bitmap
 *
 * Invert colors of bitmap
 *
 * \param bitmap Bitmap object.
 * \param value 0 for normal behaviour, 1 for invert
 */
void cucul_set_bitmap_invert(struct cucul_bitmap *bitmap, int value)
{
    bitmap->invert = value ? 1 : 0;
}

/**
 * \brief Set the contrast of a bitmap object.
 *
 * Set the contrast of bitmap.
 *
 * \param bitmap Bitmap object.
 * \param contrast contrast value.
 */
void cucul_set_bitmap_contrast(struct cucul_bitmap *bitmap, float contrast)
{
    /* FIXME */
}

/**
 * \brief Set bitmap antialiasing
 *
 * Tell the renderer whether to antialias the bitmap. Antialiasing smoothen
 * the rendered image and avoids the commonly seen staircase effect.
 *
 * \li \e "none": no antialiasing.
 *
 * \li \e "prefilter": simple prefilter antialiasing. This is the default
 *     value.
 *
 * \param bitmap Bitmap object.
 * \param str A string describing the antialiasing method that will be used
 *        for the bitmap rendering.
 */
void cucul_set_bitmap_antialias(struct cucul_bitmap *bitmap, char const *str)
{
    if(!strcasecmp(str, "none"))
        bitmap->antialias = 0;
    else /* "prefilter" is the default */
        bitmap->antialias = 1;
}

/**
 * \brief Get available antialiasing methods
 *
 * Return a list of available antialiasing methods for a given bitmap. The
 * list is a NULL-terminated array of strings, interleaving a string
 * containing the internal value for the antialiasing method to be used with
 * \e cucul_set_bitmap_antialias(), and a string containing the natural
 * language description for that antialiasing method.
 *
 * \param bitmap Bitmap object.
 * \return An array of strings.
 */
char const * const *
    cucul_get_bitmap_antialias_list(struct cucul_bitmap const *bitmap)
{
    static char const * const list[] =
    {
        "none", "No antialiasing",
        "prefilter", "Prefilter antialiasing",
        NULL, NULL
    };

    return list;
}

/**
 * \brief Choose colours used for bitmap rendering
 *
 * Tell the renderer which colours should be used to render the
 * bitmap. Valid values for \e str are:
 *
 * \li \e "mono": use light gray on a black background.
 *
 * \li \e "gray": use white and two shades of gray on a black background.
 *
 * \li \e "8": use the 8 ANSI colours on a black background.
 *
 * \li \e "16": use the 16 ANSI colours on a black background.
 *
 * \li \e "fullgray": use black, white and two shades of gray for both the
 *     characters and the background.
 *
 * \li \e "full8": use the 8 ANSI colours for both the characters and the
 *     background.
 *
 * \li \e "full16": use the 16 ANSI colours for both the characters and the
 *     background. This is the default value.
 *
 * \param bitmap Bitmap object.
 * \param str A string describing the colour set that will be used
 *        for the bitmap rendering.
 */
void cucul_set_bitmap_color(struct cucul_bitmap *bitmap, char const *str)
{
    if(!strcasecmp(str, "mono"))
        bitmap->color_mode = COLOR_MODE_MONO;
    else if(!strcasecmp(str, "gray"))
        bitmap->color_mode = COLOR_MODE_GRAY;
    else if(!strcasecmp(str, "8"))
        bitmap->color_mode = COLOR_MODE_8;
    else if(!strcasecmp(str, "16"))
        bitmap->color_mode = COLOR_MODE_16;
    else if(!strcasecmp(str, "fullgray"))
        bitmap->color_mode = COLOR_MODE_FULLGRAY;
    else if(!strcasecmp(str, "full8"))
        bitmap->color_mode = COLOR_MODE_FULL8;
    else /* "full16" is the default */
        bitmap->color_mode = COLOR_MODE_FULL16;
}

/**
 * \brief Get available colour modes
 *
 * Return a list of available colour modes for a given bitmap. The list
 * is a NULL-terminated array of strings, interleaving a string containing
 * the internal value for the colour mode, to be used with
 * \e cucul_set_bitmap_color(), and a string containing the natural
 * language description for that colour mode.
 *
 * \param bitmap Bitmap object.
 * \return An array of strings.
 */
char const * const *
    cucul_get_bitmap_color_list(struct cucul_bitmap const *bitmap)
{
    static char const * const list[] =
    {
        "mono", "white on black",
        "gray", "grayscale on black",
        "8", "8 colours on black",
        "16", "16 colours on black",
        "fullgray", "full grayscale",
        "full8", "full 8 colours",
        "full16", "full 16 colours",
        NULL, NULL
    };

    return list;
}

/**
 * \brief Choose characters used for bitmap rendering
 *
 * Tell the renderer which characters should be used to render the
 * bitmap. Valid values for \e str are:
 *
 * \li \e "ascii": use only ASCII characters. This is the default value.
 *
 * \li \e "shades": use Unicode characters "U+2591 LIGHT SHADE", "U+2592
 *     MEDIUM SHADE" and "U+2593 DARK SHADE". These characters are also
 *     present in the CP437 codepage available on DOS and VGA.
 *
 * \li \e "blocks": use Unicode quarter-cell block combinations. These
 *     characters are only found in the Unicode set.
 *
 * \param bitmap Bitmap object.
 * \param str A string describing the characters that need to be used
 *        for the bitmap rendering.
 */
void cucul_set_bitmap_charset(struct cucul_bitmap *bitmap, char const *str)
{
    if(!strcasecmp(str, "shades"))
    {
        bitmap->glyphs = shades_glyphs;
        bitmap->glyph_count = sizeof(shades_glyphs) / sizeof(*shades_glyphs);
    }
    else if(!strcasecmp(str, "blocks"))
    {
        bitmap->glyphs = blocks_glyphs;
        bitmap->glyph_count = sizeof(blocks_glyphs) / sizeof(*blocks_glyphs);
    }
    else /* "ascii" is the default */
    {
        bitmap->glyphs = ascii_glyphs;
        bitmap->glyph_count = sizeof(ascii_glyphs) / sizeof(*ascii_glyphs);
    }
}

/**
 * \brief Get available bitmap character sets
 *
 * Return a list of available character sets for a given bitmap. The list
 * is a NULL-terminated array of strings, interleaving a string containing
 * the internal value for the character set, to be used with
 * \e cucul_set_bitmap_charset(), and a string containing the natural
 * language description for that character set.
 *
 * \param bitmap Bitmap object.
 * \return An array of strings.
 */
char const * const *
    cucul_get_bitmap_charset_list(struct cucul_bitmap const *bitmap)
{
    static char const * const list[] =
    {
        "ascii", "plain ASCII",
        "shades", "CP437 shades",
        "blocks", "Unicode blocks",
        NULL, NULL
    };

    return list;
}

/**
 * \brief Set bitmap dithering method
 *
 * Tell the renderer which dithering method should be used to render the
 * bitmap. Dithering is necessary because the picture being rendered has
 * usually far more colours than the available palette. Valid values for
 * \e str are:
 *
 * \li \e "none": no dithering is used, the nearest matching colour is used.
 *
 * \li \e "ordered2": use a 2x2 Bayer matrix for dithering.
 *
 * \li \e "ordered4": use a 4x4 Bayer matrix for dithering.
 *
 * \li \e "ordered8": use a 8x8 Bayer matrix for dithering.
 *
 * \li \e "random": use random dithering.
 *
 * \li \e "fstein": use Floyd-Steinberg dithering. This is the default value.
 *
 * \param bitmap Bitmap object.
 * \param str A string describing the dithering method that needs to be used
 *        for the bitmap rendering.
 */
void cucul_set_bitmap_dithering(struct cucul_bitmap *bitmap, char const *str)
{
    if(!strcasecmp(str, "none"))
    {
        bitmap->init_dither = init_no_dither;
        bitmap->get_dither = get_no_dither;
        bitmap->increment_dither = increment_no_dither;
    }
    else if(!strcasecmp(str, "ordered2"))
    {
        bitmap->init_dither = init_ordered2_dither;
        bitmap->get_dither = get_ordered2_dither;
        bitmap->increment_dither = increment_ordered2_dither;
    }
    else if(!strcasecmp(str, "ordered4"))
    {
        bitmap->init_dither = init_ordered4_dither;
        bitmap->get_dither = get_ordered4_dither;
        bitmap->increment_dither = increment_ordered4_dither;
    }
    else if(!strcasecmp(str, "ordered4"))
    {
        bitmap->init_dither = init_ordered8_dither;
        bitmap->get_dither = get_ordered8_dither;
        bitmap->increment_dither = increment_ordered8_dither;
    }
    else if(!strcasecmp(str, "random"))
    {
        bitmap->init_dither = init_random_dither;
        bitmap->get_dither = get_random_dither;
        bitmap->increment_dither = increment_random_dither;
    }
    else /* "fstein" is the default */
    {
        bitmap->init_dither = init_fstein_dither;
        bitmap->get_dither = get_fstein_dither;
        bitmap->increment_dither = increment_fstein_dither;
    }
}

/**
 * \brief Get bitmap dithering methods
 *
 * Return a list of available dithering methods for a given bitmap. The list
 * is a NULL-terminated array of strings, interleaving a string containing
 * the internal value for the dithering method, to be used with
 * \e cucul_set_bitmap_dithering(), and a string containing the natural
 * language description for that dithering method.
 *
 * \param bitmap Bitmap object.
 * \return An array of strings.
 */
char const * const *
    cucul_get_bitmap_dithering_list(struct cucul_bitmap const *bitmap)
{
    static char const * const list[] =
    {
        "none", "no dithering",
        "ordered2", "2x2 ordered dithering",
        "ordered2", "2x2 ordered dithering",
        "ordered2", "2x2 ordered dithering",
        "random", "random dithering",
        "fstein", "Floyd-Steinberg dithering",
        NULL, NULL
    };

    return list;
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
void cucul_draw_bitmap(cucul_t *qq, int x1, int y1, int x2, int y2,
                       struct cucul_bitmap const *bitmap, void *pixels)
{
    int *floyd_steinberg, *fs_r, *fs_g, *fs_b;
    int fs_length;
    int x, y, w, h, pitch, deltax, deltay;
    unsigned int dchmax;

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
    dchmax = bitmap->glyph_count;

    fs_length = ((int)qq->width <= x2 ? (int)qq->width : x2) + 1;
    floyd_steinberg = malloc(3 * (fs_length + 2) * sizeof(int));
    memset(floyd_steinberg, 0, 3 * (fs_length + 2) * sizeof(int));
    fs_r = floyd_steinberg + 1;
    fs_g = fs_r + fs_length + 2;
    fs_b = fs_g + fs_length + 2;

    for(y = y1 > 0 ? y1 : 0; y <= y2 && y <= (int)qq->height; y++)
    {
        int remain_r = 0, remain_g = 0, remain_b = 0;

        for(x = x1 > 0 ? x1 : 0, bitmap->init_dither(y);
            x <= x2 && x <= (int)qq->width;
            x++)
    {
        unsigned int i;
        int ch = 0, distmin;
        unsigned int rgba[4];
        int fg_r = 0, fg_g = 0, fg_b = 0, bg_r, bg_g, bg_b;
        int fromx, fromy, tox, toy, myx, myy, dots, dist;
        int error[3];

        unsigned int outfg = 0, outbg = 0;
        char const *outch;

        rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;

        /* First get RGB */
        if(bitmap->antialias)
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
                get_rgba_default(bitmap, pixels, myx, myy, rgba);
            }

            /* Normalize */
            rgba[0] /= dots;
            rgba[1] /= dots;
            rgba[2] /= dots;
            rgba[3] /= dots;
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

            get_rgba_default(bitmap, pixels, myx, myy, rgba);
        }

        if(bitmap->has_alpha && rgba[3] < 0x800)
        {
            remain_r = remain_g = remain_b = 0;
            fs_r[x] = 0;
            fs_g[x] = 0;
            fs_b[x] = 0;
            continue;
        }

        /* XXX: OMG HAX */
        if(bitmap->init_dither == init_fstein_dither)
        {
            rgba[0] += remain_r;
            rgba[1] += remain_g;
            rgba[2] += remain_b;
        }
        else
        {
            rgba[0] += (bitmap->get_dither() - 0x80) * 4;
            rgba[1] += (bitmap->get_dither() - 0x80) * 4;
            rgba[2] += (bitmap->get_dither() - 0x80) * 4;
        }

        distmin = INT_MAX;
        for(i = 0; i < 16; i++)
        {
            dist = sq(rgba[0] - rgb_palette[i * 3])
                 + sq(rgba[1] - rgb_palette[i * 3 + 1])
                 + sq(rgba[2] - rgb_palette[i * 3 + 2]);
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

        /* FIXME: we currently only honour "full16" */
        if(bitmap->color_mode == COLOR_MODE_FULL16)
        {
            distmin = INT_MAX;
            for(i = 0; i < 16; i++)
            {
                if(i == outbg)
                    continue;
                dist = sq(rgba[0] - rgb_palette[i * 3])
                     + sq(rgba[1] - rgb_palette[i * 3 + 1])
                     + sq(rgba[2] - rgb_palette[i * 3 + 2]);
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
            for(i = 0; i < dchmax - 1; i++)
            {
                int newr = i * fg_r + ((2*dchmax-1) - i) * bg_r;
                int newg = i * fg_g + ((2*dchmax-1) - i) * bg_g;
                int newb = i * fg_b + ((2*dchmax-1) - i) * bg_b;
                dist = abs(rgba[0] * (2*dchmax-1) - newr)
                     + abs(rgba[1] * (2*dchmax-1) - newg)
                     + abs(rgba[2] * (2*dchmax-1) - newb);

                if(dist < distmin)
                {
                    ch = i;
                    distmin = dist;
                }
            }
            outch = bitmap->glyphs[ch];

            /* XXX: OMG HAX */
            if(bitmap->init_dither == init_fstein_dither)
            {
                error[0] = rgba[0] - (fg_r * ch + bg_r * ((2*dchmax-1) - ch)) / (2*dchmax-1);
                error[1] = rgba[1] - (fg_g * ch + bg_g * ((2*dchmax-1) - ch)) / (2*dchmax-1);
                error[2] = rgba[2] - (fg_b * ch + bg_b * ((2*dchmax-1) - ch)) / (2*dchmax-1);
            }
        }
        else
        {
            unsigned int lum = rgba[0];
            if(rgba[1] > lum) lum = rgba[1];
            if(rgba[2] > lum) lum = rgba[2];
            outfg = outbg;
            outbg = CUCUL_COLOR_BLACK;

            ch = lum * dchmax / 0x1000;
            if(ch < 0)
                ch = 0;
            else if(ch > (int)(dchmax - 1))
                ch = dchmax - 1;
            outch = bitmap->glyphs[ch];

            /* XXX: OMG HAX */
            if(bitmap->init_dither == init_fstein_dither)
            {
                error[0] = rgba[0] - bg_r * ch / (dchmax-1);
                error[1] = rgba[1] - bg_g * ch / (dchmax-1);
                error[2] = rgba[2] - bg_b * ch / (dchmax-1);
            }
        }

        /* XXX: OMG HAX */
        if(bitmap->init_dither == init_fstein_dither)
        {
            remain_r = fs_r[x+1] + 7 * error[0] / 16;
            remain_g = fs_g[x+1] + 7 * error[1] / 16;
            remain_b = fs_b[x+1] + 7 * error[2] / 16;
            fs_r[x-1] += 3 * error[0] / 16;
            fs_g[x-1] += 3 * error[1] / 16;
            fs_b[x-1] += 3 * error[2] / 16;
            fs_r[x] = 5 * error[0] / 16;
            fs_g[x] = 5 * error[1] / 16;
            fs_b[x] = 5 * error[2] / 16;
            fs_r[x+1] = 1 * error[0] / 16;
            fs_g[x+1] = 1 * error[1] / 16;
            fs_b[x+1] = 1 * error[2] / 16;
        }

        if(bitmap->invert)
        {
            outfg = 15 - outfg;
            outbg = 15 - outbg;
        }

        /* Now output the character */
        cucul_set_color(qq, outfg, outbg);
        cucul_putstr(qq, x, y, outch);

       bitmap->increment_dither();
    }
        /* end loop */
    }

    free(floyd_steinberg);
}

/**
 * \brief Free the memory associated with a bitmap.
 *
 * Free the memory allocated by cucul_create_bitmap().
 *
 * \param bitmap Bitmap object.
 */
void cucul_free_bitmap(struct cucul_bitmap *bitmap)
{
    if(!bitmap)
        return;

    free(bitmap);
}

/*
 * XXX: The following functions are local.
 */

/* Convert a mask, eg. 0x0000ff00, to shift values, eg. 8 and -4. */
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

/* Compute x^y without relying on the math library */
static float gammapow(float x, float y)
{
#ifdef HAVE_FLDLN2
    register double logx;
    register long double v, e;
#else
    register float tmp, t, t2, r;
    int i;
#endif

    if(x == 0.0)
        return y == 0.0 ? 1.0 : 0.0;

#ifdef HAVE_FLDLN2
    /* FIXME: this can be optimised by directly calling fyl2x for x and y */
    asm volatile("fldln2; fxch; fyl2x"
                 : "=t" (logx) : "0" (x) : "st(1)");

    asm volatile("fldl2e\n\t"
                 "fmul %%st(1)\n\t"
                 "fst %%st(1)\n\t"
                 "frndint\n\t"
                 "fxch\n\t"
                 "fsub %%st(1)\n\t"
                 "f2xm1\n\t"
                 : "=t" (v), "=u" (e) : "0" (y * logx));
    v += 1.0;
    asm volatile("fscale"
                 : "=t" (v) : "0" (v), "u" (e));
    return v;
#else
    /* Compute ln(x) for x ∈ ]0,1]
     *   ln(x) = 2 * (t + t^3/3 + t^5/5 + ...) with t = (x-1)/(x+1)
     * The convergence is a bit slow, especially when x is near 0. */
    t = (x - 1.0) / (x + 1.0);
    t2 = t * t;
    tmp = r = t;
    for(i = 3; i < 20; i += 2)
    {
        r *= t2;
        tmp += r / i;
    }

    /* Compute -y*ln(x) */
    tmp = - y * 2.0 * tmp;

    /* Compute x^-y as e^t where t = -y*ln(x):
     *   e^t = 1 + t/1! + t^2/2! + t^3/3! + t^4/4! + t^5/5! ...
     * The convergence is quite faster here, thanks to the factorial. */
    r = t = tmp;
    tmp = 1.0 + t;
    for(i = 2; i < 16; i++)
    {
        r = r * t / i;
        tmp += r;
    }

    /* Return x^y as 1/(x^-y) */
    return 1.0 / tmp;
#endif
}

static void get_rgba_default(struct cucul_bitmap const *bitmap, uint8_t *pixels,
                             int x, int y, unsigned int *rgba)
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
        rgba[0] += bitmap->gammatab[bitmap->red[bits]];
        rgba[1] += bitmap->gammatab[bitmap->green[bits]];
        rgba[2] += bitmap->gammatab[bitmap->blue[bits]];
        rgba[3] += bitmap->alpha[bits];
    }
    else
    {
        rgba[0] += bitmap->gammatab[((bits & bitmap->rmask) >> bitmap->rright) << bitmap->rleft];
        rgba[1] += bitmap->gammatab[((bits & bitmap->gmask) >> bitmap->gright) << bitmap->gleft];
        rgba[2] += bitmap->gammatab[((bits & bitmap->bmask) >> bitmap->bright) << bitmap->bleft];
        rgba[3] += ((bits & bitmap->amask) >> bitmap->aright) << bitmap->aleft;
    }
}

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
 * Floyd-Steinberg dithering
 */
static void init_fstein_dither(int line)
{
    ;
}

static unsigned int get_fstein_dither(void)
{
    return 0x80;
}

static void increment_fstein_dither(void)
{
    return;
}

/*
 * Ordered 2 dithering
 */
static unsigned int const *ordered2_table;
static unsigned int ordered2_index;

static void init_ordered2_dither(int line)
{
    static unsigned int const dither2x2[] =
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
static unsigned int const *ordered4_table;
static unsigned int ordered4_index;

static void init_ordered4_dither(int line)
{
    static unsigned int const dither4x4[] =
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
static unsigned int const *ordered8_table;
static unsigned int ordered8_index;

static void init_ordered8_dither(int line)
{
    static unsigned int const dither8x8[] =
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
    return cucul_rand(0x00, 0xff);
}

static void increment_random_dither(void)
{
    return;
}

#if !defined(_DOXYGEN_SKIP_ME)
int _cucul_init_bitmap(void)
{
    unsigned int v, s, h;

    /* These ones are constant */
    lookup_colors[0] = CUCUL_COLOR_BLACK;
    lookup_colors[1] = CUCUL_COLOR_DARKGRAY;
    lookup_colors[2] = CUCUL_COLOR_LIGHTGRAY;
    lookup_colors[3] = CUCUL_COLOR_WHITE;

    /* These ones will be overwritten */
    lookup_colors[4] = CUCUL_COLOR_MAGENTA;
    lookup_colors[5] = CUCUL_COLOR_LIGHTMAGENTA;
    lookup_colors[6] = CUCUL_COLOR_RED;
    lookup_colors[7] = CUCUL_COLOR_LIGHTRED;

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

int _cucul_end_bitmap(void)
{
    return 0;
}
#endif /* _DOXYGEN_SKIP_ME */


