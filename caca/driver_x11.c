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

/** \file graphics.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Character drawing
 *
 *  This file contains character and string drawing functions.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H) || defined(_DOXYGEN_SKIP_ME)
#   include <inttypes.h>
#else
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#endif

#if defined(USE_X11)

#include <X11/Xlib.h>
#if defined(HAVE_X11_XKBLIB_H)
#   include <X11/XKBlib.h>
#endif

#include <stdio.h> /* BUFSIZ */
#include <string.h>
#include <stdlib.h>
#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif
#include <stdarg.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

int x11_init_graphics(caca_t *);
int x11_end_graphics(caca_t *);
int x11_set_window_title(caca_t *, char const *);
unsigned int x11_get_window_width(caca_t *);
unsigned int x11_get_window_height(caca_t *);
void x11_display(caca_t *);
void x11_handle_resize(caca_t *);

/*
 * Local functions
 */
static int x11_error_handler(Display *, XErrorEvent *);

#if !defined(_DOXYGEN_SKIP_ME)
int x11_init_graphics(caca_t *kk)
{
    static int const x11_palette[] =
    {
        /* Standard curses colours */
        0x0,    0x0,    0x0,
        0x0,    0x0,    0x8000,
        0x0,    0x8000, 0x0,
        0x0,    0x8000, 0x8000,
        0x8000, 0x0,    0x0,
        0x8000, 0x0,    0x8000,
        0x8000, 0x8000, 0x0,
        0x8000, 0x8000, 0x8000,
        /* Extra values for xterm-16color */
        0x4000, 0x4000, 0x4000,
        0x4000, 0x4000, 0xffff,
        0x4000, 0xffff, 0x4000,
        0x4000, 0xffff, 0xffff,
        0xffff, 0x4000, 0x4000,
        0xffff, 0x4000, 0xffff,
        0xffff, 0xffff, 0x4000,
        0xffff, 0xffff, 0xffff,
    };

    Colormap colormap;
    XSetWindowAttributes x11_winattr;
    int (*old_error_handler)(Display *, XErrorEvent *);
    char const *fonts[] = { NULL, "8x13bold", "fixed" }, **parser;
    char const *geometry;
    unsigned int width = 0, height = 0;
    int i;

    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *(geometry))
        sscanf(geometry, "%ux%u", &width, &height);

    if(width && height)
        cucul_set_size(kk->qq, width, height);

    kk->x11.dpy = XOpenDisplay(NULL);
    if(kk->x11.dpy == NULL)
        return -1;

    fonts[0] = getenv("CACA_FONT");
    if(fonts[0] && *fonts[0])
        parser = fonts;
    else
        parser = fonts + 1;

    /* Ignore font errors */
    old_error_handler = XSetErrorHandler(x11_error_handler);

    /* Parse our font list */
    for( ; ; parser++)
    {
        if(!*parser)
        {
            XSetErrorHandler(old_error_handler);
            XCloseDisplay(kk->x11.dpy);
            return -1;
        }

        kk->x11.font = XLoadFont(kk->x11.dpy, *parser);
        if(!kk->x11.font)
            continue;

        kk->x11.font_struct = XQueryFont(kk->x11.dpy, kk->x11.font);
        if(!kk->x11.font_struct)
        {
            XUnloadFont(kk->x11.dpy, kk->x11.font);
            continue;
        }

        break;
    }

    /* Reset the default X11 error handler */
    XSetErrorHandler(old_error_handler);

    kk->x11.font_width = kk->x11.font_struct->max_bounds.width;
    kk->x11.font_height = kk->x11.font_struct->max_bounds.ascent
                         + kk->x11.font_struct->max_bounds.descent;
    kk->x11.font_offset = kk->x11.font_struct->max_bounds.descent;

    colormap = DefaultColormap(kk->x11.dpy, DefaultScreen(kk->x11.dpy));
    for(i = 0; i < 16; i++)
    {
        XColor color;
        color.red = x11_palette[i * 3];
        color.green = x11_palette[i * 3 + 1];
        color.blue = x11_palette[i * 3 + 2];
        XAllocColor(kk->x11.dpy, colormap, &color);
        kk->x11.colors[i] = color.pixel;
    }

    x11_winattr.backing_store = Always;
    x11_winattr.background_pixel = kk->x11.colors[0];
    x11_winattr.event_mask = ExposureMask | StructureNotifyMask;

    kk->x11.window =
        XCreateWindow(kk->x11.dpy, DefaultRootWindow(kk->x11.dpy), 0, 0,
                      kk->qq->width * kk->x11.font_width,
                      kk->qq->height * kk->x11.font_height,
                      0, 0, InputOutput, 0,
                      CWBackingStore | CWBackPixel | CWEventMask,
                      &x11_winattr);

    XStoreName(kk->x11.dpy, kk->x11.window, "caca for X");

    XSelectInput(kk->x11.dpy, kk->x11.window, StructureNotifyMask);
    XMapWindow(kk->x11.dpy, kk->x11.window);

    kk->x11.gc = XCreateGC(kk->x11.dpy, kk->x11.window, 0, NULL);
    XSetForeground(kk->x11.dpy, kk->x11.gc, kk->x11.colors[15]);
    XSetFont(kk->x11.dpy, kk->x11.gc, kk->x11.font);

    for(;;)
    {
        XEvent event;
        XNextEvent(kk->x11.dpy, &event);
        if (event.type == MapNotify)
            break;
    }

#if defined(HAVE_X11_XKBLIB_H)
    /* Disable autorepeat */
    XkbSetDetectableAutoRepeat(kk->x11.dpy, True, &kk->x11.autorepeat);
    if(!kk->x11.autorepeat)
        XAutoRepeatOff(kk->x11.dpy);
