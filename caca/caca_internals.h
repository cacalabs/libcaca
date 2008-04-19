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

#ifndef __CACA_INTERNALS_H__
#define __CACA_INTERNALS_H__

#include "stubs.h"

typedef struct caca_timer caca_timer_t;
typedef struct caca_privevent caca_privevent_t;

#if !defined(_DOXYGEN_SKIP_ME)
#   define EVENTBUF_LEN 10
#endif

/* Graphics driver */
enum caca_driver
{
    CACA_DRIVER_NONE = 0,
    CACA_DRIVER_RAW = 1,
#if defined(USE_COCOA)
    CACA_DRIVER_COCOA = 2,
#endif
#if defined(USE_CONIO)
    CACA_DRIVER_CONIO = 3,
#endif
#if defined(USE_GL)
    CACA_DRIVER_GL = 4,
#endif
#if defined(USE_NCURSES)
    CACA_DRIVER_NCURSES = 5,
#endif
#if defined(USE_SLANG)
    CACA_DRIVER_SLANG = 6,
#endif
#if defined(USE_VGA)
    CACA_DRIVER_VGA = 7,
#endif
#if defined(USE_WIN32)
    CACA_DRIVER_WIN32 = 8,
#endif
#if defined(USE_X11)
    CACA_DRIVER_X11 = 9,
#endif
};

/* Available external drivers */
#if defined(USE_COCOA)
int cocoa_install(caca_display_t *);
#endif
#if defined(USE_CONIO)
int conio_install(caca_display_t *);
#endif
#if defined(USE_GL)
int gl_install(caca_display_t *);
#endif
#if defined(USE_NCURSES)
int ncurses_install(caca_display_t *);
#endif
int raw_install(caca_display_t *);
#if defined(USE_SLANG)
int slang_install(caca_display_t *);
#endif
#if defined(USE_VGA)
int vga_install(caca_display_t *);
#endif
#if defined(USE_WIN32)
int win32_install(caca_display_t *);
#endif
#if defined(USE_X11)
int x11_install(caca_display_t *);
#endif

/* Timer structure */
struct caca_timer
{
    int last_sec, last_usec;
};

/* Private event structure */
struct caca_privevent
{
    enum caca_event_type type;

    union
    {
        struct { unsigned int x, y, button; } mouse;
        struct { unsigned int w, h; } resize;
        struct { unsigned int ch; unsigned long int utf32; char utf8[8]; } key;
    } data;
};

/* Internal caca display context */
struct caca_display
{
    /* A link to our cucul canvas */
    cucul_canvas_t *cv;
    int autorelease;

#if defined(USE_PLUGINS)
    void *plugin;
#endif

    /* Device-specific functions */
    struct drv
    {
        char const * driver;
        enum caca_driver id;
        struct driver_private *p;

        int (* init_graphics) (caca_display_t *);
        int (* end_graphics) (caca_display_t *);
        int (* set_display_title) (caca_display_t *, char const *);
        unsigned int (* get_display_width) (caca_display_t const *);
        unsigned int (* get_display_height) (caca_display_t const *);
        void (* display) (caca_display_t *);
        void (* handle_resize) (caca_display_t *);
        int (* get_event) (caca_display_t *, caca_privevent_t *);
        void (* set_mouse) (caca_display_t *, int);
        void (* set_cursor) (caca_display_t *, int);
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
        int allow;     /* The display driver allows resizing */
        unsigned w, h; /* Requested width and height */
    } resize;

    /* Framerate handling */
    unsigned int delay, rendertime;
    caca_timer_t timer;
    int lastticks;

    struct events
    {
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
        caca_privevent_t buf[EVENTBUF_LEN];
        int queue;
#endif
#if defined(USE_SLANG) || defined(USE_NCURSES)
        caca_timer_t key_timer;
        unsigned int last_key_ticks;
        unsigned int autorepeat_ticks;
        caca_privevent_t last_key_event;
#endif
#if defined(USE_WIN32)
        unsigned char not_empty_struct;
#endif
    } events;
};

/* Internal timer functions */
extern void _caca_sleep(unsigned int);
extern unsigned int _caca_getticks(caca_timer_t *);

/* Internal event functions */
extern void _caca_handle_resize(caca_display_t *);
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
extern void _push_event(caca_display_t *, caca_privevent_t *);
extern int _pop_event(caca_display_t *, caca_privevent_t *);
#endif

/* Internal window functions */
extern void _caca_set_term_title(char const *);

#endif /* __CACA_INTERNALS_H__ */
