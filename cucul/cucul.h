/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
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

/** libcucul API version */
#define CUCUL_API_VERSION_1

#ifdef __cplusplus
extern "C"
{
#endif

/** \e libcucul canvas */
typedef struct cucul_canvas cucul_canvas_t;
/** dither structure */
typedef struct cucul_dither cucul_dither_t;
/** data buffer structure */
typedef struct cucul_buffer cucul_buffer_t;
/** font structure */
typedef struct cucul_font cucul_font_t;

/** \defgroup colour libcucul colour definitions
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

/** \defgroup cucul libcucul basic functions
 *
 *  These functions provide the basic \e libcaca routines for library
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
cucul_canvas_t * cucul_create_canvas(unsigned int, unsigned int);
int cucul_set_canvas_size(cucul_canvas_t *, unsigned int, unsigned int);
unsigned int cucul_get_canvas_width(cucul_canvas_t *);
unsigned int cucul_get_canvas_height(cucul_canvas_t *);
int cucul_free_canvas(cucul_canvas_t *);
int cucul_rand(int, int);
/*  @} */

/** \defgroup buffer libcucul buffer handling
 *
 *  These functions provide methods to handle libcucul buffers.
 *
 *  @{ */
cucul_buffer_t *cucul_load_memory(void *, unsigned long int);
cucul_buffer_t *cucul_load_file(char const *);
unsigned long int cucul_get_buffer_size(cucul_buffer_t *);
void * cucul_get_buffer_data(cucul_buffer_t *);
int cucul_free_buffer(cucul_buffer_t *);
/*  @} */

/** \defgroup canvas libcucul canvas drawing
 *
 *  These functions provide low-level character printing routines and
 *  higher level graphics functions.
 *
 *  @{ */
int cucul_set_color(cucul_canvas_t *, unsigned char, unsigned char);
int cucul_set_truecolor(cucul_canvas_t *, unsigned int, unsigned int);
char const *cucul_get_color_name(unsigned int);
int cucul_putchar(cucul_canvas_t *, int, int, unsigned long int);
int cucul_putstr(cucul_canvas_t *, int, int, char const *);
int cucul_printf(cucul_canvas_t *, int, int, char const *, ...);
int cucul_clear_canvas(cucul_canvas_t *);
int cucul_blit(cucul_canvas_t *, int, int, cucul_canvas_t const *, cucul_canvas_t const *);
/*  @} */

/** \defgroup transform libcucul canvas transformation
 *
 *  These functions perform horizontal and vertical canvas flipping.
 *
 *  @{ */
int cucul_invert(cucul_canvas_t *);
int cucul_flip(cucul_canvas_t *);
int cucul_flop(cucul_canvas_t *);
int cucul_rotate(cucul_canvas_t *);
/*  @} */

/** \defgroup charset libcucul character set conversions
 *
 *  These functions perform conversions between usual character sets.
 *
 *  @{ */
extern unsigned long int cucul_utf8_to_utf32(char const *, unsigned int *);
extern unsigned int cucul_utf32_to_utf8(char *, unsigned long int);
extern unsigned char cucul_utf32_to_cp437(unsigned long int);
extern unsigned long int cucul_cp437_to_utf32(unsigned char);
/*  @} */

/** \defgroup prim libcucul primitives drawing
 *
 *  These functions provide routines for primitive drawing, such as lines,
 *  boxes, triangles and ellipses.
 *
 *  @{ */
int cucul_draw_line(cucul_canvas_t *, int, int, int, int, char const *);
int cucul_draw_polyline(cucul_canvas_t *, int const x[], int const y[], int, char const *);
int cucul_draw_thin_line(cucul_canvas_t *, int, int, int, int);
int cucul_draw_thin_polyline(cucul_canvas_t *, int const x[], int const y[], int);

int cucul_draw_circle(cucul_canvas_t *, int, int, int, char const *);
int cucul_draw_ellipse(cucul_canvas_t *, int, int, int, int, char const *);
int cucul_draw_thin_ellipse(cucul_canvas_t *, int, int, int, int);
int cucul_fill_ellipse(cucul_canvas_t *, int, int, int, int, char const *);

int cucul_draw_box(cucul_canvas_t *, int, int, int, int, char const *);
int cucul_draw_thin_box(cucul_canvas_t *, int, int, int, int);
int cucul_fill_box(cucul_canvas_t *, int, int, int, int, char const *);

int cucul_draw_triangle(cucul_canvas_t *, int, int, int, int, int, int, char const *);
int cucul_draw_thin_triangle(cucul_canvas_t *, int, int, int, int, int, int);
int cucul_fill_triangle(cucul_canvas_t *, int, int, int, int, int, int, char const *);
/*  @} */

/** \defgroup frame libcucul canvas frame handling
 *
 *  These functions provide high level routines for canvas frame insertion,
 *  removal, copying etc.
 *
 *  @{ */
unsigned int cucul_get_canvas_frame_count(cucul_canvas_t *);
int cucul_set_canvas_frame(cucul_canvas_t *, unsigned int);
int cucul_create_canvas_frame(cucul_canvas_t *, unsigned int);
int cucul_free_canvas_frame(cucul_canvas_t *, unsigned int);
/*  @} */

/** \defgroup dither libcucul bitmap dithering
 *
 *  These functions provide high level routines for dither allocation and
 *  rendering.
 *
 *  @{ */
cucul_dither_t *cucul_create_dither(unsigned int, unsigned int,
                                    unsigned int, unsigned int,
                                    unsigned long int, unsigned long int,
                                    unsigned long int, unsigned long int);
int cucul_set_dither_palette(cucul_dither_t *,
                             unsigned int r[], unsigned int g[],
                             unsigned int b[], unsigned int a[]);
int cucul_set_dither_brightness(cucul_dither_t *, float);
int cucul_set_dither_gamma(cucul_dither_t *, float);
int cucul_set_dither_contrast(cucul_dither_t *, float);
int cucul_set_dither_invert(cucul_dither_t *, int);
int cucul_set_dither_antialias(cucul_dither_t *, char const *);
char const * const * cucul_get_dither_antialias_list(cucul_dither_t const *);
int cucul_set_dither_color(cucul_dither_t *, char const *);
char const * const * cucul_get_dither_color_list(cucul_dither_t const *);
int cucul_set_dither_charset(cucul_dither_t *, char const *);
char const * const * cucul_get_dither_charset_list(cucul_dither_t const *);
int cucul_set_dither_mode(cucul_dither_t *, char const *);
char const * const * cucul_get_dither_mode_list(cucul_dither_t const *);
int cucul_dither_bitmap(cucul_canvas_t *, int, int, int, int,
                         cucul_dither_t const *, void *);
int cucul_free_dither(cucul_dither_t *);
/*  @} */

/** \defgroup font libcucul font handling
 *
 *  These functions provide font handling routines and high quality
 *  canvas to bitmap rendering.
 *
 *  @{ */
cucul_font_t *cucul_load_font(void const *, unsigned int);
char const * const * cucul_get_font_list(void);
unsigned int cucul_get_font_width(cucul_font_t *);
unsigned int cucul_get_font_height(cucul_font_t *);
int cucul_render_canvas(cucul_canvas_t *, cucul_font_t *, void *,
                         unsigned int, unsigned int, unsigned int);
int cucul_free_font(cucul_font_t *);
/*  @} */

/** \defgroup importexport libcucul importers/exporters from/to various formats
 *
 *  These functions import various file formats into a new canvas, or export
 *  the current canvas to various text formats.
 *
 *  @{ */
cucul_buffer_t * cucul_export_canvas(cucul_canvas_t *, char const *);
char const * const * cucul_get_export_list(void);
cucul_canvas_t * cucul_import_canvas(cucul_buffer_t *, char const *);
char const * const * cucul_get_import_list(void);
/*  @} */

#ifdef __cplusplus
}
#endif

#endif /* __CUCUL_H__ */
