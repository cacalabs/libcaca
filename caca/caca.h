/*
 *  libcaca       Colour ASCII-Art library
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
 *  text terminal. For machines without a screen, the raw driver can be used
 *  to send the output to another machine, using for instance cacaserver.
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
 *  \e libcaca without having to modify the program which uses them. These
 *  variables are:
 *
 *  \li \b CACA_DRIVER: set the backend video driver. In order of preference:
 *      - \c conio uses the DOS conio.h interface.
 *      - \c ncurses uses the ncurses library.
 *      - \c slang uses the S-Lang library.
 *      - \c x11 uses the native X11 driver.
 *      - \c gl uses freeglut and opengl libraries.
 *      - \c raw outputs to the standard output instead of rendering the
 *        canvas. This is can be used together with cacaserver.
 *
 *  \li \b CACA_GEOMETRY: set the video display size. The format of this
 *      variable must be XxY, with X and Y being integer values. This option
 *      currently works with the network, X11 and GL drivers.
 *
 *  \li \b CACA_FONT: set the rendered font. The format of this variable is
 *      implementation dependent, but since it currently only works with the
 *      X11 driver, an X11 font name such as "fixed" or "5x7" is expected.
 */

#ifndef __CACA_H__
#define __CACA_H__

/** libcaca API version */
#define CACA_API_VERSION_1

#include <cucul.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \e libcaca context */
typedef struct caca caca_t;
/** event structure */
typedef struct caca_event caca_event_t;

/** \brief User events.
 *
 *  This structure is filled by caca_get_event() when an event is received.
 *  The \e type field is always valid. The validity of the \e data union
 *  depends on the value of the \e type field:
 *
 *  \li \b CACA_EVENT_NONE: no other field is valid.
 *
 *  \li \b CACA_EVENT_KEY_PRESS, \b CACA_EVENT_KEY_RELEASE: the \e data.key.c
 *      field is valid and contains either the ASCII value for the key, or
 *      an \e enum \e caca_key value. If the value is a printable ASCII
 *      character, the \e data.key.ucs4 and \e data.key.utf8 fields are
 *      also filled and contain respectively the UCS-4/UTF-32 and the UTF-8
 *      representations of the character. Otherwise, their content is
 *      undefined.
 *
 *  \li \b CACA_EVENT_MOUSE_PRESS, \b CACA_EVENT_MOUSE_RELEASE: the
 *      \e data.mouse.button field is valid and contains the index of the
 *      mouse button that was pressed.
 *
 *  \li \b CACA_EVENT_MOUSE_MOTION: the \e data.mouse.x and \e data.mouse.y
 *      fields are valid and contain the mouse coordinates in character
 *      cells.
 *
 *  \li \b CACA_EVENT_RESIZE: the \e data.resize.w and \e data.resize.h
 *      fields are valid and contain the new width and height values of
 *      the \e libcucul canvas attached to \e libcaca.
 *
 *  The result of accessing data members outside the above conditions is
 *  undefined.
 */
struct caca_event
{
    enum caca_event_type
    {
        CACA_EVENT_NONE =          0x0000, /**< No event. */

        CACA_EVENT_KEY_PRESS =     0x0001, /**< A key was pressed. */
        CACA_EVENT_KEY_RELEASE =   0x0002, /**< A key was released. */
        CACA_EVENT_MOUSE_PRESS =   0x0004, /**< A mouse button was pressed. */
        CACA_EVENT_MOUSE_RELEASE = 0x0008, /**< A mouse button was released. */
        CACA_EVENT_MOUSE_MOTION =  0x0010, /**< The mouse was moved. */
        CACA_EVENT_RESIZE =        0x0020, /**< The window was resized. */

        CACA_EVENT_ANY =           0xffff  /**< Bitmask for any event. */
    } type;

    union
    {
        struct { unsigned int x, y, button; } mouse;
        struct { unsigned int w, h; } resize;
        struct { unsigned int c; unsigned long int ucs4; char utf8[8]; } key;
    } data;
};

/** \brief Special key values.
 *
 *  Special key values returned by caca_get_event() for which there is no
 *  ASCII equivalent.
 */
enum caca_key
{
    CACA_KEY_UNKNOWN = 0x00, /**< Unknown key. */

    /* The following keys have ASCII equivalents */
    CACA_KEY_BACKSPACE = 0x08, /**< The backspace key. */
    CACA_KEY_TAB =       0x09, /**< The tabulation key. */
    CACA_KEY_RETURN =    0x0d, /**< The return key. */
    CACA_KEY_PAUSE =     0x13, /**< The pause key. */
    CACA_KEY_ESCAPE =    0x1b, /**< The escape key. */
    CACA_KEY_DELETE =    0x7f, /**< The delete key. */

    /* The following keys do not have ASCII equivalents but have been
     * chosen to match the SDL equivalents */
    CACA_KEY_UP =    0x111, /**< The up arrow key. */
    CACA_KEY_DOWN =  0x112, /**< The down arrow key. */
    CACA_KEY_LEFT =  0x113, /**< The left arrow key. */
    CACA_KEY_RIGHT = 0x114, /**< The right arrow key. */

    CACA_KEY_INSERT =   0x115, /**< The insert key. */
    CACA_KEY_HOME =     0x116, /**< The home key. */
    CACA_KEY_END =      0x117, /**< The end key. */
    CACA_KEY_PAGEUP =   0x118, /**< The page up key. */
    CACA_KEY_PAGEDOWN = 0x119, /**< The page down key. */

    CACA_KEY_F1 =  0x11a, /**< The F1 key. */
    CACA_KEY_F2 =  0x11b, /**< The F2 key. */
    CACA_KEY_F3 =  0x11c, /**< The F3 key. */
    CACA_KEY_F4 =  0x11d, /**< The F4 key. */
    CACA_KEY_F5 =  0x11e, /**< The F5 key. */
    CACA_KEY_F6 =  0x11f, /**< The F6 key. */
    CACA_KEY_F7 =  0x120, /**< The F7 key. */
    CACA_KEY_F8 =  0x121, /**< The F8 key. */
    CACA_KEY_F9 =  0x122, /**< The F9 key. */
    CACA_KEY_F10 = 0x123, /**< The F10 key. */
    CACA_KEY_F11 = 0x124, /**< The F11 key. */
    CACA_KEY_F12 = 0x125, /**< The F12 key. */
    CACA_KEY_F13 = 0x126, /**< The F13 key. */
    CACA_KEY_F14 = 0x127, /**< The F14 key. */
    CACA_KEY_F15 = 0x128  /**< The F15 key. */
};

/** \defgroup caca Basic libcaca functions
 *
 *  These functions provide the basic \e libcaca routines for driver
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
caca_t * caca_attach(cucul_t *qq);
void caca_detach(caca_t *kk);
void caca_set_delay(caca_t *kk, unsigned int);
void caca_display(caca_t *kk);
unsigned int caca_get_rendertime(caca_t *kk);
unsigned int caca_get_window_width(caca_t *kk);
unsigned int caca_get_window_height(caca_t *kk);
int caca_set_window_title(caca_t *kk, char const *);
/*  @} */

/** \defgroup event Event handling
 *
 *  These functions handle user events such as keyboard input and mouse
 *  clicks.
 *
 *  @{ */
int caca_get_event(caca_t *kk, unsigned int, caca_event_t *, int);
unsigned int caca_get_mouse_x(caca_t *kk);
unsigned int caca_get_mouse_y(caca_t *kk);
void caca_set_mouse(caca_t *kk, int);
/*  @} */

#ifdef __cplusplus
}
#endif

#endif /* __CACA_H__ */
