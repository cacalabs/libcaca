/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains bitmap dithering functions.
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

#include "caca.h"
#include "caca_internals.h"

#define CP437 0

/*
 * Local variables
 */
#if !defined(_DOXYGEN_SKIP_ME)
#   define LOOKUP_VAL 32
#   define LOOKUP_SAT 32
#   define LOOKUP_HUE 16
#endif
static uint8_t hsv_distances[LOOKUP_VAL][LOOKUP_SAT][LOOKUP_HUE];
static uint16_t lookup_colors[8];
static int lookup_initialised = 0;

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
    /* 2, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2 */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* List of glyphs */
static uint32_t ascii_glyphs[] =
{
    ' ', '.', ':', ';', 't', '%', 'S', 'X', '@', '8', '?'
};

static uint32_t shades_glyphs[] =
{
    /* ' '. '·', '░', '▒', '?' */
    ' ', 0xb7, 0x2591, 0x2592, '?'
};

static uint32_t blocks_glyphs[] =
{
    /* ' ', '▘', '▚', '?' */
    ' ', 0x2598, 0x259a, '?'
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
    COLOR_MODE_FULL16
};

struct caca_dither
{
    int bpp, has_palette, has_alpha;
    int w, h, pitch;
    int rmask, gmask, bmask, amask;
    int rright, gright, bright, aright;
    int rleft, gleft, bleft, aleft;
    void (*get_hsv)(caca_dither_t *, char *, int, int);
    int red[256], green[256], blue[256], alpha[256];

    /* Colour features */
    float gamma, brightness, contrast;
    int gammatab[4097];

    /* Dithering features */
    char const *antialias_name;
    int antialias;

    char const *color_name;
    enum color_mode color;

    char const *algo_name;
    void (*init_dither) (int);
    int (*get_dither) (void);
    void (*increment_dither) (void);

    char const *glyph_name;
    uint32_t const * glyphs;
    int glyph_count;

