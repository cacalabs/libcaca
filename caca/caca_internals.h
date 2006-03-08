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

/** \file caca_internals.h
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief The \e libcaca private header.
 *
 *  This header contains the private types and functions used by \e libcaca.
 */

#ifndef __CACA_INTERNALS_H__
#define __CACA_INTERNALS_H__

#if defined(HAVE_INTTYPES_H) || defined(_DOXYGEN_SKIP_ME)
#   include <inttypes.h>
#elif !defined(CUSTOM_INTTYPES)
#   define CUSTOM_INTTYPES
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#if !defined(_DOXYGEN_SKIP_ME)
#   define EVENTBUF_LEN 10
#endif

/* Graphics driver */
enum caca_driver
{
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
    CACA_DRIVER_NONE = 0
};

/* Available drivers */
#if defined(USE_CONIO)
void conio_init_driver(caca_t *);
#endif
#if defined(USE_GL)
void gl_init_driver(caca_t *);
#endif
#if defined(USE_NCURSES)
void ncurses_init_driver(caca_t *);
#endif
#if defined(USE_SLANG)
void slang_init_driver(caca_t *);
#endif
#if defined(USE_WIN32)
void win32_init_driver(caca_t *);
#endif
#if defined(USE_X11)
void x11_init_driver(caca_t *);
#endif

/* Timer structure */
struct caca_timer
{
    int last_sec, last_usec;
};

/* Internal caca context */
struct caca_context
{
    cucul_t *qq;

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
        void (* handle_resize) (caca_t *, unsigned int *, unsigned int *);
        unsigned int (* get_event) (caca_t *);
    } drv;

    //unsigned int width, height;
    unsigned int mouse_x, mouse_y;

    int resize;
    int resize_event;

    unsigned int delay, rendertime;
    struct caca_timer timer;
    int lastticks;

    struct events
    {
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO)
        unsigned int buf[EVENTBUF_LEN];
        int queue;
#endif
#if defined(USE_SLANG) || defined(USE_NCURSES)
        struct caca_timer key_timer;
        unsigned int last_key_ticks;
        unsigned int autorepeat_ticks;
        unsigned int last_key;
#endif
    } events;
};

/* Timer functions */
extern void _caca_sleep(unsigned int);
extern unsigned int _caca_getticks(struct caca_timer *);

#endif /* __CACA_INTERNALS_H__ */