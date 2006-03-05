/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
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
 *  driver, and an OpenGL driver (through freeglut) that does not require a
 *  text terminal.
 *
 *  \e libcaca is free software, released under the Do What The Fuck You
 *  Want To Public License. This ensures that no one, not even the \e libcaca
 *  developers, will ever have anything to say about what you do with the
 *  software. It used to be licensed under the GNU Lesser General Public
 *  License, but that was not free enough.
 *
 *  \section api The libcaca API
 *
 *  \e libcaca relies on a low-level, device independent library, called
 *  \e libcucul. \e libcucul can be used alone as a simple ASCII and/or
 *  Unicode compositing canvas.
 *
 *  The complete \e libcucul and \e libcaca programming interface is
 *  available from the cucul.h and caca.h headers.
 *
 *  \section env Environment variables
 *
 *  Some environment variables can be used to change the behaviour of
 *  \e libcaca or \e libcucul without having to modify the program which
 *  uses them. These variables are:
 *
 *  \li \b CACA_DRIVER: set the backend video driver. In order of preference:
 *      - \c conio uses the DOS conio.h interface.
 *      - \c ncurses uses the ncurses library.
 *      - \c slang uses the S-Lang library.
 *      - \c x11 uses the native X11 driver.
 *      - \c gl uses freeglut and opengl libraries.
 *      - \c null uses nothing at all, and will display nothing as well.
 *
 *  \li \b CUCUL_BACKGROUND: set the background type.
 *      - \c solid uses solid coloured backgrounds for all characters. This
 *        feature does not work with all terminal emulators. This is the
 *        default choice.
 *      - \c black uses only black backgrounds to render characters.
 *
 *  \li \b CUCUL_ANTIALIASING: set the antialiasing mode. Antialiasing
 *      smoothens the rendered image and avoids the commonly seen staircase
 *      effect.
 *      - \c none disables antialiasing.
 *      - \c prefilter uses a simple prefilter antialiasing method. This is
 *        the default choice.
 *
 *  \li \b CUCUL_DITHERING: set the dithering mode. Dithering is necessary
 *      when rendering a picture that has more colours than the usually
 *      available palette.
 *      - \c none disables dithering.
 *      - \c ordered2 uses a 2x2 Bayer matrix for dithering.
 *      - \c ordered4 uses a 4x4 Bayer matrix for dithering. This is the
 *        default choice.
 *      - \c ordered8 uses a 8x8 Bayer matrix for dithering.
 *      - \c random uses random dithering.
 *
 *  \li \b CACA_GEOMETRY: set the video display size. The format of this
 *      variable must be XxY, with X and Y being integer values. This option
 *      currently only works with the X11 and the GL driver.
 *
 *  \li \b CACA_FONT: set the rendered font. The format of this variable is
 *      implementation dependent, but since it currently only works with the
 *      X11 driver, an X11 font name such as "fixed" or "5x7" is expected.
 */

#ifndef __CACA_H__
#define __CACA_H__

#include <cucul.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \brief User events.
 *
 *  Event types returned by caca_get_event().
 */
enum caca_event
{
    CACA_EVENT_NONE =          0x00000000, /**< No event. */
    CACA_EVENT_KEY_PRESS =     0x01000000, /**< A key was pressed. */
    CACA_EVENT_KEY_RELEASE =   0x02000000, /**< A key was released. */
    CACA_EVENT_MOUSE_PRESS =   0x04000000, /**< A mouse button was pressed. */
    CACA_EVENT_MOUSE_RELEASE = 0x08000000, /**< A mouse button was released. */
    CACA_EVENT_MOUSE_MOTION =  0x10000000, /**< The mouse was moved. */
    CACA_EVENT_RESIZE =        0x20000000, /**< The window was resized. */
    CACA_EVENT_ANY =           0xff000000  /**< Bitmask for any event. */
};

/** \brief Special key values.
 *
 *  Special key values returned by caca_get_event() for which there is no
 *  ASCII equivalent.
 */
enum caca_key
{
    CACA_KEY_UNKNOWN = 0, /**< Unknown key. */

    /* The following keys have ASCII equivalents */
    CACA_KEY_BACKSPACE = 8, /**< The backspace key. */
    CACA_KEY_TAB = 9, /**< The tabulation key. */
    CACA_KEY_RETURN = 13, /**< The return key. */
    CACA_KEY_PAUSE = 19, /**< The pause key. */
    CACA_KEY_ESCAPE = 27, /**< The escape key. */
    CACA_KEY_DELETE = 127, /**< The delete key. */

    /* The following keys do not have ASCII equivalents but have been
     * chosen to match the SDL equivalents */
    CACA_KEY_UP = 273, /**< The up arrow key. */
    CACA_KEY_DOWN = 274, /**< The down arrow key. */
    CACA_KEY_LEFT = 275, /**< The left arrow key. */
    CACA_KEY_RIGHT = 276, /**< The right arrow key. */

    CACA_KEY_INSERT = 277, /**< The insert key. */
    CACA_KEY_HOME = 278, /**< The home key. */
    CACA_KEY_END = 279, /**< The end key. */
    CACA_KEY_PAGEUP = 280, /**< The page up key. */
    CACA_KEY_PAGEDOWN = 281, /**< The page down key. */

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

typedef struct caca_context caca_t;

/** \defgroup basic Basic functions
 *
 *  These functions provide the basic \e libcaca routines for library
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
caca_t * caca_attach(cucul_t *qq);
void caca_detach(caca_t *kk);
void caca_set_delay(caca_t *kk, unsigned int);
void caca_refresh(caca_t *kk);
unsigned int caca_get_rendertime(caca_t *kk);
unsigned int caca_get_window_width(caca_t *kk);
unsigned int caca_get_window_height(caca_t *kk);
void caca_set_size(caca_t *kk, unsigned int width, unsigned int height);
void caca_set_width(caca_t *kk, unsigned int width);
void caca_set_height(caca_t *kk, unsigned int height);
int caca_set_window_title(caca_t *kk, char const *);
/*  @} */

/** \defgroup event Event handling
 *
 *  These functions handle user events such as keyboard input and mouse
 *  clicks.
 *
 *  @{ */
unsigned int caca_get_event(caca_t *kk, unsigned int);
unsigned int caca_wait_event(caca_t *kk, unsigned int);
unsigned int caca_get_mouse_x(caca_t *kk);
unsigned int caca_get_mouse_y(caca_t *kk);
/*  @} */

#ifdef __cplusplus
}
#endif

#endif /* __CACA_H__ */
