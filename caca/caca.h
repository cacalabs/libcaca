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

#ifndef __CACA_H__
#define __CACA_H__

/** libcaca API version */
#define CACA_API_VERSION_1

#include <cucul.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \e libcaca display context */
typedef struct caca_display caca_display_t;
/** \e libcaca event structure */
typedef struct caca_event caca_event_t;

/** \brief Handling of user events.
 *
 *  This structure is filled by caca_get_event() when an event is received.
 *  The \e type field is always valid. */
struct caca_event
{
    /** \brief User event type enumeration.
     *
     *  This enum serves two purposes:
     *  - Build listening masks for caca_get_event().
     *  - Define the type of a \e caca_event_t.
     */
    enum caca_event_type
    {
        CACA_EVENT_NONE =          0x0000, /**< No event. */

        CACA_EVENT_KEY_PRESS =     0x0001, /**< A key was pressed. */
        CACA_EVENT_KEY_RELEASE =   0x0002, /**< A key was released. */
        CACA_EVENT_MOUSE_PRESS =   0x0004, /**< A mouse button was pressed. */
        CACA_EVENT_MOUSE_RELEASE = 0x0008, /**< A mouse button was released. */
        CACA_EVENT_MOUSE_MOTION =  0x0010, /**< The mouse was moved. */
        CACA_EVENT_RESIZE =        0x0020, /**< The window was resized. */
        CACA_EVENT_QUIT =          0x0040, /**< The user requested to quit. */

        CACA_EVENT_ANY =           0xffff  /**< Bitmask for any event. */
    }
    /** \brief User event type member.
     *
     *  This field is always valid when caca_get_event() returns.
     */
    type;

    /** \brief User event data member.
     *
     *  The validity of the \e data union depends on the value of the \e type
     *  field:
     *  - \c CACA_EVENT_NONE: no field is valid.
     *  - \c CACA_EVENT_KEY_PRESS, \c CACA_EVENT_KEY_RELEASE: the
     *  \e data.key.ch field is valid and contains either the ASCII value for
     *  the key, or an \e enum \e caca_key value. If the value is a printable
     *  ASCII character, the \e data.key.utf32 and \e data.key.utf8 fields are
     *  also filled and contain respectively the UTF-32/UCS-4 and the UTF-8
     *  representations of the character. Otherwise, their content is
     *  undefined.
     *  - \c CACA_EVENT_MOUSE_PRESS, \c CACA_EVENT_MOUSE_RELEASE: the
     *  \e data.mouse.button field is valid and contains the index of the
     *  mouse button that was pressed.
     *  - \c CACA_EVENT_MOUSE_MOTION: the \e data.mouse.x and \e data.mouse.y
     *  fields are valid and contain the mouse coordinates in character
     *  cells.
     *  - \c CACA_EVENT_RESIZE: the \e data.resize.w and \e data.resize.h
     *  fields are valid and contain the new width and height values of
     *  the \e libcucul canvas attached to \e libcaca.
     *  - \c CACA_EVENT_QUIT: no other field is valid.
     *
     *  The result of accessing data members outside the above conditions is
     *  undefined.
     */
    union
    {
        struct { unsigned int x, y, button; } mouse;
        struct { unsigned int w, h; } resize;
        struct { unsigned int ch; unsigned long int utf32; char utf8[8]; } key;
    } data;
};

/** \brief Special key values.
 *
 *  Special key values returned by caca_get_event() for which there is no
 *  printable ASCII equivalent.
 */
enum caca_key
{
    CACA_KEY_UNKNOWN = 0x00, /**< Unknown key. */

    /* The following keys have ASCII equivalents */
    CACA_KEY_CTRL_A =    0x01, /**< The Ctrl-A key. */
    CACA_KEY_CTRL_B =    0x02, /**< The Ctrl-B key. */
    CACA_KEY_CTRL_C =    0x03, /**< The Ctrl-C key. */
    CACA_KEY_CTRL_D =    0x04, /**< The Ctrl-D key. */
    CACA_KEY_CTRL_E =    0x05, /**< The Ctrl-E key. */
    CACA_KEY_CTRL_F =    0x06, /**< The Ctrl-F key. */
    CACA_KEY_CTRL_G =    0x07, /**< The Ctrl-G key. */
    CACA_KEY_BACKSPACE = 0x08, /**< The backspace key. */
    CACA_KEY_TAB =       0x09, /**< The tabulation key. */
    CACA_KEY_CTRL_J =    0x0a, /**< The Ctrl-J key. */
    CACA_KEY_CTRL_K =    0x0b, /**< The Ctrl-K key. */
    CACA_KEY_CTRL_L =    0x0c, /**< The Ctrl-L key. */
    CACA_KEY_RETURN =    0x0d, /**< The return key. */
    CACA_KEY_CTRL_N =    0x0e, /**< The Ctrl-N key. */
    CACA_KEY_CTRL_O =    0x0f, /**< The Ctrl-O key. */
    CACA_KEY_CTRL_P =    0x10, /**< The Ctrl-P key. */
    CACA_KEY_CTRL_Q =    0x11, /**< The Ctrl-Q key. */
    CACA_KEY_CTRL_R =    0x12, /**< The Ctrl-R key. */
    CACA_KEY_PAUSE =     0x13, /**< The pause key. */
    CACA_KEY_CTRL_T =    0x14, /**< The Ctrl-T key. */
    CACA_KEY_CTRL_U =    0x15, /**< The Ctrl-U key. */
    CACA_KEY_CTRL_V =    0x16, /**< The Ctrl-V key. */
    CACA_KEY_CTRL_W =    0x17, /**< The Ctrl-W key. */
    CACA_KEY_CTRL_X =    0x18, /**< The Ctrl-X key. */
    CACA_KEY_CTRL_Y =    0x19, /**< The Ctrl-Y key. */
    CACA_KEY_CTRL_Z =    0x1a, /**< The Ctrl-Z key. */
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

/** \defgroup caca libcaca basic functions
 *
 *  These functions provide the basic \e libcaca routines for driver
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
caca_display_t * caca_create_display(cucul_canvas_t *);
int caca_free_display(caca_display_t *);
int caca_refresh_display(caca_display_t *);
int caca_set_display_time(caca_display_t *, unsigned int);
unsigned int caca_get_display_time(caca_display_t *);
unsigned int caca_get_display_width(caca_display_t *);
unsigned int caca_get_display_height(caca_display_t *);
int caca_set_display_title(caca_display_t *, char const *);
/*  @} */

/** \defgroup event libcaca event handling
 *
 *  These functions handle user events such as keyboard input and mouse
 *  clicks.
 *
 *  @{ */
int caca_get_event(caca_display_t *, unsigned int, caca_event_t *, int);
unsigned int caca_get_mouse_x(caca_display_t *);
unsigned int caca_get_mouse_y(caca_display_t *);
int caca_set_mouse(caca_display_t *, int);
int caca_set_cursor(caca_display_t *, int);
/*  @} */

#ifdef __cplusplus
}
#endif

#endif /* __CACA_H__ */
