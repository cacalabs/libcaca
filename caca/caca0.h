/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This header contains glue code for applications using the pre-1.0
 *  libcaca API.
 */

#ifndef __CACA0_H__
#define __CACA0_H__

#include <caca.h>

#undef __extern
#if defined _DOXYGEN_SKIP_ME
#elif defined _WIN32 && defined __LIBCACA__ && defined DLL_EXPORT
#   define __extern extern __declspec(dllexport)
#elif defined _WIN32 && !defined __LIBCACA__
#   define __extern extern __declspec(dllimport)
#else
#   define __extern extern
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* Function emulation */
__extern int __caca0_init(void);
__extern void __caca0_end(void);
__extern unsigned int __caca0_get_event(unsigned int, int);
__extern unsigned int __caca0_sqrt(unsigned int);
__extern int __caca0_get_feature(int);
__extern void __caca0_set_feature(int);
__extern char const *__caca0_get_feature_name(int);
__extern caca_canvas_t *__caca0_load_sprite(char const *);
__extern caca_dither_t *__caca0_create_bitmap(unsigned int, unsigned int,
            unsigned int, unsigned int, unsigned long int, unsigned long int,
            unsigned long int, unsigned long int);
__extern void __caca0_free_bitmap(caca_dither_t *);
__extern char const *__caca0_get_color_name(unsigned char);

/* These variables are needed to emulate old non-thread safe behaviour */
__extern caca_canvas_t *__caca0_cv;
__extern caca_display_t *__caca0_dp;
__extern unsigned char __caca0_fg;
__extern unsigned char __caca0_bg;

/* This enum still exists in libcaca 1.x, thus cannot be redefined */
#define CACA_COLOR_BLACK        CACA_BLACK
#define CACA_COLOR_BLUE         CACA_BLUE
#define CACA_COLOR_GREEN        CACA_GREEN
#define CACA_COLOR_CYAN         CACA_CYAN
#define CACA_COLOR_RED          CACA_RED
#define CACA_COLOR_MAGENTA      CACA_MAGENTA
#define CACA_COLOR_BROWN        CACA_BROWN
#define CACA_COLOR_LIGHTGRAY    CACA_LIGHTGRAY
#define CACA_COLOR_DARKGRAY     CACA_DARKGRAY
#define CACA_COLOR_LIGHTBLUE    CACA_LIGHTBLUE
#define CACA_COLOR_LIGHTGREEN   CACA_LIGHTGREEN
#define CACA_COLOR_LIGHTCYAN    CACA_LIGHTCYAN
#define CACA_COLOR_LIGHTRED     CACA_LIGHTRED
#define CACA_COLOR_LIGHTMAGENTA CACA_LIGHTMAGENTA
#define CACA_COLOR_YELLOW       CACA_YELLOW
#define CACA_COLOR_WHITE        CACA_WHITE

/* These enums and macros changed names or values */
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

#if ! defined __LIBCACA__

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
#define caca_get_width() caca_get_canvas_width(__caca0_cv)
#define caca_get_height() caca_get_canvas_height(__caca0_cv)
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
    (__caca0_fg = (x), __caca0_bg = (y), caca_set_color_ansi(__caca0_cv, x, y))
#define caca_get_fg_color() __caca0_fg
#define caca_get_bg_color() __caca0_bg
#define caca_get_color_name __caca0_get_color_name
#define caca_putchar(x, y, c) caca_put_char(__caca0_cv, x, y, c)
#define caca_putstr(x, y, s) caca_put_str(__caca0_cv, x, y, s)
#define caca_printf(x, y, f, z...) caca_printf(__caca0_cv, x, y, f, ##z)
#define caca_clear() caca_clear_canvas(__caca0_cv)

#define caca_draw_line(x, y, z, t, c) \
    caca_draw_line(__caca0_cv, x, y, z, t, c)
#define caca_draw_polyline(x, y, z, c) \
    caca_draw_polyline(__caca0_cv, x, y, z, c)
#define caca_draw_thin_line(x, y, z, t) \
    caca_draw_thin_line(__caca0_cv, x, y, z, t)
#define caca_draw_thin_polyline(x, y, z) \
    caca_draw_thin_polyline(__caca0_cv, x, y, z)
#define caca_draw_circle(x, y, z, c) \
    caca_draw_circle(__caca0_cv, x, y, z, c)
#define caca_draw_ellipse(x, y, z, t, c) \
    caca_draw_ellipse(__caca0_cv, x, y, z, t, c)
#define caca_draw_thin_ellipse(x, y, z, t) \
    caca_draw_thin_ellipse(__caca0_cv, x, y, z, t)
#define caca_fill_ellipse(x, y, z, t, c) \
    caca_fill_ellipse(__caca0_cv, x, y, z, t, c)
#define caca_draw_box(x, y, z, t, c) \
    caca_draw_box(__caca0_cv, x, y, z, t, c)
#define caca_draw_thin_box(x, y, z, t) \
    caca_draw_thin_box(__caca0_cv, x, y, z, t)
#define caca_fill_box(x, y, z, t, c) \
    caca_fill_box(__caca0_cv, x, y, z, t, c)
#define caca_draw_triangle(x, y, z, t, u, v, c) \
    caca_draw_triangle(__caca0_cv, x, y, z, t, u, v, c)
#define caca_draw_thin_triangle(x, y, z, t, u, v) \
    caca_draw_thin_triangle(__caca0_cv, x, y, z, t, u, v)
#define caca_fill_triangle(x, y, z, t, u, v, c) \
    caca_fill_triangle(__caca0_cv, x, y, z, t, u, v, c)

#define caca_rand(a, b) caca_rand(a, (b)+1)
#define caca_sqrt __caca0_sqrt

#define caca_sprite caca_canvas
#define caca_load_sprite __caca0_load_sprite
#define caca_get_sprite_frames(c) 1
#define caca_get_sprite_width(c, f) caca_get_canvas_width(c)
#define caca_get_sprite_height(c, f) caca_get_canvas_height(c)
#define caca_get_sprite_dx(c, f) 0
#define caca_get_sprite_dx(c, f) 0
#define caca_draw_sprite(x, y, c, f) caca_blit(__caca0_cv, x, y, c, NULL)
#define caca_free_sprite caca_free_canvas

#define caca_bitmap caca_dither
#define caca_create_bitmap __caca0_create_bitmap
#define caca_set_bitmap_palette caca_set_dither_palette
#define caca_draw_bitmap(x, y, z, t, b, p) \
    caca_dither_bitmap(__caca0_cv, x, y, z, t, b, p)
#define caca_free_bitmap __caca0_free_bitmap

#endif /* ! defined __LIBCACA__ */

#ifdef __cplusplus
}
#endif

#undef __extern

#endif /* __CACA0_H__ */