    int invert;
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
static void mask2shift(uint32_t, int *, int *);
static float gammapow(float x, float y);

static void get_rgba_default(caca_dither_t const *, uint8_t *, int, int,
                             unsigned int *);
static int init_lookup(void);

/* Dithering algorithms */
static void init_no_dither(int);
static int get_no_dither(void);
static void increment_no_dither(void);

static void init_fstein_dither(int);
static int get_fstein_dither(void);
static void increment_fstein_dither(void);

static void init_ordered2_dither(int);
static int get_ordered2_dither(void);
static void increment_ordered2_dither(void);

static void init_ordered4_dither(int);
static int get_ordered4_dither(void);
static void increment_ordered4_dither(void);

static void init_ordered8_dither(int);
static int get_ordered8_dither(void);
static void increment_ordered8_dither(void);

static void init_random_dither(int);
static int get_random_dither(void);
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

/** \brief Create an internal dither object.
 *
 *  Create a dither structure from its coordinates (depth, width, height and
 *  pitch) and pixel mask values. If the depth is 8 bits per pixel, the mask
 *  values are ignored and the colour palette should be set using the
 *  caca_set_dither_palette() function. For depths greater than 8 bits per
 *  pixel, a zero alpha mask causes the alpha values to be ignored.
 *
 *  If an error occurs, NULL is returned and \b errno is set accordingly:
 *  - \c EINVAL Requested width, height, pitch or bits per pixel value was
 *    invalid.
 *  - \c ENOMEM Not enough memory to allocate dither structure.
 *
 *  \param bpp Bitmap depth in bits per pixel.
 *  \param w Bitmap width in pixels.
 *  \param h Bitmap height in pixels.
 *  \param pitch Bitmap pitch in bytes.
 *  \param rmask Bitmask for red values.
 *  \param gmask Bitmask for green values.
 *  \param bmask Bitmask for blue values.
 *  \param amask Bitmask for alpha values.
 *  \return Dither object upon success, NULL if an error occurred.
 */
caca_dither_t *caca_create_dither(int bpp, int w, int h, int pitch,
                                    uint32_t rmask, uint32_t gmask,
                                    uint32_t bmask, uint32_t amask)
{
    caca_dither_t *d;
    int i;

    /* Minor sanity test */
    if(w < 0 || h < 0 || pitch < 0 || bpp > 32 || bpp < 8)
    {
        seterrno(EINVAL);
        return NULL;
    }

    d = malloc(sizeof(caca_dither_t));
    if(!d)
    {
        seterrno(ENOMEM);
        return NULL;
    }

    if(!lookup_initialised)
    {
        /* XXX: because we do not wish to be thread-safe, there is a slight
         * chance that the following code will be executed twice. It is
         * totally harmless. */
        init_lookup();
        lookup_initialised = 1;
    }

    d->bpp = bpp;
    d->has_palette = 0;
    d->has_alpha = amask ? 1 : 0;

    d->w = w;
    d->h = h;
    d->pitch = pitch;

    d->rmask = rmask;
    d->gmask = gmask;
    d->bmask = bmask;
    d->amask = amask;

    /* Load bitmasks */
    if(rmask || gmask || bmask || amask)
    {
        mask2shift(rmask, &d->rright, &d->rleft);
        mask2shift(gmask, &d->gright, &d->gleft);
        mask2shift(bmask, &d->bright, &d->bleft);
        mask2shift(amask, &d->aright, &d->aleft);
    }

    /* In 8 bpp mode, default to a grayscale palette */
    if(bpp == 8)
    {
        d->has_palette = 1;
        d->has_alpha = 0;
        for(i = 0; i < 256; i++)
        {
            d->red[i] = i * 0xfff / 256;
            d->green[i] = i * 0xfff / 256;
            d->blue[i] = i * 0xfff / 256;
        }
    }

    /* Default gamma value */
    d->gamma = 1.0;
    for(i = 0; i < 4096; i++)
        d->gammatab[i] = i;

    /* Default colour properties */
    d->brightness = 1.0;
    d->contrast = 1.0;

    /* Default features */
    d->antialias_name = "prefilter";
    d->antialias = 1;

    d->color_name = "full16";
    d->color = COLOR_MODE_FULL16;

    d->glyph_name = "ascii";
    d->glyphs = ascii_glyphs;
    d->glyph_count = sizeof(ascii_glyphs) / sizeof(*ascii_glyphs);

    d->algo_name = "fstein";
    d->init_dither = init_fstein_dither;
    d->get_dither = get_fstein_dither;
    d->increment_dither = increment_fstein_dither;

    d->invert = 0;

    return d;
}

/** \brief Set the palette of an 8bpp dither object.
 *
 *  Set the palette of an 8 bits per pixel bitmap. Values should be between
 *  0 and 4095 (0xfff).
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Dither bits per pixel value is not 8, or one of the pixel
 *   values was outside the range 0 - 4095.
 *
 *  \param d Dither object.
 *  \param red Array of 256 red values.
 *  \param green Array of 256 green values.
 *  \param blue Array of 256 blue values.
 *  \param alpha Array of 256 alpha values.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_palette(caca_dither_t *d,
                             uint32_t red[], uint32_t green[],
                             uint32_t blue[], uint32_t alpha[])
{
    int i, has_alpha = 0;

    if(d->bpp != 8)
    {
        seterrno(EINVAL);
        return -1;
    }

    for(i = 0; i < 256; i++)
    {
        if((red[i] | green[i] | blue[i] | alpha[i]) >= 0x1000)
        {
            seterrno(EINVAL);
            return -1;
        }
    }

    for(i = 0; i < 256; i++)
    {
        d->red[i] = red[i];
        d->green[i] = green[i];
        d->blue[i] = blue[i];
        if(alpha[i])
        {
            d->alpha[i] = alpha[i];
            has_alpha = 1;
        }
    }

    d->has_alpha = has_alpha;

    return 0;
}

/** \brief Set the brightness of a dither object.
 *
 *  Set the brightness of dither.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Brightness value was out of range.
 *
 *  \param d Dither object.
 *  \param brightness brightness value.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_brightness(caca_dither_t *d, float brightness)
{
    /* FIXME */
    d->brightness = brightness;

