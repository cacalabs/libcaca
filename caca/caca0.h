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
 *  This header contains glue code for applications using the pre-1.0
 *  libcaca API.
 */

#ifndef __CACA0_H__
#define __CACA0_H__

#include <caca.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Function emulation */
extern int __caca0_init(void);
extern void __caca0_end(void);
extern unsigned int __caca0_get_event(unsigned int, int);
extern unsigned int __caca0_sqrt(unsigned int);
extern int __caca0_get_feature(int);
extern void __caca0_set_feature(int);
extern char const *__caca0_get_feature_name(int);
extern cucul_canvas_t *__caca0_load_sprite(char const *);
extern cucul_dither_t *__caca0_create_bitmap(unsigned int, unsigned int,
          unsigned int, unsigned int, unsigned long int, unsigned long int,
          unsigned long int, unsigned long int);
extern void __caca0_free_bitmap(cucul_dither_t *);
extern char const *__caca0_get_color_name(unsigned char);

/* These variables are needed to emulate old non-thread safe behaviour */
extern cucul_canvas_t *__caca0_cv;
extern caca_display_t *__caca0_dp;
extern unsigned char __caca0_fg;
extern unsigned char __caca0_bg;

/* These enums and macros changed names or values */
enum caca_color
{
    CACA_COLOR_BLACK = CUCUL_BLACK,
    CACA_COLOR_BLUE = CUCUL_BLUE,
    CACA_COLOR_GREEN = CUCUL_GREEN,
    CACA_COLOR_CYAN = CUCUL_CYAN,
    CACA_COLOR_RED = CUCUL_RED,
    CACA_COLOR_MAGENTA = CUCUL_MAGENTA,
    CACA_COLOR_BROWN = CUCUL_BROWN,
    CACA_COLOR_LIGHTGRAY = CUCUL_LIGHTGRAY,
    CACA_COLOR_DARKGRAY = CUCUL_DARKGRAY,
    CACA_COLOR_LIGHTBLUE = CUCUL_LIGHTBLUE,
    CACA_COLOR_LIGHTGREEN = CUCUL_LIGHTGREEN,
    CACA_COLOR_LIGHTCYAN = CUCUL_LIGHTCYAN,
    CACA_COLOR_LIGHTRED = CUCUL_LIGHTRED,
    CACA_COLOR_LIGHTMAGENTA = CUCUL_LIGHTMAGENTA,
    CACA_COLOR_YELLOW = CUCUL_YELLOW,
    CACA_COLOR_WHITE = CUCUL_WHITE,
};

enum caca_feature
{
    CACA_BACKGROUND       = 0x10,
    CACA_BACKGROUND_BLACK = 0x11,
    CACA_BACKGROUND_SOLID = 0x12,
#define CACA_BACKGROUND_MIN 0x11
#define CACA_BACKGROUND_MAX 0x12
    CACA_ANTIALIASING           = 0x20,
    CACA_ANTIALIASING_NONE      = 0x21,
    CACA_ANTIALIASING_PREFILTER = 0x22,
#define CACA_ANTIALIASING_MIN     0x21
#define CACA_ANTIALIASING_MAX     0x22
    CACA_DITHERING          = 0x30,
    CACA_DITHERING_NONE     = 0x31,
    CACA_DITHERING_ORDERED2 = 0x32,
    CACA_DITHERING_ORDERED4 = 0x33,
    CACA_DITHERING_ORDERED8 = 0x34,
    CACA_DITHERING_RANDOM   = 0x35,
#define CACA_DITHERING_MIN    0x31
#define CACA_DITHERING_MAX    0x35
    CACA_FEATURE_UNKNOWN = 0xffff
};

/* This enum still exists in libcaca 1.x, thus cannot be redefined */
#define CACA_EVENT_NONE          0x00000000
#define CACA_EVENT_KEY_PRESS     0x01000000
#define CACA_EVENT_KEY_RELEASE   0x02000000
#define CACA_EVENT_MOUSE_PRESS   0x04000000
#define CACA_EVENT_MOUSE_RELEASE 0x08000000
#define CACA_EVENT_MOUSE_MOTION  0x10000000
#define CACA_EVENT_RESIZE        0x20000000
#define CACA_EVENT_ANY           0xff000000

/* Backwards compatibility macros for even older shit */
#define caca_dithering caca_feature
#define caca_set_dithering caca_set_feature
#define caca_get_dithering_name caca_get_feature_name
#define CACA_DITHER_NONE    CACA_DITHERING_NONE
#define CACA_DITHER_ORDERED CACA_DITHERING_ORDERED8
#define CACA_DITHER_RANDOM  CACA_DITHERING_RANDOM

