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

#include <cucul.h>

#undef __extern
#if defined(_DOXYGEN_SKIP_ME)
#elif defined(_WIN32) && defined(__LIBCACA__)
#   define __extern extern __declspec(dllexport)
#else
#   define __extern extern
#endif

/** libcaca API version */
#define CACA_API_VERSION_1

#ifdef __cplusplus
extern "C"
{
#endif

/** \e libcaca display context */
typedef struct caca_display caca_display_t;
/** \e libcaca event structure */
typedef struct caca_event caca_event_t;

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
};

/** \brief Handling of user events.
 *
 *  This structure is filled by caca_get_event() when an event is received.
 *  It is an opaque structure that should only be accessed through
 *  caca_event_get_type() and similar functions. The struct members may no
 *  longer be directly accessible in future versions.
 */
struct caca_event
{
    enum caca_event_type type;
    union
    {
        struct { int x, y, button; } mouse;
        struct { int w, h; } resize;
        struct { int ch; uint32_t utf32; char utf8[8]; } key;
    } data;
    uint8_t padding[16];
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

/** \defgroup libcaca libcaca basic functions
 *
 *  These functions provide the basic \e libcaca routines for driver
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
__extern caca_display_t * caca_create_display(cucul_canvas_t *);
__extern caca_display_t * caca_create_display_with_driver(cucul_canvas_t *,
                                                          char const *);
__extern char const * const * caca_get_display_driver_list(void);
__extern char const * caca_get_display_driver(caca_display_t *);
__extern int caca_set_display_driver(caca_display_t *, char const *);
__extern int caca_free_display(caca_display_t *);
__extern cucul_canvas_t * caca_get_canvas(caca_display_t *);
__extern int caca_refresh_display(caca_display_t *);
__extern int caca_set_display_time(caca_display_t *, int);
__extern int caca_get_display_time(caca_display_t const *);
__extern int caca_get_display_width(caca_display_t const *);
__extern int caca_get_display_height(caca_display_t const *);
__extern int caca_set_display_title(caca_display_t *, char const *);
__extern int caca_set_mouse(caca_display_t *, int);
__extern int caca_set_cursor(caca_display_t *, int);
__extern char const * caca_get_version(void);
/*  @} */

/** \defgroup caca_event libcaca event handling
 *
 *  These functions handle user events such as keyboard input and mouse
 *  clicks.
 *
 *  @{ */
__extern int caca_get_event(caca_display_t *, int, caca_event_t *, int);
__extern int caca_get_mouse_x(caca_display_t const *);
__extern int caca_get_mouse_y(caca_display_t const *);
__extern enum caca_event_type caca_get_event_type(caca_event_t const *);
__extern int caca_get_event_key_ch(caca_event_t const *);
__extern uint32_t caca_get_event_key_utf32(caca_event_t const *);
__extern int caca_get_event_key_utf8(caca_event_t const *, char *);
__extern int caca_get_event_mouse_button(caca_event_t const *);
__extern int caca_get_event_mouse_x(caca_event_t const *);
__extern int caca_get_event_mouse_y(caca_event_t const *);
__extern int caca_get_event_resize_width(caca_event_t const *);
__extern int caca_get_event_resize_height(caca_event_t const *);
/*  @} */

#ifdef __cplusplus
}
#endif

#undef __extern

#endif /* __CACA_H__ */