    return 0;
}

/** \brief Get the brightness of a dither object.
 *
 *  Get the brightness of the given dither object.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return Brightness value.
 */
float caca_get_dither_brightness(caca_dither_t const *d)
{
    return d->brightness;
}

/** \brief Set the gamma of a dither object.
 *
 *  Set the gamma of the given dither object. A negative value causes
 *  colour inversion.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Gamma value was out of range.
 *
 *  \param d Dither object.
 *  \param gamma Gamma value.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_gamma(caca_dither_t *d, float gamma)
{
    /* FIXME: we don't need 4096 calls to gammapow(), we could just compute
     * a few of them and do linear interpolation for the rest. This will
     * probably speed up things a lot. */
    int i;

    if(gamma < 0.0)
    {
        d->invert = 1;
        gamma = -gamma;
    }
    else if(gamma == 0.0)
    {
        seterrno(EINVAL);
        return -1;
    }

    d->gamma = gamma;

    for(i = 0; i < 4096; i++)
        d->gammatab[i] = 4096.0 * gammapow((float)i / 4096.0, 1.0 / gamma);

    return 0;
}

/** \brief Get the gamma of a dither object.
 *
 *  Get the gamma of the given dither object.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return Gamma value.
 */
float caca_get_dither_gamma(caca_dither_t const *d)
{
    return d->gamma;
}

/** \brief Set the contrast of a dither object.
 *
 *  Set the contrast of dither.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Contrast value was out of range.
 *
 *  \param d Dither object.
 *  \param contrast contrast value.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_contrast(caca_dither_t *d, float contrast)
{
    /* FIXME */
    d->contrast = contrast;

    return 0;
}

/** \brief Get the contrast of a dither object.
 *
 *  Get the contrast of the given dither object.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return Contrast value.
 */
float caca_get_dither_contrast(caca_dither_t const *d)
{
    return d->contrast;
}

/** \brief Set dither antialiasing
 *
 *  Tell the renderer whether to antialias the dither. Antialiasing smoothens
 *  the rendered image and avoids the commonly seen staircase effect.
 *  - \c "none": no antialiasing.
 *  - \c "prefilter" or \c "default": simple prefilter antialiasing. This
 *    is the default value.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Invalid antialiasing mode.
 *
 *  \param d Dither object.
 *  \param str A string describing the antialiasing method that will be used
 *         for the dithering.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_antialias(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "none"))
    {
        d->antialias_name = "none";
        d->antialias = 0;
    }
    else if(!strcasecmp(str, "prefilter") || !strcasecmp(str, "default"))
    {
        d->antialias_name = "prefilter";
        d->antialias = 1;
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}

/** \brief Get available antialiasing methods
 *
 *  Return a list of available antialiasing methods for a given dither. The
 *  list is a NULL-terminated array of strings, interleaving a string
 *  containing the internal value for the antialiasing method to be used with
 *  caca_set_dither_antialias(), and a string containing the natural
 *  language description for that antialiasing method.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return An array of strings.
 */
char const * const *
    caca_get_dither_antialias_list(caca_dither_t const *d)
{
    static char const * const list[] =
    {
        "none", "No antialiasing",
        "prefilter", "Prefilter antialiasing",
        NULL, NULL
    };

    return list;
}

/** \brief Get current antialiasing method
 *
 *  Return the given dither's current antialiasing method.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return A static string.
 */
char const * caca_get_dither_antialias(caca_dither_t const *d)
{
    return d->antialias_name;
}