#endif

    kk->x11.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask
          | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask
          | ExposureMask;

    XSelectInput(kk->x11.dpy, kk->x11.window, kk->x11.event_mask);

    XSync(kk->x11.dpy, False);

    kk->x11.pixmap = XCreatePixmap(kk->x11.dpy, kk->x11.window,
                                   kk->qq->width * kk->x11.font_width,
                                   kk->qq->height * kk->x11.font_height,
                                   DefaultDepth(kk->x11.dpy,
                                            DefaultScreen(kk->x11.dpy)));

    kk->x11.new_width = kk->x11.new_height = 0;

    return 0;
}

int x11_end_graphics(caca_t *kk)
{
    XSync(kk->x11.dpy, False);
#if defined(HAVE_X11_XKBLIB_H)
    if(!kk->x11.autorepeat)
        XAutoRepeatOn(kk->x11.dpy);
#endif
    XFreePixmap(kk->x11.dpy, kk->x11.pixmap);
    XFreeFont(kk->x11.dpy, kk->x11.font_struct);
    XFreeGC(kk->x11.dpy, kk->x11.gc);
    XUnmapWindow(kk->x11.dpy, kk->x11.window);
    XDestroyWindow(kk->x11.dpy, kk->x11.window);
    XCloseDisplay(kk->x11.dpy);

    return 0;
}
#endif /* _DOXYGEN_SKIP_ME */

int x11_set_window_title(caca_t *kk, char const *title)
{
    XStoreName(kk->x11.dpy, kk->x11.window, title);
    return 0;
}

unsigned int x11_get_window_width(caca_t *kk)
{
    return kk->qq->width * kk->x11.font_width;
}

unsigned int x11_get_window_height(caca_t *kk)
{
    return kk->qq->height * kk->x11.font_height;
}

void x11_display(caca_t *kk)
{
    unsigned int x, y, len;

    /* First draw the background colours. Splitting the process in two
     * loops like this is actually slightly faster. */
    for(y = 0; y < kk->qq->height; y++)
    {
        for(x = 0; x < kk->qq->width; x += len)
        {
            uint8_t *attr = kk->qq->attr + x + y * kk->qq->width;

            len = 1;
            while(x + len < kk->qq->width
                   && (attr[len] >> 4) == (attr[0] >> 4))
                len++;

            XSetForeground(kk->x11.dpy, kk->x11.gc,
                           kk->x11.colors[attr[0] >> 4]);
            XFillRectangle(kk->x11.dpy, kk->x11.pixmap, kk->x11.gc,
                           x * kk->x11.font_width, y * kk->x11.font_height,
                           len * kk->x11.font_width, kk->x11.font_height);
        }
    }

    /* Then print the foreground characters */
    for(y = 0; y < kk->qq->height; y++)
    {
        for(x = 0; x < kk->qq->width; x += len)
        {
            char buffer[BUFSIZ]; /* FIXME: use a smaller buffer */
            uint32_t *chars = kk->qq->chars + x + y * kk->qq->width;
            uint8_t *attr = kk->qq->attr + x + y * kk->qq->width;

            len = 1;

            /* Skip spaces */
            if(chars[0] == ' ')
                continue;

            buffer[0] = chars[0] & 0x7f;

            while(x + len < kk->qq->width
                   && (attr[len] & 0xf) == (attr[0] & 0xf))
            {
                buffer[len] = chars[len] & 0x7f;
                len++;
            }

            XSetForeground(kk->x11.dpy, kk->x11.gc, kk->x11.colors[attr[0] & 0xf]);
            XDrawString(kk->x11.dpy, kk->x11.pixmap, kk->x11.gc,
                        x * kk->x11.font_width,
                        (y + 1) * kk->x11.font_height - kk->x11.font_offset,
                        buffer, len);
        }
    }

    XCopyArea(kk->x11.dpy, kk->x11.pixmap, kk->x11.window, kk->x11.gc, 0, 0,
              kk->qq->width * kk->x11.font_width,
              kk->qq->height * kk->x11.font_height,
              0, 0);
    XFlush(kk->x11.dpy);
}

void x11_handle_resize(caca_t *kk)
{
    unsigned int new_width, new_height;

    Pixmap new_pixmap;

    new_width = kk->qq->width;
    new_height = kk->qq->height;

    new_width = kk->x11.new_width;
    new_height = kk->x11.new_height;

    new_pixmap = XCreatePixmap(kk->x11.dpy, kk->x11.window,
                               kk->qq->width * kk->x11.font_width,
                               kk->qq->height * kk->x11.font_height,
                               DefaultDepth(kk->x11.dpy,
                                            DefaultScreen(kk->x11.dpy)));
    XCopyArea(kk->x11.dpy, kk->x11.pixmap, new_pixmap, kk->x11.gc, 0, 0,
              kk->qq->width * kk->x11.font_width,
              kk->qq->height * kk->x11.font_height, 0, 0);
    XFreePixmap(kk->x11.dpy, kk->x11.pixmap);
    kk->x11.pixmap = new_pixmap;
}

/*
 * XXX: following functions are local
 */

static int x11_error_handler(Display *dpy, XErrorEvent *event)
{
    /* Ignore the error */
    return 0;
}

/*
 * Driver initialisation
 */

void x11_init_driver(caca_t *kk)
{
    kk->driver.driver = CACA_DRIVER_X11;

    kk->driver.init_graphics = x11_init_graphics;
    kk->driver.end_graphics = x11_end_graphics;
    kk->driver.set_window_title = x11_set_window_title;
    kk->driver.get_window_width = x11_get_window_width;
    kk->driver.get_window_height = x11_get_window_height;
    kk->driver.display = x11_display;
    kk->driver.handle_resize = x11_handle_resize;
}

#endif /* USE_X11 */

