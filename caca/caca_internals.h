/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/** \file caca_internals.h
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief The \e libcaca private header.
 *
 *  This header contains the private types and functions used by \e libcaca.
 */

#ifndef __CACA_INTERNALS_H__
#define __CACA_INTERNALS_H__

#if defined(HAVE_INTTYPES_H) && !defined(__KERNEL__)
#   include <inttypes.h>
#elif !defined(CUSTOM_INTTYPES) && !defined(_DOXYGEN_SKIP_ME)
#   define CUSTOM_INTTYPES
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long int uint32_t;
typedef long int intptr_t;
typedef long unsigned int uintptr_t;
#endif

#if !defined(_DOXYGEN_SKIP_ME)
#   define EVENTBUF_LEN 10
#endif

/* Graphics driver */
enum caca_driver
{
    CACA_DRIVER_NONE = 0,
#if defined(USE_CONIO)
    CACA_DRIVER_CONIO = 1,
#endif
#if defined(USE_NCURSES)
    CACA_DRIVER_NCURSES = 2,
#endif
#if defined(USE_SLANG)
    CACA_DRIVER_SLANG = 3,
#endif
#if defined(USE_X11)
    CACA_DRIVER_X11 = 4,
#endif
#if defined(USE_WIN32)
    CACA_DRIVER_WIN32 = 5,
#endif
#if defined(USE_GL)
    CACA_DRIVER_GL = 6,
#endif
#if defined(USE_NETWORK)
    CACA_DRIVER_NETWORK = 7,
#endif
#if defined(USE_VGA)
    CACA_DRIVER_VGA = 8,
#endif
};

/* Available drivers */
#if defined(USE_CONIO)
int conio_install(caca_t *);
#endif
#if defined(USE_GL)
int gl_install(caca_t *);
#endif
#if defined(USE_NCURSES)
int ncurses_install(caca_t *);
#endif
#if defined(USE_SLANG)
int slang_install(caca_t *);
#endif
#if defined(USE_WIN32)
int win32_install(caca_t *);
#endif
#if defined(USE_X11)
int x11_install(caca_t *);
#endif
#if defined(USE_NETWORK)
int network_install(caca_t *);
#endif
#if defined(USE_VGA)
int vga_install(caca_t *);
#endif

/* Timer structure */
struct caca_timer
{
    int last_sec, last_usec;
};

/* Internal caca context */
struct caca_context
{
    /* A link to our cucul canvas */
    cucul_t *qq;

    /* Device-specific functions */
    struct drv
    {
        enum caca_driver driver;
        struct driver_private *p;

        int (* init_graphics) (caca_t *);
        int (* end_graphics) (caca_t *);
        int (* set_window_title) (caca_t *, char const *);
        unsigned int (* get_window_width) (caca_t *);
        unsigned int (* get_window_height) (caca_t *);
        void (* display) (caca_t *);
        void (* handle_resize) (caca_t *);
        int (* get_event) (caca_t *, struct caca_event *);
    } drv;

    /* Mouse position */
    struct mouse
    {
        unsigned int x, y;
    } mouse;

    /* Window resize handling */
    struct resize
    {
        int resized;   /* A resize event was requested */
        unsigned w, h; /* Requested width and height */
    } resize;

    /* Framerate handling */
    unsigned int delay, rendertime;
    struct caca_timer timer;
    int lastticks;

    struct events
    {
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
        struct caca_event buf[EVENTBUF_LEN];
        int queue;
#endif
#if defined(USE_SLANG) || defined(USE_NCURSES)
        struct caca_timer key_timer;
        unsigned int last_key_ticks;
        unsigned int autorepeat_ticks;
        struct caca_event last_key_event;
#endif
    } events;
};

/* Internal timer functions */
extern void _caca_sleep(unsigned int);
extern unsigned int _caca_getticks(struct caca_timer *);

/* Internal event functions */
extern void _caca_handle_resize(caca_t *);
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
extern void _push_event(caca_t *, struct caca_event *);
extern int _pop_event(caca_t *, struct caca_event *);
#endif

#endif /* __CACA_INTERNALS_H__ */