/** \brief Choose colours used for dithering
 *
 *  Tell the renderer which colours should be used to render the
 *  bitmap. Valid values for \c str are:
 *  - \c "mono": use light gray on a black background.
 *  - \c "gray": use white and two shades of gray on a black background.
 *  - \c "8": use the 8 ANSI colours on a black background.
 *  - \c "16": use the 16 ANSI colours on a black background.
 *  - \c "fullgray": use black, white and two shades of gray for both the
 *    characters and the background.
 *  - \c "full8": use the 8 ANSI colours for both the characters and the
 *    background.
 *  - \c "full16" or \c "default": use the 16 ANSI colours for both the
 *    characters and the background. This is the default value.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Invalid colour set.
 *
 *  \param d Dither object.
 *  \param str A string describing the colour set that will be used
 *         for the dithering.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_color(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "mono"))
    {
        d->color_name = "mono";
        d->color = COLOR_MODE_MONO;
    }
    else if(!strcasecmp(str, "gray"))
    {
        d->color_name = "gray";
        d->color = COLOR_MODE_GRAY;
    }
    else if(!strcasecmp(str, "8"))
    {
        d->color_name = "8";
        d->color = COLOR_MODE_8;
    }
    else if(!strcasecmp(str, "16"))
    {
        d->color_name = "16";
        d->color = COLOR_MODE_16;
    }
    else if(!strcasecmp(str, "fullgray"))
    {
        d->color_name = "fullgray";
        d->color = COLOR_MODE_FULLGRAY;
    }
    else if(!strcasecmp(str, "full8"))
    {
        d->color_name = "full8";
        d->color = COLOR_MODE_FULL8;
    }
    else if(!strcasecmp(str, "full16") || !strcasecmp(str, "default"))
    {
        d->color_name = "full16";
        d->color = COLOR_MODE_FULL16;
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}

/** \brief Get available colour modes
 *
 *  Return a list of available colour modes for a given dither. The list
 *  is a NULL-terminated array of strings, interleaving a string containing
 *  the internal value for the colour mode, to be used with
 *  caca_set_dither_color(), and a string containing the natural
 *  language description for that colour mode.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return An array of strings.
 */
char const * const *
    caca_get_dither_color_list(caca_dither_t const *d)
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

/** \brief Get current colour mode
 *
 *  Return the given dither's current colour mode.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return A static string.
 */
char const * caca_get_dither_color(caca_dither_t const *d)
{
    return d->color_name;
}

/** \brief Choose characters used for dithering
 *
 *  Tell the renderer which characters should be used to render the
 *  dither. Valid values for \c str are:
 *  - \c "ascii" or \c "default": use only ASCII characters. This is the
 *    default value.
 *  - \c "shades": use Unicode characters "U+2591 LIGHT SHADE", "U+2592
 *    MEDIUM SHADE" and "U+2593 DARK SHADE". These characters are also
 *    present in the CP437 codepage available on DOS and VGA.
 *  - \c "blocks": use Unicode quarter-cell block combinations. These
 *    characters are only found in the Unicode set.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Invalid character set.
 *
 *  \param d Dither object.
 *  \param str A string describing the characters that need to be used
 *         for the dithering.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_charset(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "shades"))
    {
        d->glyph_name = "shades";
        d->glyphs = shades_glyphs;
        d->glyph_count = sizeof(shades_glyphs) / sizeof(*shades_glyphs);
    }
    else if(!strcasecmp(str, "blocks"))
    {
        d->glyph_name = "blocks";
        d->glyphs = blocks_glyphs;
        d->glyph_count = sizeof(blocks_glyphs) / sizeof(*blocks_glyphs);
    }
    else if(!strcasecmp(str, "ascii") || !strcasecmp(str, "default"))
    {
        d->glyph_name = "ascii";
        d->glyphs = ascii_glyphs;
        d->glyph_count = sizeof(ascii_glyphs) / sizeof(*ascii_glyphs);
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}

/** \brief Get available dither character sets
 *
 *  Return a list of available character sets for a given dither. The list
 *  is a NULL-terminated array of strings, interleaving a string containing
 *  the internal value for the character set, to be used with
 *  caca_set_dither_charset(), and a string containing the natural
 *  language description for that character set.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return An array of strings.
 */
