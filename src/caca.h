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

/**  \file caca.h
 *   \version \$Id$
 *   \author Sam Hocevar <sam@zoy.org>
 *   \brief The \e libcaca public header.
 *
 *   This header contains the public types and functions that applications
 *   using \e libcaca may use.
 */

/**  \mainpage libcaca developer documentation
 *
 *   \section intro Introduction
 *
 *   \e libcaca is a graphics library that outputs text instead of pixels,
 *   so that it can work on older video cards or text terminals. It is not
 *   unlike the famous AAlib library. libcaca needs a terminal to work,
 *   thus it should work on all Unix systems (including Mac OS X) using
 *   either the slang library or the ncurses library, on DOS using the conio
 *   library, and on Windows systems using either slang or ncurses (through
 *   Cygwin emulation) or conio.
 *
 *   \e libcaca is free software, released under the GNU Lesser General
 *   Public License. This ensures that \e libcaca will always remain free
 *   software.
 *
 *   \section api The libcaca API
 *
 *   The complete \e libcaca programming interface is available from the
 *   caca.h file.
 *
 *   \section env Environment variables
 *
 *   Some environment variables can be used to change the behaviour of
 *   \e libcaca without having to modify the program which uses it. These
 *   variables are:
 *
 *   \li \b CACA_DRIVER: set the backend video driver. In order of preference:
 *       - \c conio uses the DOS conio.h interface.
 *       - \c ncurses uses the ncurses library.
 *       - \c slang uses the S-Lang library.
 *       - \c x11 uses the native X11 driver.
 *
 *   \li \b CACA_BACKGROUND: set the background type.
 *       - \c solid uses solid coloured backgrounds for all characters. This
 *         feature does not work with all terminal emulators. This is the
 *         default choice.
 *       - \c black uses only black backgrounds to render characters.
 *
 *   \li \b CACA_ANTIALIASING: set the antialiasing mode. Antialiasing
 *       smoothens the rendered image and avoids the commonly seen staircase
 *       effect.
 *       - \c none disables antialiasing.
 *       - \c prefilter uses a simple prefilter antialiasing method. This is
 *         the default choice.
 *
 *   \li \b CACA_DITHERING: set the dithering mode. Dithering is necessary
 *       when rendering a picture that has more colours than the usually
 *       available palette.
 *       - \c none disables dithering.
 *       - \c ordered2 uses a 2x2 bayer matrix for dithering.
 *       - \c ordered4 uses a 4x4 bayer matrix for dithering. This is the
 *         default choice.
 *       - \c ordered8 uses a 8x8 bayer matrix for dithering.
 *       - \c random uses random dithering.
 */

#ifndef __CACA_H__
#define __CACA_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * The colour definitions to be used with caca_set_color().
 */
enum caca_color
{
    CACA_COLOR_BLACK = 0,
    CACA_COLOR_BLUE = 1,
    CACA_COLOR_GREEN = 2,
    CACA_COLOR_CYAN = 3,
    CACA_COLOR_RED = 4,
    CACA_COLOR_MAGENTA = 5,
    CACA_COLOR_BROWN = 6,
    CACA_COLOR_LIGHTGRAY = 7,
    CACA_COLOR_DARKGRAY = 8,
    CACA_COLOR_LIGHTBLUE = 9,
    CACA_COLOR_LIGHTGREEN = 10,
    CACA_COLOR_LIGHTCYAN = 11,
    CACA_COLOR_LIGHTRED = 12,
    CACA_COLOR_LIGHTMAGENTA = 13,
    CACA_COLOR_YELLOW = 14,
    CACA_COLOR_WHITE = 15
};

const char *caca_get_color_name(enum caca_color);

/**
 * The internal libcaca features.
 */
enum caca_feature
{
    /* Properties of background characters */
    CACA_BACKGROUND       = 0x10,
    CACA_BACKGROUND_BLACK = 0x11,
    CACA_BACKGROUND_SOLID = 0x12,
#define CACA_BACKGROUND_MIN 0x11
#define CACA_BACKGROUND_MAX 0x12

    /* Antialiasing features */
    CACA_ANTIALIASING           = 0x20,
    CACA_ANTIALIASING_NONE      = 0x21,
    CACA_ANTIALIASING_PREFILTER = 0x22,
#define CACA_ANTIALIASING_MIN     0x21
#define CACA_ANTIALIASING_MAX     0x22

    /* Dithering methods */
    CACA_DITHERING          = 0x30,
    CACA_DITHERING_NONE     = 0x31,
    CACA_DITHERING_ORDERED2 = 0x32,
    CACA_DITHERING_ORDERED4 = 0x33,
    CACA_DITHERING_ORDERED8 = 0x34,
    CACA_DITHERING_RANDOM   = 0x35,
#define CACA_DITHERING_MIN    0x31
#define CACA_DITHERING_MAX    0x35

