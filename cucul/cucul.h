/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
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

#include <cucul_types.h>

#undef __extern
#if defined(_DOXYGEN_SKIP_ME)
#elif defined(_WIN32) && defined(__LIBCUCUL__)
#   define __extern extern __declspec(dllexport)
#else
#   define __extern extern
#endif

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
/** font structure */
typedef struct cucul_font cucul_font_t;
/** file handle structure */
typedef struct cucul_file cucul_file_t;

/** \defgroup cucul_attr libcucul attribute definitions
 *
 *  Colours and styles that can be used with cucul_set_attr().
 *
 *  @{ */
#define CUCUL_BLACK 0x00 /**< The colour index for black. */
#define CUCUL_BLUE 0x01 /**< The colour index for blue. */
#define CUCUL_GREEN 0x02 /**< The colour index for green. */
#define CUCUL_CYAN 0x03 /**< The colour index for cyan. */
#define CUCUL_RED 0x04 /**< The colour index for red. */
#define CUCUL_MAGENTA 0x05 /**< The colour index for magenta. */
#define CUCUL_BROWN 0x06 /**< The colour index for brown. */
#define CUCUL_LIGHTGRAY 0x07 /**< The colour index for light gray. */
#define CUCUL_DARKGRAY 0x08 /**< The colour index for dark gray. */
#define CUCUL_LIGHTBLUE 0x09 /**< The colour index for blue. */
#define CUCUL_LIGHTGREEN 0x0a /**< The colour index for light green. */
#define CUCUL_LIGHTCYAN 0x0b /**< The colour index for light cyan. */
#define CUCUL_LIGHTRED 0x0c /**< The colour index for light red. */
#define CUCUL_LIGHTMAGENTA 0x0d /**< The colour index for light magenta. */
#define CUCUL_YELLOW 0x0e /**< The colour index for yellow. */
#define CUCUL_WHITE 0x0f /**< The colour index for white. */
#define CUCUL_DEFAULT 0x10 /**< The output driver's default colour. */
#define CUCUL_TRANSPARENT 0x20 /**< The transparent colour. */

#define CUCUL_BOLD 0x01 /**< The style mask for bold. */
#define CUCUL_ITALICS 0x02 /**< The style mask for italics. */
#define CUCUL_UNDERLINE 0x04 /**< The style mask for underline. */
#define CUCUL_BLINK 0x08 /**< The style mask for blink. */
/*  @} */

/** \defgroup libcucul libcucul basic functions
 *
 *  These functions provide the basic \e libcaca routines for library
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
__extern cucul_canvas_t * cucul_create_canvas(int, int);
__extern int cucul_manage_canvas(cucul_canvas_t *, int (*)(void *), void *);
__extern int cucul_unmanage_canvas(cucul_canvas_t *, int (*)(void *), void *);
__extern int cucul_set_canvas_size(cucul_canvas_t *, int, int);
__extern int cucul_get_canvas_width(cucul_canvas_t const *);
__extern int cucul_get_canvas_height(cucul_canvas_t const *);
__extern uint8_t const * cucul_get_canvas_chars(cucul_canvas_t const *);
__extern uint8_t const * cucul_get_canvas_attrs(cucul_canvas_t const *);
__extern int cucul_free_canvas(cucul_canvas_t *);
__extern int cucul_rand(int, int);
__extern char const * cucul_get_version(void);
/*  @} */

/** \defgroup cucul_canvas libcucul canvas drawing
 *
 *  These functions provide low-level character printing routines and
 *  higher level graphics functions.
 *
 *  @{ */
#define CUCUL_MAGIC_FULLWIDTH 0x000ffffe /**< Used to indicate that the previous character was a fullwidth glyph. */
__extern int cucul_gotoxy(cucul_canvas_t *, int, int);
__extern int cucul_get_cursor_x(cucul_canvas_t const *);
__extern int cucul_get_cursor_y(cucul_canvas_t const *);
__extern int cucul_put_char(cucul_canvas_t *, int, int, uint32_t);
__extern uint32_t cucul_get_char(cucul_canvas_t const *, int, int);
__extern int cucul_put_str(cucul_canvas_t *, int, int, char const *);
__extern uint32_t cucul_get_attr(cucul_canvas_t const *, int, int);
__extern int cucul_set_attr(cucul_canvas_t *, uint32_t);
__extern int cucul_put_attr(cucul_canvas_t *, int, int, uint32_t);
__extern int cucul_set_color_ansi(cucul_canvas_t *, uint8_t, uint8_t);
__extern int cucul_set_color_argb(cucul_canvas_t *, uint16_t, uint16_t);
__extern int cucul_printf(cucul_canvas_t *, int, int, char const *, ...);
__extern int cucul_clear_canvas(cucul_canvas_t *);
__extern int cucul_set_canvas_handle(cucul_canvas_t *, int, int);
__extern int cucul_get_canvas_handle_x(cucul_canvas_t const *);
__extern int cucul_get_canvas_handle_y(cucul_canvas_t const *);
__extern int cucul_blit(cucul_canvas_t *, int, int, cucul_canvas_t const *,
                        cucul_canvas_t const *);