char const * const * caca_get_dither_charset_list(caca_dither_t const *d)
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

/** \brief Get current character set
 *
 *  Return the given dither's current character set.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return A static string.
 */
char const * caca_get_dither_charset(caca_dither_t const *d)
{
    return d->glyph_name;
}

/** \brief Set dithering algorithm
 *
 *  Tell the renderer which dithering algorithm should be used. Dithering is
 *  necessary because the picture being rendered has usually far more colours
 *  than the available palette. Valid values for \c str are:
 *  - \c "none": no dithering is used, the nearest matching colour is used.
 *  - \c "ordered2": use a 2x2 Bayer matrix for dithering.
 *  - \c "ordered4": use a 4x4 Bayer matrix for dithering.
 *  - \c "ordered8": use a 8x8 Bayer matrix for dithering.
 *  - \c "random": use random dithering.
 *  - \c "fstein": use Floyd-Steinberg dithering. This is the default value.
 *
 *  If an error occurs, -1 is returned and \b errno is set accordingly:
 *  - \c EINVAL Unknown dithering mode.
 *
 *  \param d Dither object.
 *  \param str A string describing the algorithm that needs to be used
 *         for the dithering.
 *  \return 0 in case of success, -1 if an error occurred.
 */
int caca_set_dither_algorithm(caca_dither_t *d, char const *str)
{
    if(!strcasecmp(str, "none"))
    {
        d->algo_name = "none";
        d->init_dither = init_no_dither;
        d->get_dither = get_no_dither;
        d->increment_dither = increment_no_dither;
    }
    else if(!strcasecmp(str, "ordered2"))
    {
        d->algo_name = "ordered2";
        d->init_dither = init_ordered2_dither;
        d->get_dither = get_ordered2_dither;
        d->increment_dither = increment_ordered2_dither;
    }
    else if(!strcasecmp(str, "ordered4"))
    {
        d->algo_name = "ordered4";
        d->init_dither = init_ordered4_dither;
        d->get_dither = get_ordered4_dither;
        d->increment_dither = increment_ordered4_dither;
    }
    else if(!strcasecmp(str, "ordered8"))
    {
        d->algo_name = "ordered8";
        d->init_dither = init_ordered8_dither;
        d->get_dither = get_ordered8_dither;
        d->increment_dither = increment_ordered8_dither;
    }
    else if(!strcasecmp(str, "random"))
    {
        d->algo_name = "random";
        d->init_dither = init_random_dither;
        d->get_dither = get_random_dither;
        d->increment_dither = increment_random_dither;
    }
    else if(!strcasecmp(str, "fstein") || !strcasecmp(str, "default"))
    {
        d->algo_name = "fstein";
        d->init_dither = init_fstein_dither;
        d->get_dither = get_fstein_dither;
        d->increment_dither = increment_fstein_dither;
    }
    else
    {
        seterrno(EINVAL);
        return -1;
    }

    return 0;
}

/** \brief Get dithering algorithms
 *
 *  Return a list of available dithering algorithms for a given dither. The
 *  list is a NULL-terminated array of strings, interleaving a string
 *  containing the internal value for the dithering algorithm, to be used
 *  with caca_set_dither_dithering(), and a string containing the natural
 *  language description for that algorithm.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return An array of strings.
 */
