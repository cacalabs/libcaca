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

/** \file caca.h
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief The \e libcaca public header.
 *
 *  This header contains the public types and functions that applications
 *  using \e libcaca may use.
 */

/** \mainpage libcaca developer documentation
 *
 *  \section intro Introduction
 *
 *  \e libcaca is a graphics library that outputs text instead of pixels,
 *  so that it can work on older video cards or text terminals. It is not
 *  unlike the famous AAlib library. \e libcaca can use almost any virtual
 *  terminal to work, thus it should work on all Unix systems (including
 *  Mac OS X) using either the slang library or the ncurses library, on DOS
 *  using the conio library, and on Windows systems using either slang or
 *  ncurses (through Cygwin emulation) or conio. There is also a native X11
 *  driver that does not require a text terminal.
 *
 *  \e libcaca is free software, released under the GNU Lesser General
 *  Public License. This ensures that \e libcaca will always remain free
 *  software.
 *
 *  \section api The libcaca API
 *
 *  The complete \e libcaca programming interface is available from the
 *  caca.h header.
 *
 *  \section env Environment variables
 *
 *  Some environment variables can be used to change the behaviour of
 *  \e libcaca without having to modify the program which uses it. These
 *  variables are:
 *
 *  \li \b CACA_DRIVER: set the backend video driver. In order of preference:
 *      - \c conio uses the DOS conio.h interface.
 *      - \c ncurses uses the ncurses library.
 *      - \c slang uses the S-Lang library.
 *      - \c x11 uses the native X11 driver.
 *
 *  \li \b CACA_BACKGROUND: set the background type.
 *      - \c solid uses solid coloured backgrounds for all characters. This
 *        feature does not work with all terminal emulators. This is the
 *        default choice.
 *      - \c black uses only black backgrounds to render characters.
 *
 *  \li \b CACA_ANTIALIASING: set the antialiasing mode. Antialiasing
 *      smoothens the rendered image and avoids the commonly seen staircase
 *      effect.
 *      - \c none disables antialiasing.
 *      - \c prefilter uses a simple prefilter antialiasing method. This is
 *        the default choice.
 *
 *  \li \b CACA_DITHERING: set the dithering mode. Dithering is necessary
 *      when rendering a picture that has more colours than the usually
 *      available palette.
 *      - \c none disables dithering.
 *      - \c ordered2 uses a 2x2 Bayer matrix for dithering.
 *      - \c ordered4 uses a 4x4 Bayer matrix for dithering. This is the
 *        default choice.
 *      - \c ordered8 uses a 8x8 Bayer matrix for dithering.
 *      - \c random uses random dithering.
 */

#ifndef __CACA_H__
#define __CACA_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** \brief Colour definitions.
 *
 *  This enum lists all colours that can be used with caca_set_color().
 */
enum caca_color
{
    CACA_COLOR_BLACK = 0, /**< The colour index for black. */
    CACA_COLOR_BLUE = 1, /**< The colour index for blue. */
    CACA_COLOR_GREEN = 2, /**< The colour index for green. */
    CACA_COLOR_CYAN = 3, /**< The colour index for cyan. */
    CACA_COLOR_RED = 4, /**< The colour index for red. */
    CACA_COLOR_MAGENTA = 5, /**< The colour index for magenta. */
    CACA_COLOR_BROWN = 6, /**< The colour index for brown. */
    CACA_COLOR_LIGHTGRAY = 7, /**< The colour index for light gray. */
    CACA_COLOR_DARKGRAY = 8, /**< The colour index for dark gray. */
    CACA_COLOR_LIGHTBLUE = 9, /**< The colour index for blue. */
    CACA_COLOR_LIGHTGREEN = 10, /**< The colour index for light green. */
    CACA_COLOR_LIGHTCYAN = 11, /**< The colour index for light cyan. */
    CACA_COLOR_LIGHTRED = 12, /**< The colour index for light red. */
    CACA_COLOR_LIGHTMAGENTA = 13, /**< The colour index for light magenta. */
    CACA_COLOR_YELLOW = 14, /**< The colour index for yellow. */
    CACA_COLOR_WHITE = 15 /**< The colour index for white. */
};

/** \brief Internal features.
 *
 *  This enum lists all possible internal libcaca features such as the
 *  rendering method or the dithering mode.
 */
enum caca_feature
{
    CACA_BACKGROUND       = 0x10, /**< Properties of background characters. */
    CACA_BACKGROUND_BLACK = 0x11, /**< Draw only black backgrounds. */
    CACA_BACKGROUND_SOLID = 0x12, /**< Draw coloured solid backgorunds. */
#define CACA_BACKGROUND_MIN 0x11 /**< First background property */
#define CACA_BACKGROUND_MAX 0x12 /**< Last background property */

    CACA_ANTIALIASING           = 0x20, /**< Antialiasing features. */
    CACA_ANTIALIASING_NONE      = 0x21, /**< No antialiasing. */
    CACA_ANTIALIASING_PREFILTER = 0x22, /**< Prefilter antialiasing. */
#define CACA_ANTIALIASING_MIN     0x21 /**< First antialiasing feature. */
#define CACA_ANTIALIASING_MAX     0x22 /**< Last antialiasing feature. */