__extern int cucul_set_canvas_boundaries(cucul_canvas_t *, int, int, int, int);
/*  @} */

/** \defgroup cucul_transform libcucul canvas transformation
 *
 *  These functions perform horizontal and vertical canvas flipping.
 *
 *  @{ */
__extern int cucul_invert(cucul_canvas_t *);
__extern int cucul_flip(cucul_canvas_t *);
__extern int cucul_flop(cucul_canvas_t *);
__extern int cucul_rotate_180(cucul_canvas_t *);
__extern int cucul_rotate_left(cucul_canvas_t *);
__extern int cucul_rotate_right(cucul_canvas_t *);
__extern int cucul_stretch_left(cucul_canvas_t *);
__extern int cucul_stretch_right(cucul_canvas_t *);
/*  @} */

/** \defgroup cucul_attributes libcucul attribute conversions
 *
 *  These functions perform conversions between attribute values.
 *
 *  @{ */
__extern uint8_t cucul_attr_to_ansi(uint32_t);
__extern uint8_t cucul_attr_to_ansi_fg(uint32_t);
__extern uint8_t cucul_attr_to_ansi_bg(uint32_t);
__extern uint16_t cucul_attr_to_rgb12_fg(uint32_t);
__extern uint16_t cucul_attr_to_rgb12_bg(uint32_t);
__extern void cucul_attr_to_argb64(uint32_t, uint8_t[8]);
/*  @} */

/** \defgroup cucul_charset libcucul character set conversions
 *
 *  These functions perform conversions between usual character sets.
 *
 *  @{ */
__extern uint32_t cucul_utf8_to_utf32(char const *, size_t *);
__extern size_t cucul_utf32_to_utf8(char *, uint32_t);
__extern uint8_t cucul_utf32_to_cp437(uint32_t);
__extern uint32_t cucul_cp437_to_utf32(uint8_t);
__extern char cucul_utf32_to_ascii(uint32_t);
__extern int cucul_utf32_is_fullwidth(uint32_t);
/*  @} */

/** \defgroup cucul_primitives libcucul primitives drawing
 *
 *  These functions provide routines for primitive drawing, such as lines,
 *  boxes, triangles and ellipses.
 *
 *  @{ */
__extern int cucul_draw_line(cucul_canvas_t *, int, int, int, int, uint32_t);
__extern int cucul_draw_polyline(cucul_canvas_t *, int const x[],
                                 int const y[], int, uint32_t);
__extern int cucul_draw_thin_line(cucul_canvas_t *, int, int, int, int);
__extern int cucul_draw_thin_polyline(cucul_canvas_t *, int const x[],
                                      int const y[], int);

__extern int cucul_draw_circle(cucul_canvas_t *, int, int, int, uint32_t);
__extern int cucul_draw_ellipse(cucul_canvas_t *, int, int, int, int, uint32_t);
__extern int cucul_draw_thin_ellipse(cucul_canvas_t *, int, int, int, int);
__extern int cucul_fill_ellipse(cucul_canvas_t *, int, int, int, int, uint32_t);

__extern int cucul_draw_box(cucul_canvas_t *, int, int, int, int, uint32_t);
__extern int cucul_draw_thin_box(cucul_canvas_t *, int, int, int, int);
__extern int cucul_draw_cp437_box(cucul_canvas_t *, int, int, int, int);
__extern int cucul_fill_box(cucul_canvas_t *, int, int, int, int, uint32_t);

__extern int cucul_draw_triangle(cucul_canvas_t *, int, int, int, int, int,
                                 int, uint32_t);
__extern int cucul_draw_thin_triangle(cucul_canvas_t *, int, int, int, int,
                                      int, int);
__extern int cucul_fill_triangle(cucul_canvas_t *, int, int, int, int, int,
                                 int, uint32_t);
/*  @} */

/** \defgroup cucul_frame libcucul canvas frame handling
 *
 *  These functions provide high level routines for canvas frame insertion,
 *  removal, copying etc.
 *
 *  @{ */
__extern int cucul_get_frame_count(cucul_canvas_t const *);
__extern int cucul_set_frame(cucul_canvas_t *, int);
__extern char const *cucul_get_frame_name(cucul_canvas_t const *);
__extern int cucul_set_frame_name(cucul_canvas_t *, char const *);
__extern int cucul_create_frame(cucul_canvas_t *, int);
__extern int cucul_free_frame(cucul_canvas_t *, int);
/*  @} */