char const * const * caca_get_dither_algorithm_list(caca_dither_t const *d)
{
    static char const * const list[] =
    {
        "none", "no dithering",
        "ordered2", "2x2 ordered dithering",
        "ordered4", "4x4 ordered dithering",
        "ordered8", "8x8 ordered dithering",
        "random", "random dithering",
        "fstein", "Floyd-Steinberg dithering",
        NULL, NULL
    };

    return list;
}

/** \brief Get current dithering algorithm
 *
 *  Return the given dither's current dithering algorithm.
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return A static string.
 */
char const * caca_get_dither_algorithm(caca_dither_t const *d)
{
    return d->algo_name;
}

/** \brief Dither a bitmap on the canvas.
 *
 *  Dither a bitmap at the given coordinates. The dither can be of any size
 *  and will be stretched to the text area.
 *
 *  This function never fails.
 *
 *  \param cv A handle to the libcaca canvas.
 *  \param x X coordinate of the upper-left corner of the drawing area.
 *  \param y Y coordinate of the upper-left corner of the drawing area.
 *  \param w Width of the drawing area.
 *  \param h Height of the drawing area.
 *  \param d Dither object to be drawn.
 *  \param pixels Bitmap's pixels.
 *  \return This function always returns 0.
 */
int caca_dither_bitmap(caca_canvas_t *cv, int x, int y, int w, int h,
                        caca_dither_t const *d, void *pixels)
{
    int *floyd_steinberg, *fs_r, *fs_g, *fs_b;
    uint32_t savedattr;
    int fs_length;
    int x1, y1, x2, y2, pitch, deltax, deltay, dchmax;

    if(!d || !pixels)
        return 0;

    savedattr = caca_get_attr(cv, -1, -1);

    x1 = x; x2 = x + w - 1;
    y1 = y; y2 = y + h - 1;

    /* FIXME: do not overwrite arguments */
    w = d->w;
    h = d->h;
    pitch = d->pitch;

    deltax = x2 - x1 + 1;
    deltay = y2 - y1 + 1;
    dchmax = d->glyph_count;

    fs_length = ((int)cv->width <= x2 ? (int)cv->width : x2) + 1;
    floyd_steinberg = malloc(3 * (fs_length + 2) * sizeof(int));
    memset(floyd_steinberg, 0, 3 * (fs_length + 2) * sizeof(int));
    fs_r = floyd_steinberg + 1;
    fs_g = fs_r + fs_length + 2;
    fs_b = fs_g + fs_length + 2;

    for(y = y1 > 0 ? y1 : 0; y <= y2 && y <= (int)cv->height; y++)
    {
        int remain_r = 0, remain_g = 0, remain_b = 0;

        for(x = x1 > 0 ? x1 : 0, d->init_dither(y);
            x <= x2 && x <= (int)cv->width;
            x++)
    {
        unsigned int rgba[4];
        int error[3];
        int i, ch = 0, distmin;
        int fg_r = 0, fg_g = 0, fg_b = 0, bg_r, bg_g, bg_b;
        int fromx, fromy, tox, toy, myx, myy, dots, dist;

        int outfg = 0, outbg = 0;
        uint32_t outch;

        rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;

        /* First get RGB */
        if(d->antialias)
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
                get_rgba_default(d, pixels, myx, myy, rgba);
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

            /* tox and toy can overflow the canvas, but they cannot overflow
             * when averaged with fromx and fromy because these are guaranteed
             * to be within the pixel boundaries. */
            myx = (fromx + tox) / 2;
            myy = (fromy + toy) / 2;

            get_rgba_default(d, pixels, myx, myy, rgba);
        }

        if(d->has_alpha && rgba[3] < 0x800)
        {
            remain_r = remain_g = remain_b = 0;
            fs_r[x] = 0;
            fs_g[x] = 0;
            fs_b[x] = 0;
            continue;
        }

        /* XXX: OMG HAX */
        if(d->init_dither == init_fstein_dither)
        {
            rgba[0] += remain_r;
            rgba[1] += remain_g;
            rgba[2] += remain_b;
        }
        else
        {
            rgba[0] += (d->get_dither() - 0x80) * 4;
            rgba[1] += (d->get_dither() - 0x80) * 4;
            rgba[2] += (d->get_dither() - 0x80) * 4;
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
        if(d->color == COLOR_MODE_FULL16)
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
            outch = d->glyphs[ch];

            /* XXX: OMG HAX */
            if(d->init_dither == init_fstein_dither)
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
            outbg = CACA_BLACK;

            ch = lum * dchmax / 0x1000;
            if(ch < 0)
                ch = 0;
            else if(ch > (int)(dchmax - 1))
                ch = dchmax - 1;
            outch = d->glyphs[ch];

            /* XXX: OMG HAX */
            if(d->init_dither == init_fstein_dither)
            {
                error[0] = rgba[0] - bg_r * ch / (dchmax-1);
                error[1] = rgba[1] - bg_g * ch / (dchmax-1);
                error[2] = rgba[2] - bg_b * ch / (dchmax-1);
            }
        }

        /* XXX: OMG HAX */
        if(d->init_dither == init_fstein_dither)
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

        if(d->invert)
        {
            outfg = 15 - outfg;
            outbg = 15 - outbg;
        }

        /* Now output the character */
        caca_set_color_ansi(cv, outfg, outbg);
        caca_put_char(cv, x, y, outch);

        d->increment_dither();
    }
        /* end loop */
    }

    free(floyd_steinberg);

    caca_set_attr(cv, savedattr);

    return 0;
}

