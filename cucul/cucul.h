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

/** \file cucul.h
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief The \e libcucul public header.
 *
 *  This header contains the public types and functions that applications
 *  using \e libcucul may use.
 */

#ifndef __CUCUL_H__
#define __CUCUL_H__

#define CUCUL_API_VERSION_1

#ifdef __cplusplus
extern "C"
{
#endif

/** \brief Colour definitions.
 *
 *  Colours that can be used with cucul_set_color().
 */
enum cucul_color
{
    CUCUL_COLOR_BLACK = 0, /**< The colour index for black. */
    CUCUL_COLOR_BLUE = 1, /**< The colour index for blue. */
    CUCUL_COLOR_GREEN = 2, /**< The colour index for green. */
    CUCUL_COLOR_CYAN = 3, /**< The colour index for cyan. */
    CUCUL_COLOR_RED = 4, /**< The colour index for red. */
    CUCUL_COLOR_MAGENTA = 5, /**< The colour index for magenta. */
    CUCUL_COLOR_BROWN = 6, /**< The colour index for brown. */
    CUCUL_COLOR_LIGHTGRAY = 7, /**< The colour index for light gray. */
    CUCUL_COLOR_DARKGRAY = 8, /**< The colour index for dark gray. */
    CUCUL_COLOR_LIGHTBLUE = 9, /**< The colour index for blue. */
    CUCUL_COLOR_LIGHTGREEN = 10, /**< The colour index for light green. */
    CUCUL_COLOR_LIGHTCYAN = 11, /**< The colour index for light cyan. */
    CUCUL_COLOR_LIGHTRED = 12, /**< The colour index for light red. */
    CUCUL_COLOR_LIGHTMAGENTA = 13, /**< The colour index for light magenta. */
    CUCUL_COLOR_YELLOW = 14, /**< The colour index for yellow. */
    CUCUL_COLOR_WHITE = 15 /**< The colour index for white. */
};

/** \brief Export formats
 *
 *  Export formats understood by libcucul.
 */
enum cucul_format
{
    CUCUL_FORMAT_ANSI = 0, /**< Export to ANSI format. */
    CUCUL_FORMAT_HTML = 1, /**< Export to HTML format. */
    CUCUL_FORMAT_HTML3 = 2, /**< Export to old HTMLv3 format. */
    CUCUL_FORMAT_IRC = 3, /**< Export to text with mIRC colours. */
    CUCUL_FORMAT_PS = 4, /**< Export to PostScript. */
    CUCUL_FORMAT_SVG = 5, /**< Export to Scalable Vector Graphics format. */
};

/** \brief Internal features.
 *
 *  Internal libcaca features such as the rendering method or the dithering
 *  mode.
 */
enum cucul_feature
{
    CUCUL_BACKGROUND       = 0x10, /**< Properties of background characters. */
    CUCUL_BACKGROUND_BLACK = 0x11, /**< Draw only black backgrounds. */
    CUCUL_BACKGROUND_SOLID = 0x12, /**< Draw coloured solid backgorunds. */
#define CUCUL_BACKGROUND_MIN 0x11 /**< First background property */
#define CUCUL_BACKGROUND_MAX 0x12 /**< Last background property */

    CUCUL_ANTIALIASING           = 0x20, /**< Antialiasing features. */
    CUCUL_ANTIALIASING_NONE      = 0x21, /**< No antialiasing. */
    CUCUL_ANTIALIASING_PREFILTER = 0x22, /**< Prefilter antialiasing. */
#define CUCUL_ANTIALIASING_MIN     0x21 /**< First antialiasing feature. */
#define CUCUL_ANTIALIASING_MAX     0x22 /**< Last antialiasing feature. */

    CUCUL_DITHERING          = 0x30, /**< Dithering methods */
    CUCUL_DITHERING_NONE     = 0x31, /**< No dithering. */
    CUCUL_DITHERING_ORDERED2 = 0x32, /**< Ordered 2x2 Bayer dithering. */
    CUCUL_DITHERING_ORDERED4 = 0x33, /**< Ordered 4x4 Bayer dithering. */
    CUCUL_DITHERING_ORDERED8 = 0x34, /**< Ordered 8x8 Bayer dithering. */
    CUCUL_DITHERING_RANDOM   = 0x35, /**< Random dithering. */
    CUCUL_DITHERING_FSTEIN   = 0x36, /**< Floyd-Steinberg dithering. */
#define CUCUL_DITHERING_MIN    0x31 /**< First dithering feature. */
#define CUCUL_DITHERING_MAX    0x36 /**< Last dithering feature. */

    CUCUL_FEATURE_UNKNOWN = 0xffff /**< Unknown feature. */
};

/*
 * Backwards compatibility macros
 */
#if !defined(_DOXYGEN_SKIP_ME)
#define caca_dithering cucul_feature
#define caca_set_dithering caca_set_feature
#define caca_get_dithering_name caca_get_feature_name
#define CACA_DITHER_NONE    CUCUL_DITHERING_NONE
#define CACA_DITHER_ORDERED CUCUL_DITHERING_ORDERED8
#define CACA_DITHER_RANDOM  CUCUL_DITHERING_RANDOM
#endif

typedef struct cucul_context cucul_t;

/** \defgroup basic Basic functions
 *
 *  These functions provide the basic \e libcaca routines for library
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
cucul_t * cucul_create(unsigned int, unsigned int);
void cucul_set_size(cucul_t *, unsigned int, unsigned int);
unsigned int cucul_get_width(cucul_t *);
unsigned int cucul_get_height(cucul_t *);
enum cucul_feature cucul_get_feature(cucul_t *, enum cucul_feature);
void cucul_set_feature(cucul_t *, enum cucul_feature);
char const *cucul_get_feature_name(enum cucul_feature);
void cucul_free(cucul_t *);
/*  @} */

/** \defgroup canvas Canvas drawing
 *
 *  These functions provide low-level character printing routines and
 *  higher level graphics functions.
 *
 *  @{ */
void cucul_set_color(cucul_t *, enum cucul_color, enum cucul_color);
enum cucul_color cucul_get_fg_color(cucul_t const *);
enum cucul_color cucul_get_bg_color(cucul_t const *);
char const *cucul_get_color_name(enum cucul_color);
void cucul_putchar(cucul_t *, int, int, char);
void cucul_putstr(cucul_t *, int, int, char const *);
void cucul_printf(cucul_t *, int, int, char const *, ...);
void cucul_get_screen(cucul_t const *, char *);
void cucul_clear(cucul_t *);
void cucul_blit(cucul_t *, int, int, cucul_t const *, cucul_t const *);
/*  @} */

/** \defgroup transform Canvas transformation
 *
 *  These functions perform horizontal and vertical canvas flipping.
 *
 *  @{ */
void cucul_flip(cucul_t *);
void cucul_flop(cucul_t *);
void cucul_rotate(cucul_t *);
/*  @} */

/** \defgroup prim Primitives drawing
 *
 *  These functions provide routines for primitive drawing, such as lines,
 *  boxes, triangles and ellipses.
 *
 *  @{ */
void cucul_draw_line(cucul_t *, int, int, int, int, char const *);
void cucul_draw_polyline(cucul_t *, int const x[], int const y[], int, char const *);
void cucul_draw_thin_line(cucul_t *, int, int, int, int);
void cucul_draw_thin_polyline(cucul_t *, int const x[], int const y[], int);

void cucul_draw_circle(cucul_t *, int, int, int, char const *);
void cucul_draw_ellipse(cucul_t *, int, int, int, int, char const *);
void cucul_draw_thin_ellipse(cucul_t *, int, int, int, int);
void cucul_fill_ellipse(cucul_t *, int, int, int, int, char const *);

void cucul_draw_box(cucul_t *, int, int, int, int, char const *);
void cucul_draw_thin_box(cucul_t *, int, int, int, int);
void cucul_fill_box(cucul_t *, int, int, int, int, char const *);

void cucul_draw_triangle(cucul_t *, int, int, int, int, int, int, char const *);
void cucul_draw_thin_triangle(cucul_t *, int, int, int, int, int, int);
void cucul_fill_triangle(cucul_t *, int, int, int, int, int, int, char const *);
/*  @} */

/** \defgroup math Mathematical functions
 *
 *  These functions provide a few useful math-related routines.
 *
 *  @{ */
int cucul_rand(int, int);
unsigned int cucul_sqrt(unsigned int);
/*  @} */

/** \defgroup sprite Sprite handling
 *
 *  These functions provide high level routines for sprite loading, animation
 *  and rendering.
 *
 *  @{ */
struct cucul_sprite;
struct cucul_sprite * cucul_load_sprite(cucul_t *, char const *);
int cucul_get_sprite_frames(cucul_t *, struct cucul_sprite const *);
int cucul_get_sprite_width(cucul_t *, struct cucul_sprite const *, int);
int cucul_get_sprite_height(cucul_t *, struct cucul_sprite const *, int);
int cucul_get_sprite_dx(cucul_t *, struct cucul_sprite const *, int);
int cucul_get_sprite_dy(cucul_t *, struct cucul_sprite const *, int);
void cucul_draw_sprite(cucul_t *, int, int, struct cucul_sprite const *, int);
void cucul_free_sprite(struct cucul_sprite *);
/*  @} */

/** \defgroup bitmap Bitmap handling
 *
 *  These functions provide high level routines for bitmap allocation and
 *  rendering.
 *
 *  @{ */
struct cucul_bitmap;
struct cucul_bitmap *cucul_create_bitmap(unsigned int, unsigned int,
                                         unsigned int, unsigned int,
                                         unsigned int, unsigned int,
                                         unsigned int, unsigned int);
void cucul_set_bitmap_palette(struct cucul_bitmap *,
                              unsigned int r[], unsigned int g[],
                              unsigned int b[], unsigned int a[]);
void cucul_set_bitmap_gamma(struct cucul_bitmap *, float);
void cucul_draw_bitmap(cucul_t *, int, int, int, int, struct cucul_bitmap const *, void *);
void cucul_free_bitmap(struct cucul_bitmap *);
/*  @} */

/** \defgroup exporter Exporters to various formats
 *
 *  These functions export the current canvas to various text formats. It
 *  is necessary to call cucul_free_export() to dispose of the data.
 *
 *  @{ */
struct cucul_export
{
    unsigned int size;
    char *buffer;
};

struct cucul_export * cucul_create_export(cucul_t *, enum cucul_format);
void cucul_free_export(struct cucul_export *);

/*  @} */

#ifdef __cplusplus
}
#endif

#endif /* __CUCUL_H__ */
