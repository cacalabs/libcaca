/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
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

#if defined(USE_X11)
#include <X11/Xlib.h>
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

/* Timer structure */
#define CACA_TIMER_INITIALIZER { 0, 0 }
struct caca_timer
{
    int last_sec, last_usec;
};

/* Internal caca context */
struct caca_context
{
    cucul_t *qq;

    enum caca_driver driver;
    unsigned int width, height;

    int resize;
    int resize_event;

    unsigned int delay, rendertime;

#if defined(USE_X11) && !defined(_DOXYGEN_SKIP_ME)
    struct x11
    {
        Display *dpy;
        Window window;
        Pixmap pixmap;
        GC gc;
        long int event_mask;
        int font_width, font_height;
        unsigned int new_width, new_height;
        int colors[16];
        Font font;
        XFontStruct *font_struct;
        int font_offset;
#if defined(HAVE_X11_XKBLIB_H)
        Bool detect_autorepeat;
#endif
    } x11;
#endif
};

/* Initialisation functions */
extern int _caca_init_graphics(caca_t *kk);
extern int _caca_end_graphics(caca_t *kk);

/* Timer functions */
extern void _caca_sleep(unsigned int);
extern unsigned int _caca_getticks(struct caca_timer *);

/* Cached screen size */
extern unsigned int _caca_width;
extern unsigned int _caca_height;
extern int _caca_resize;
extern int _caca_resize_event;

#if defined(USE_WIN32)
#include <windows.h>
extern HANDLE win32_hin, win32_hout;
#endif

#if defined(USE_GL)
#include <GL/glut.h>
extern unsigned int gl_width, gl_height;
#endif


#endif /* __CACA_INTERNALS_H__ */