/** \brief Free the memory associated with a dither.
 *
 *  Free the memory allocated by caca_create_dither().
 *
 *  This function never fails.
 *
 *  \param d Dither object.
 *  \return This function always returns 0.
 */
int caca_free_dither(caca_dither_t *d)
{
    if(!d)
        return 0;

    free(d);

    return 0;
}

/*
 * XXX: The following functions are local.
 */

/* Convert a mask, eg. 0x0000ff00, to shift values, eg. 8 and -4. */
static void mask2shift(uint32_t mask, int *right, int *left)
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

static void get_rgba_default(caca_dither_t const *d, uint8_t *pixels,
                             int x, int y, unsigned int *rgba)
{
    uint32_t bits;

    pixels += (d->bpp / 8) * x + d->pitch * y;

    switch(d->bpp / 8)
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
            uint32_t const tmp = 0x12345678;
            if(*(uint8_t const *)&tmp == 0x12)
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

    if(d->has_palette)
    {
        rgba[0] += d->gammatab[d->red[bits]];
        rgba[1] += d->gammatab[d->green[bits]];
        rgba[2] += d->gammatab[d->blue[bits]];
        rgba[3] += d->alpha[bits];
    }
    else
    {
        rgba[0] += d->gammatab[((bits & d->rmask) >> d->rright) << d->rleft];
        rgba[1] += d->gammatab[((bits & d->gmask) >> d->gright) << d->gleft];
        rgba[2] += d->gammatab[((bits & d->bmask) >> d->bright) << d->bleft];
        rgba[3] += ((bits & d->amask) >> d->aright) << d->aleft;
    }
}

/*
 * No dithering
 */
static void init_no_dither(int line)
{
    ;
}

static int get_no_dither(void)
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

static int get_fstein_dither(void)
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
static int const *ordered2_table;
static int ordered2_index;

static void init_ordered2_dither(int line)
{
    static int const dither2x2[] =
    {
        0x00, 0x80,
        0xc0, 0x40,
    };

    ordered2_table = dither2x2 + (line % 2) * 2;
    ordered2_index = 0;
}

static int get_ordered2_dither(void)
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
static int const *ordered4_table;
static int ordered4_index;

static void init_ordered4_dither(int line)
{
    static int const dither4x4[] =
    {
        0x00, 0x80, 0x20, 0xa0,
        0xc0, 0x40, 0xe0, 0x60,
        0x30, 0xb0, 0x10, 0x90,
        0xf0, 0x70, 0xd0, 0x50
    };

    ordered4_table = dither4x4 + (line % 4) * 4;
    ordered4_index = 0;
}