    /* Unknown feature */
    CACA_UNKNOWN_FEATURE = 0xffff
};

const char *caca_get_feature_name(enum caca_feature);

/* Backwards compatibility */
#define caca_dithering caca_feature
#define caca_set_dithering caca_set_feature
#define caca_get_dithering_name caca_get_feature_name
#define CACA_DITHER_NONE    CACA_DITHERING_NONE
#define CACA_DITHER_ORDERED CACA_DITHERING_ORDERED8
#define CACA_DITHER_RANDOM  CACA_DITHERING_RANDOM

/**
 * The event types returned by caca_get_event().
 */
enum caca_event
{
    CACA_EVENT_NONE =        0x00000000,
    CACA_EVENT_KEY_PRESS =   0x01000000,
    CACA_EVENT_KEY_RELEASE = 0x02000000,
    CACA_EVENT_MOUSE_CLICK = 0x04000000
};

/**
 * The special key values returned by caca_get_event().
 */
enum caca_key
{
    CACA_KEY_UP = 273,
    CACA_KEY_DOWN = 274,
    CACA_KEY_LEFT = 275,
    CACA_KEY_RIGHT = 276,

    CACA_KEY_F1 = 282,
    CACA_KEY_F2 = 283,
    CACA_KEY_F3 = 284,
    CACA_KEY_F4 = 285,
    CACA_KEY_F5 = 286,
    CACA_KEY_F6 = 287,
    CACA_KEY_F7 = 288,
    CACA_KEY_F8 = 289,
    CACA_KEY_F9 = 290,
    CACA_KEY_F10 = 291,
    CACA_KEY_F11 = 292,
    CACA_KEY_F12 = 293,
    CACA_KEY_F13 = 294,
    CACA_KEY_F14 = 295,
    CACA_KEY_F15 = 296
};

/*
 * Basic functions
 */
int caca_init(void);
void caca_set_delay(unsigned int);
enum caca_feature caca_get_feature(enum caca_feature);
void caca_set_feature(enum caca_feature);
unsigned int caca_get_rendertime(void);
unsigned int caca_get_width(void);
unsigned int caca_get_height(void);
void caca_refresh(void);
void caca_end(void);

/*
 * Events
 */
unsigned int caca_get_event(void);

/*
 * Character graphics
 */
void caca_set_color(enum caca_color, enum caca_color);
enum caca_color caca_get_fg_color(void);
enum caca_color caca_get_bg_color(void);
void caca_putchar(int, int, char);
void caca_putstr(int, int, const char *);
void caca_printf(int, int, const char *, ...);
void caca_clear(void);

/*
 * Graphics primitives
 */
void caca_draw_line(int, int, int, int, char);
void caca_draw_polyline(const int x[], const int y[], int, char);
void caca_draw_thin_line(int, int, int, int);
void caca_draw_thin_polyline(const int x[], const int y[], int);

void caca_draw_circle(int, int, int, char);
void caca_draw_ellipse(int, int, int, int, char);
void caca_draw_thin_ellipse(int, int, int, int);
void caca_fill_ellipse(int, int, int, int, char);

void caca_draw_box(int, int, int, int, char);
void caca_draw_thin_box(int, int, int, int);
void caca_fill_box(int, int, int, int, char);

void caca_draw_triangle(int, int, int, int, int, int, char);
void caca_draw_thin_triangle(int, int, int, int, int, int);
void caca_fill_triangle(int, int, int, int, int, int, char);

/*
 * Maths
 */
int caca_rand(int, int);
unsigned int caca_sqrt(unsigned int);

/*
 * Sprite handling
 */
struct caca_sprite;
struct caca_sprite * caca_load_sprite(const char *);
int caca_get_sprite_frames(const struct caca_sprite *);
int caca_get_sprite_width(const struct caca_sprite *, int);
int caca_get_sprite_height(const struct caca_sprite *, int);
int caca_get_sprite_dx(const struct caca_sprite *, int);
int caca_get_sprite_dy(const struct caca_sprite *, int);
void caca_draw_sprite(int, int, const struct caca_sprite *, int);
void caca_free_sprite(struct caca_sprite *);

/*
 * Bitmap handling
 */
struct caca_bitmap;
struct caca_bitmap *caca_create_bitmap(unsigned int, unsigned int,
                                       unsigned int, unsigned int,
                                       unsigned int, unsigned int,
                                       unsigned int, unsigned int);
void caca_set_bitmap_palette(struct caca_bitmap *,
                             unsigned int r[], unsigned int g[],
                             unsigned int b[], unsigned int a[]);
void caca_draw_bitmap(int, int, int, int, const struct caca_bitmap *, void *);
void caca_free_bitmap(struct caca_bitmap *);

#ifdef __cplusplus
}
#endif

#endif /* __CACA_H__ */