/** \defgroup cucul_dither libcucul bitmap dithering
 *
 *  These functions provide high level routines for dither allocation and
 *  rendering.
 *
 *  @{ */
__extern cucul_dither_t *cucul_create_dither(int, int, int, int,
                                             uint32_t, uint32_t,
                                             uint32_t, uint32_t);
__extern int cucul_set_dither_palette(cucul_dither_t *,
                                      uint32_t r[], uint32_t g[],
                                      uint32_t b[], uint32_t a[]);
__extern int cucul_set_dither_brightness(cucul_dither_t *, float);
__extern float cucul_get_dither_brightness(cucul_dither_t const *);
__extern int cucul_set_dither_gamma(cucul_dither_t *, float);
__extern float cucul_get_dither_gamma(cucul_dither_t const *);
__extern int cucul_set_dither_contrast(cucul_dither_t *, float);
__extern float cucul_get_dither_contrast(cucul_dither_t const *);
__extern int cucul_set_dither_antialias(cucul_dither_t *, char const *);
__extern char const * const * cucul_get_dither_antialias_list(cucul_dither_t
                                                              const *);
__extern char const * cucul_get_dither_antialias(cucul_dither_t const *);
__extern int cucul_set_dither_color(cucul_dither_t *, char const *);
__extern char const * const * cucul_get_dither_color_list(cucul_dither_t
                                                          const *);
__extern char const * cucul_get_dither_color(cucul_dither_t const *);
__extern int cucul_set_dither_charset(cucul_dither_t *, char const *);
__extern char const * const * cucul_get_dither_charset_list(cucul_dither_t
                                                            const *);
__extern char const * cucul_get_dither_charset(cucul_dither_t const *);
__extern int cucul_set_dither_algorithm(cucul_dither_t *, char const *);
__extern char const * const * cucul_get_dither_algorithm_list(cucul_dither_t
                                                              const *);
__extern char const * cucul_get_dither_algorithm(cucul_dither_t const *);
__extern int cucul_dither_bitmap(cucul_canvas_t *, int, int, int, int,
                         cucul_dither_t const *, void *);
__extern int cucul_free_dither(cucul_dither_t *);
/*  @} */

/** \defgroup cucul_font libcucul font handling
 *
 *  These functions provide font handling routines and high quality
 *  canvas to bitmap rendering.
 *
 *  @{ */
__extern cucul_font_t *cucul_load_font(void const *, size_t);
__extern char const * const * cucul_get_font_list(void);
__extern int cucul_get_font_width(cucul_font_t const *);
__extern int cucul_get_font_height(cucul_font_t const *);
__extern uint32_t const *cucul_get_font_blocks(cucul_font_t const *);
__extern int cucul_render_canvas(cucul_canvas_t const *, cucul_font_t const *,
                                 void *, int, int, int);
__extern int cucul_free_font(cucul_font_t *);
/*  @} */

/** \defgroup cucul_figfont libcucul FIGfont handling
 *
 *  These functions provide FIGlet and TOIlet font handling routines.
 *
 *  @{ */
__extern int cucul_canvas_set_figfont(cucul_canvas_t *, char const *);
__extern int cucul_put_figchar(cucul_canvas_t *, uint32_t);
__extern int cucul_flush_figlet(cucul_canvas_t *);
/*  @} */

/** \defgroup cucul_file libcucul file IO
 *
 *  These functions allow to read and write files in a platform-independent
 *  way.
 *  @{ */
__extern cucul_file_t *cucul_file_open(char const *, const char *);
__extern int cucul_file_close(cucul_file_t *);
__extern uint64_t cucul_file_tell(cucul_file_t *);
__extern size_t cucul_file_read(cucul_file_t *, void *, size_t);
__extern size_t cucul_file_write(cucul_file_t *, const void *, size_t);
__extern char * cucul_file_gets(cucul_file_t *, char *, int);
__extern int cucul_file_eof(cucul_file_t *);
/*  @} */

/** \defgroup cucul_importexport libcucul importers/exporters from/to various
 *  formats
 *
 *  These functions import various file formats into a new canvas, or export
 *  the current canvas to various text formats.
 *
 *  @{ */
__extern ssize_t cucul_import_memory(cucul_canvas_t *, void const *,
                                     size_t, char const *);
__extern ssize_t cucul_import_file(cucul_canvas_t *, char const *,
                                   char const *);
__extern char const * const * cucul_get_import_list(void);
__extern void *cucul_export_memory(cucul_canvas_t const *, char const *,
                                   size_t *);
