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

typedef struct cucul_context cucul_t;

struct cucul_buffer
{
    unsigned int size;
    char *data;
};

/** \defgroup colour Colour definitions
 *
 *  Colours that can be used with cucul_set_color().
 *
 *  @{ */
#define CUCUL_COLOR_BLACK 0x00 /**< The colour index for black. */
#define CUCUL_COLOR_BLUE 0x01 /**< The colour index for blue. */
#define CUCUL_COLOR_GREEN 0x02 /**< The colour index for green. */
#define CUCUL_COLOR_CYAN 0x03 /**< The colour index for cyan. */
#define CUCUL_COLOR_RED 0x04 /**< The colour index for red. */
#define CUCUL_COLOR_MAGENTA 0x05 /**< The colour index for magenta. */
#define CUCUL_COLOR_BROWN 0x06 /**< The colour index for brown. */
#define CUCUL_COLOR_LIGHTGRAY 0x07 /**< The colour index for light gray. */
#define CUCUL_COLOR_DARKGRAY 0x08 /**< The colour index for dark gray. */
#define CUCUL_COLOR_LIGHTBLUE 0x09 /**< The colour index for blue. */
#define CUCUL_COLOR_LIGHTGREEN 0x0a /**< The colour index for light green. */
#define CUCUL_COLOR_LIGHTCYAN 0x0b /**< The colour index for light cyan. */
#define CUCUL_COLOR_LIGHTRED 0x0c /**< The colour index for light red. */
#define CUCUL_COLOR_LIGHTMAGENTA 0x0d /**< The colour index for light magenta. */
#define CUCUL_COLOR_YELLOW 0x0e /**< The colour index for yellow. */
#define CUCUL_COLOR_WHITE 0x0f /**< The colour index for white. */
#define CUCUL_COLOR_DEFAULT 0x10 /**< The output driver's default colour. */
#define CUCUL_COLOR_TRANSPARENT 0x20 /**< The transparent colour. */
/*  @} */

/** \defgroup basic Basic functions
 *
 *  These functions provide the basic \e libcaca routines for library
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
cucul_t * cucul_create(unsigned int, unsigned int);
cucul_t * cucul_load(void *, unsigned int);
void cucul_set_size(cucul_t *, unsigned int, unsigned int);
unsigned int cucul_get_width(cucul_t *);
unsigned int cucul_get_height(cucul_t *);
void cucul_free(cucul_t *);
/*  @} */

/** \defgroup canvas Canvas drawing
 *
 *  These functions provide low-level character printing routines and
 *  higher level graphics functions.
 *
 *  @{ */
void cucul_set_color(cucul_t *, unsigned int, unsigned int);
char const *cucul_get_color_name(unsigned int);
void cucul_putchar(cucul_t *, int, int, char);
void cucul_putstr(cucul_t *, int, int, char const *);
void cucul_printf(cucul_t *, int, int, char const *, ...);
void cucul_clear(cucul_t *);
void cucul_blit(cucul_t *, int, int, cucul_t const *, cucul_t const *);
/*  @} */

/** \defgroup transform Canvas transformation
 *
 *  These functions perform horizontal and vertical canvas flipping.
 *
 *  @{ */
void cucul_invert(cucul_t *);
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

/** \defgroup dither Bitmap dithering
 *
 *  These functions provide high level routines for dither allocation and
 *  rendering.
 *
 *  @{ */
struct cucul_dither;
struct cucul_dither *cucul_create_dither(unsigned int, unsigned int,
                                         unsigned int, unsigned int,
                                         unsigned int, unsigned int,
                                         unsigned int, unsigned int);
void cucul_set_dither_palette(struct cucul_dither *,
                              unsigned int r[], unsigned int g[],
                              unsigned int b[], unsigned int a[]);
void cucul_set_dither_brightness(struct cucul_dither *, float);
void cucul_set_dither_gamma(struct cucul_dither *, float);
void cucul_set_dither_contrast(struct cucul_dither *, float);
void cucul_set_dither_invert(struct cucul_dither *, int);
void cucul_set_dither_antialias(struct cucul_dither *, char const *);
char const * const * cucul_get_dither_antialias_list(struct cucul_dither const *);
void cucul_set_dither_color(struct cucul_dither *, char const *);
char const * const * cucul_get_dither_color_list(struct cucul_dither const *);
void cucul_set_dither_charset(struct cucul_dither *, char const *);
char const * const * cucul_get_dither_charset_list(struct cucul_dither const *);
void cucul_set_dither_mode(struct cucul_dither *, char const *);
char const * const * cucul_get_dither_mode_list(struct cucul_dither const *);
void cucul_dither_bitmap(cucul_t *, int, int, int, int,
                         struct cucul_dither const *, void *);
void cucul_free_dither(struct cucul_dither *);
/*  @} */

/** \defgroup font Font handling
 *
 *  These functions provide font handling routines and high quality
 *  canvas to bitmap rendering.
 *
 *  @{ */
struct cucul_font;
struct cucul_font *cucul_load_font(void const *, unsigned int);
char const * const * cucul_get_font_list(void);
unsigned int cucul_get_font_width(struct cucul_font *);
unsigned int cucul_get_font_height(struct cucul_font *);
void cucul_render_canvas(cucul_t *, struct cucul_font *, unsigned char *,
                         unsigned int, unsigned int, unsigned int);
void cucul_free_font(struct cucul_font *);
/*  @} */

/** \defgroup exporter Exporters to various formats
 *
 *  These functions export the current canvas to various text formats. It
 *  is necessary to call cucul_free_export() to dispose of the data.
 *
 *  @{ */
struct cucul_buffer * cucul_create_export(cucul_t *, char const *);
char const * const * cucul_get_export_list(void);
void cucul_free_export(struct cucul_buffer *);
/*  @} */

#ifdef __cplusplus
}
#endif

#endif /* __CUCUL_H__ */