    CACA_DITHERING          = 0x30, /**< Dithering methods */
    CACA_DITHERING_NONE     = 0x31, /**< No dithering. */
    CACA_DITHERING_ORDERED2 = 0x32, /**< Ordered 2x2 Bayer dithering. */
    CACA_DITHERING_ORDERED4 = 0x33, /**< Ordered 4x4 Bayer dithering. */
    CACA_DITHERING_ORDERED8 = 0x34, /**< Ordered 8x8 Bayer dithering. */
    CACA_DITHERING_RANDOM   = 0x35, /**< Random dithering. */
#define CACA_DITHERING_MIN    0x31 /**< First dithering feature. */
#define CACA_DITHERING_MAX    0x35 /**< Last dithering feature. */

    CACA_UNKNOWN_FEATURE = 0xffff /**< Unknown feature. */
};

/*
 * Backwards compatibility macros
 */
#ifndef _DOXYGEN_SKIP_ME
#define caca_dithering caca_feature
#define caca_set_dithering caca_set_feature
#define caca_get_dithering_name caca_get_feature_name
#define CACA_DITHER_NONE    CACA_DITHERING_NONE
#define CACA_DITHER_ORDERED CACA_DITHERING_ORDERED8
#define CACA_DITHER_RANDOM  CACA_DITHERING_RANDOM
#endif

/** \brief User events.
 *
 *  This enum lists all possible event types returned by caca_get_event().
 */
enum caca_event
{
    CACA_EVENT_NONE =        0x00000000, /**< No event. */
    CACA_EVENT_KEY_PRESS =   0x01000000, /**< A key was pressed. */
    CACA_EVENT_KEY_RELEASE = 0x02000000, /**< A key was released. */
    CACA_EVENT_MOUSE_CLICK = 0x04000000 /**< A mouse button was clicked. */
};

/** \brief Special key values.
 *
 *  This enum lists special key values returned by caca_get_event() for
 *  which there is no ASCII equivalent.
 */
enum caca_key
{
    CACA_KEY_UP = 273, /**< The up arrow key. */
    CACA_KEY_DOWN = 274, /**< The down arrow key. */
    CACA_KEY_LEFT = 275, /**< The left arrow key. */
    CACA_KEY_RIGHT = 276, /**< The right arrow key. */

    CACA_KEY_F1 = 282, /**< The F1 key. */
    CACA_KEY_F2 = 283, /**< The F2 key. */
    CACA_KEY_F3 = 284, /**< The F3 key. */
    CACA_KEY_F4 = 285, /**< The F4 key. */
    CACA_KEY_F5 = 286, /**< The F5 key. */
    CACA_KEY_F6 = 287, /**< The F6 key. */
    CACA_KEY_F7 = 288, /**< The F7 key. */
    CACA_KEY_F8 = 289, /**< The F8 key. */
    CACA_KEY_F9 = 290, /**< The F9 key. */
    CACA_KEY_F10 = 291, /**< The F10 key. */
    CACA_KEY_F11 = 292, /**< The F11 key. */
    CACA_KEY_F12 = 293, /**< The F12 key. */
    CACA_KEY_F13 = 294, /**< The F13 key. */
    CACA_KEY_F14 = 295, /**< The F14 key. */
    CACA_KEY_F15 = 296 /**< The F15 key. */
};

/** \defgroup basic Basic functions
 *
 *  These functions provide the basic \e libcaca routines for library
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
int caca_init(void);
void caca_set_delay(unsigned int);
enum caca_feature caca_get_feature(enum caca_feature);
void caca_set_feature(enum caca_feature);
const char *caca_get_feature_name(enum caca_feature);
unsigned int caca_get_rendertime(void);
unsigned int caca_get_width(void);
unsigned int caca_get_height(void);
void caca_refresh(void);
void caca_end(void);
/*  @} */

/** \defgroup event Event handling functions
 *
 *  These functions handle user events such as keyboard input and mouse
 *  clicks.
 *
 *  @{ */
unsigned int caca_get_event(void);
unsigned int caca_wait_event(void);
/*  @} */

/** \defgroup char Character printing functions
 *
 *  These functions provide low-level character printing routines.
 *
 *  @{ */
void caca_set_color(enum caca_color, enum caca_color);
enum caca_color caca_get_fg_color(void);
enum caca_color caca_get_bg_color(void);
const char *caca_get_color_name(enum caca_color);
void caca_putchar(int, int, char);
void caca_putstr(int, int, const char *);
void caca_printf(int, int, const char *, ...);
void caca_clear(void);
/*  @} */

/** \defgroup prim Primitives drawing functions
 *
 *  These functions provide routines for primitive drawing, such as lines,
 *  boxes, triangles and ellipses.
 *
 *  @{ */
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
/*  @} */

/** \defgroup math Mathematical functions
 *
 *  These functions provide a few useful math-related routines.
 *
 *  @{ */
int caca_rand(int, int);
unsigned int caca_sqrt(unsigned int);
/*  @} */

/** \defgroup sprite Sprite handling functions
 *
 *  These functions provide high level routines for sprite loading, animation
 *  and rendering.
 *
 *  @{ */
struct caca_sprite;
struct caca_sprite * caca_load_sprite(const char *);
int caca_get_sprite_frames(const struct caca_sprite *);
int caca_get_sprite_width(const struct caca_sprite *, int);
int caca_get_sprite_height(const struct caca_sprite *, int);
int caca_get_sprite_dx(const struct caca_sprite *, int);
int caca_get_sprite_dy(const struct caca_sprite *, int);
void caca_draw_sprite(int, int, const struct caca_sprite *, int);
void caca_free_sprite(struct caca_sprite *);
/*  @} */

/** \defgroup bitmap Bitmap handling functions
 *
 *  These functions provide high level routines for bitmap allocation and
 *  rendering.
 *
 *  @{ */
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
/*  @} */

#ifdef __cplusplus
}
#endif

#endif /* __CACA_H__ */