__extern char const * const * cucul_get_export_list(void);
/*  @} */

#if !defined(_DOXYGEN_SKIP_ME)
    /* Legacy stuff from beta versions, will probably disappear in 1.0 */
typedef struct cucul_buffer cucul_buffer_t;
#   if defined __GNUC__ && __GNUC__ >= 3
#       define CUCUL_DEPRECATED __attribute__ ((__deprecated__))
#   else
#       define CUCUL_DEPRECATED
#   endif
__extern int cucul_putchar(cucul_canvas_t *, int, int,
                           unsigned long int) CUCUL_DEPRECATED;
__extern unsigned long int cucul_getchar(cucul_canvas_t *,
                                         int, int) CUCUL_DEPRECATED;
__extern int cucul_putstr(cucul_canvas_t *, int, int,
                          char const *) CUCUL_DEPRECATED;
__extern int cucul_set_color(cucul_canvas_t *, unsigned char,
                             unsigned char) CUCUL_DEPRECATED;
__extern int cucul_set_truecolor(cucul_canvas_t *, unsigned int,
                                 unsigned int) CUCUL_DEPRECATED;
__extern unsigned int cucul_get_canvas_frame_count(cucul_canvas_t *)
                                                   CUCUL_DEPRECATED;
__extern int cucul_set_canvas_frame(cucul_canvas_t *,
                                    unsigned int) CUCUL_DEPRECATED;
__extern int cucul_create_canvas_frame(cucul_canvas_t *,
                                       unsigned int) CUCUL_DEPRECATED;
__extern int cucul_free_canvas_frame(cucul_canvas_t *,
                                     unsigned int) CUCUL_DEPRECATED;
__extern cucul_buffer_t *cucul_load_memory(void *,
                                           unsigned long int) CUCUL_DEPRECATED;
__extern cucul_buffer_t *cucul_load_file(char const *) CUCUL_DEPRECATED;
__extern unsigned long int cucul_get_buffer_size(cucul_buffer_t *)
                                                 CUCUL_DEPRECATED;
__extern void * cucul_get_buffer_data(cucul_buffer_t *) CUCUL_DEPRECATED;
__extern int cucul_free_buffer(cucul_buffer_t *) CUCUL_DEPRECATED;
__extern cucul_buffer_t * cucul_export_canvas(cucul_canvas_t *,
                                              char const *) CUCUL_DEPRECATED;
__extern cucul_canvas_t * cucul_import_canvas(cucul_buffer_t *,
                                              char const *) CUCUL_DEPRECATED;
__extern int cucul_rotate(cucul_canvas_t *) CUCUL_DEPRECATED;
__extern int cucul_set_dither_invert(cucul_dither_t *, int) CUCUL_DEPRECATED;
__extern int cucul_set_dither_mode(cucul_dither_t *,
                                   char const *) CUCUL_DEPRECATED;
__extern char const * const * cucul_get_dither_mode_list(cucul_dither_t
                                                         const *)
                                                         CUCUL_DEPRECATED;
#   define CUCUL_COLOR_BLACK CUCUL_BLACK
#   define CUCUL_COLOR_BLUE CUCUL_BLUE
#   define CUCUL_COLOR_GREEN CUCUL_GREEN
#   define CUCUL_COLOR_CYAN CUCUL_CYAN
#   define CUCUL_COLOR_RED CUCUL_RED
#   define CUCUL_COLOR_MAGENTA CUCUL_MAGENTA
#   define CUCUL_COLOR_BROWN CUCUL_BROWN
#   define CUCUL_COLOR_LIGHTGRAY CUCUL_LIGHTGRAY
#   define CUCUL_COLOR_DARKGRAY CUCUL_DARKGRAY
#   define CUCUL_COLOR_LIGHTBLUE CUCUL_LIGHTBLUE
#   define CUCUL_COLOR_LIGHTGREEN CUCUL_LIGHTGREEN
#   define CUCUL_COLOR_LIGHTCYAN CUCUL_LIGHTCYAN
#   define CUCUL_COLOR_LIGHTRED CUCUL_LIGHTRED
#   define CUCUL_COLOR_LIGHTMAGENTA CUCUL_LIGHTMAGENTA
#   define CUCUL_COLOR_YELLOW CUCUL_YELLOW
#   define CUCUL_COLOR_WHITE CUCUL_YELLOW
#   define CUCUL_COLOR_DEFAULT CUCUL_DEFAULT
#   define CUCUL_COLOR_TRANSPARENT CUCUL_TRANSPARENT
#endif

#ifdef __cplusplus
}
#endif

#undef __extern

#endif /* __CUCUL_H__ */