static int get_ordered4_dither(void)
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
static int const *ordered8_table;
static int ordered8_index;

static void init_ordered8_dither(int line)
{
    static int const dither8x8[] =
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

static int get_ordered8_dither(void)
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

static int get_random_dither(void)
{
    return caca_rand(0x00, 0x100);
}

static void increment_random_dither(void)
{
    return;
}

/*
 * Lookup tables
 */
static int init_lookup(void)
{
    int v, s, h;

    /* These ones are constant */
    lookup_colors[0] = CACA_BLACK;
    lookup_colors[1] = CACA_DARKGRAY;
    lookup_colors[2] = CACA_LIGHTGRAY;
    lookup_colors[3] = CACA_WHITE;

    /* These ones will be overwritten */
    lookup_colors[4] = CACA_MAGENTA;
    lookup_colors[5] = CACA_LIGHTMAGENTA;
    lookup_colors[6] = CACA_RED;
    lookup_colors[7] = CACA_LIGHTRED;

    for(v = 0; v < LOOKUP_VAL; v++)
        for(s = 0; s < LOOKUP_SAT; s++)
            for(h = 0; h < LOOKUP_HUE; h++)
    {
        int i, distbg, distfg, dist;
        int val, sat, hue;
        uint8_t outbg, outfg;

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

/*
 * XXX: The following functions are aliases.
 */

cucul_dither_t *cucul_create_dither(int, int, int, int, uint32_t, uint32_t,
                                    uint32_t, uint32_t)
         CACA_ALIAS(caca_create_dither);
int cucul_set_dither_palette(cucul_dither_t *, uint32_t r[], uint32_t g[],
                             uint32_t b[], uint32_t a[])
         CACA_ALIAS(caca_set_dither_palette);
int cucul_set_dither_brightness(cucul_dither_t *, float)
         CACA_ALIAS(caca_set_dither_brightness);
float cucul_get_dither_brightness(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_brightness);
int cucul_set_dither_gamma(cucul_dither_t *, float)
         CACA_ALIAS(caca_set_dither_gamma);
float cucul_get_dither_gamma(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_gamma);
int cucul_set_dither_contrast(cucul_dither_t *, float)
         CACA_ALIAS(caca_set_dither_contrast);
float cucul_get_dither_contrast(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_contrast);
int cucul_set_dither_antialias(cucul_dither_t *, char const *)
         CACA_ALIAS(caca_set_dither_antialias);
char const * const * cucul_get_dither_antialias_list(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_antialias_list);
char const * cucul_get_dither_antialias(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_antialias);
int cucul_set_dither_color(cucul_dither_t *, char const *)
         CACA_ALIAS(caca_set_dither_color);
char const * const * cucul_get_dither_color_list(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_color_list);
char const * cucul_get_dither_color(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_color);
int cucul_set_dither_charset(cucul_dither_t *, char const *)
         CACA_ALIAS(caca_set_dither_charset);
char const * const * cucul_get_dither_charset_list(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_charset_list);
char const * cucul_get_dither_charset(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_charset);
int cucul_set_dither_algorithm(cucul_dither_t *, char const *)
         CACA_ALIAS(caca_set_dither_algorithm);
char const * const * cucul_get_dither_algorithm_list(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_algorithm_list);
char const * cucul_get_dither_algorithm(cucul_dither_t const *)
         CACA_ALIAS(caca_get_dither_algorithm);
int cucul_dither_bitmap(cucul_canvas_t *, int, int, int, int,
                        cucul_dither_t const *, void *)
         CACA_ALIAS(caca_dither_bitmap);
int cucul_free_dither(cucul_dither_t *) CACA_ALIAS(caca_free_dither);