/* Macros to redefine all the shit */
#define caca_init __caca0_init
#define caca_set_delay(x) caca_set_display_time(__caca0_dp, x)
#define caca_get_feature __caca0_get_feature
#define caca_set_feature __caca0_set_feature
#define caca_get_feature_name __caca0_get_feature_name
#define caca_get_rendertime() caca_get_display_time(__caca0_dp)
#define caca_get_width() cucul_get_canvas_width(__caca0_cv)
#define caca_get_height() cucul_get_canvas_height(__caca0_cv)
#define caca_set_window_title(s) caca_set_display_title(__caca0_dp, s)
#define caca_get_window_width() caca_get_display_width(__caca0_dp)
#define caca_get_window_height() caca_get_display_height(__caca0_dp)
#define caca_refresh() caca_refresh_display(__caca0_dp)
#define caca_end __caca0_end

#define caca_get_event(x) __caca0_get_event(x, 0)
#define caca_wait_event(x) __caca0_get_event(x, -1)
#define caca_get_mouse_x() caca_get_mouse_x(__caca0_dp)
#define caca_get_mouse_y() caca_get_mouse_y(__caca0_dp)

#define caca_set_color(x, y) \
    (__caca0_fg = (x), __caca0_bg = (y), cucul_set_color_ansi(__caca0_cv, x, y))
#define caca_get_fg_color() __caca0_fg
#define caca_get_bg_color() __caca0_bg
#define caca_get_color_name __caca0_get_color_name
#define caca_putchar(x, y, c) cucul_put_char(__caca0_cv, x, y, c)
#define caca_putstr(x, y, s) cucul_put_str(__caca0_cv, x, y, s)
#define caca_printf(x, y, f, z...) cucul_printf(__caca0_cv, x, y, f, ##z)
#define caca_clear() cucul_clear_canvas(__caca0_cv)

#define caca_draw_line(x, y, z, t, c) \
    cucul_draw_line(__caca0_cv, x, y, z, t, c)
#define caca_draw_polyline(x, y, z, c) \
    cucul_draw_polyline(__caca0_cv, x, y, z, c)
#define caca_draw_thin_line(x, y, z, t) \
    cucul_draw_thin_line(__caca0_cv, x, y, z, t)
#define caca_draw_thin_polyline(x, y, z) \
    cucul_draw_thin_polyline(__caca0_cv, x, y, z)
#define caca_draw_circle(x, y, z, c) \
    cucul_draw_circle(__caca0_cv, x, y, z, c)
#define caca_draw_ellipse(x, y, z, t, c) \
    cucul_draw_ellipse(__caca0_cv, x, y, z, t, c)
#define caca_draw_thin_ellipse(x, y, z, t) \
    cucul_draw_thin_ellipse(__caca0_cv, x, y, z, t)
#define caca_fill_ellipse(x, y, z, t, c) \
    cucul_fill_ellipse(__caca0_cv, x, y, z, t, c)
#define caca_draw_box(x, y, z, t, c) \
    cucul_draw_box(__caca0_cv, x, y, z, t, c)
#define caca_draw_thin_box(x, y, z, t) \
    cucul_draw_thin_box(__caca0_cv, x, y, z, t)
#define caca_fill_box(x, y, z, t, c) \
    cucul_fill_box(__caca0_cv, x, y, z, t, c)
#define caca_draw_triangle(x, y, z, t, u, v, c) \
    cucul_draw_triangle(__caca0_cv, x, y, z, t, u, v, c)
#define caca_draw_thin_triangle(x, y, z, t, u, v) \
    cucul_draw_thin_triangle(__caca0_cv, x, y, z, t, u, v)
#define caca_fill_triangle(x, y, z, t, u, v, c) \
    cucul_fill_triangle(__caca0_cv, x, y, z, t, u, v, c)

#define caca_rand(a, b) cucul_rand(a, (b)+1)
#define caca_sqrt __caca0_sqrt

#define caca_sprite cucul_canvas
#define caca_load_sprite __caca0_load_sprite
#define caca_get_sprite_frames(c) 1
#define caca_get_sprite_width(c, f) cucul_get_canvas_width(c)
#define caca_get_sprite_height(c, f) cucul_get_canvas_height(c)
#define caca_get_sprite_dx(c, f) 0
#define caca_get_sprite_dx(c, f) 0
#define caca_draw_sprite(x, y, c, f) cucul_blit(__caca0_cv, x, y, c, NULL)
#define caca_free_sprite cucul_free_canvas

#define caca_bitmap cucul_dither
#define caca_create_bitmap __caca0_create_bitmap
#define caca_set_bitmap_palette cucul_set_dither_palette
#define caca_draw_bitmap(x, y, z, t, b, p) \
    cucul_dither_bitmap(__caca0_cv, x, y, z, t, b, p)
#define caca_free_bitmap __caca0_free_bitmap

#ifdef __cplusplus
}
#endif

#endif /* __CACA0_H__ */
