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

/*
 *  This file contains the libcaca X11 input and output driver
 */

#include "config.h"

#if defined(USE_X11)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#if defined(HAVE_X11_XKBLIB_H)
#   include <X11/XKBlib.h>
#endif

#include <stdio.h> /* BUFSIZ */
#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

/*
 * Local functions
 */
static int x11_error_handler(Display *, XErrorEvent *);

struct driver_private
{
    Display *dpy;
    Window window;
    Pixmap pixmap;
    GC gc;
    long int event_mask;
    int font_width, font_height;
    int colors[4096];
    Font font;
    XFontStruct *font_struct;
    int font_offset;
    Cursor pointer;
    Atom wm_protocols;
    Atom wm_delete_window;
#if defined(HAVE_X11_XKBLIB_H)
    Bool autorepeat;
#endif
};

static int x11_init_graphics(caca_t *kk)
{
    Colormap colormap;
    XSetWindowAttributes x11_winattr;
    int (*old_error_handler)(Display *, XErrorEvent *);
    char const *fonts[] = { NULL, "8x13bold", "fixed" }, **parser;
    char const *geometry;
    unsigned int width = 0, height = 0;
    int i;

    kk->drv.p = malloc(sizeof(struct driver_private));

#if defined(HAVE_GETENV)
    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

    if(width && height)
        _cucul_set_size(kk->c, width, height);

    kk->drv.p->dpy = XOpenDisplay(NULL);
    if(kk->drv.p->dpy == NULL)
        return -1;

#if defined(HAVE_GETENV)
    fonts[0] = getenv("CACA_FONT");
    if(fonts[0] && *fonts[0])
        parser = fonts;
    else
#endif
        parser = fonts + 1;

    /* Ignore font errors */
    old_error_handler = XSetErrorHandler(x11_error_handler);

    /* Parse our font list */
    for( ; ; parser++)
    {
        if(!*parser)
        {
            XSetErrorHandler(old_error_handler);
            XCloseDisplay(kk->drv.p->dpy);
            return -1;
        }

        kk->drv.p->font = XLoadFont(kk->drv.p->dpy, *parser);
        if(!kk->drv.p->font)
            continue;

        kk->drv.p->font_struct = XQueryFont(kk->drv.p->dpy, kk->drv.p->font);
        if(!kk->drv.p->font_struct)
        {
            XUnloadFont(kk->drv.p->dpy, kk->drv.p->font);
            continue;
        }

        break;
    }

    /* Reset the default X11 error handler */
    XSetErrorHandler(old_error_handler);

    kk->drv.p->font_width = kk->drv.p->font_struct->max_bounds.width;
    kk->drv.p->font_height = kk->drv.p->font_struct->max_bounds.ascent
                         + kk->drv.p->font_struct->max_bounds.descent;
    kk->drv.p->font_offset = kk->drv.p->font_struct->max_bounds.descent;

    colormap = DefaultColormap(kk->drv.p->dpy, DefaultScreen(kk->drv.p->dpy));
    for(i = 0x000; i < 0x1000; i++)
    {
        XColor color;
        color.red = ((i & 0xf00) >> 8) * 0x1111;
        color.green = ((i & 0x0f0) >> 4) * 0x1111;
        color.blue = (i & 0x00f) * 0x1111;
        XAllocColor(kk->drv.p->dpy, colormap, &color);
        kk->drv.p->colors[i] = color.pixel;
    }

    x11_winattr.backing_store = Always;
    x11_winattr.background_pixel = kk->drv.p->colors[0x000];
    x11_winattr.event_mask = ExposureMask | StructureNotifyMask;

    kk->drv.p->window =
        XCreateWindow(kk->drv.p->dpy, DefaultRootWindow(kk->drv.p->dpy), 0, 0,
                      kk->c->width * kk->drv.p->font_width,
                      kk->c->height * kk->drv.p->font_height,
                      0, 0, InputOutput, 0,
                      CWBackingStore | CWBackPixel | CWEventMask,
                      &x11_winattr);

    kk->drv.p->wm_protocols =
        XInternAtom(kk->drv.p->dpy, "WM_PROTOCOLS", True);
    kk->drv.p->wm_delete_window =
        XInternAtom(kk->drv.p->dpy, "WM_DELETE_WINDOW", True);

    if(kk->drv.p->wm_protocols != None && kk->drv.p->wm_delete_window != None)
        XSetWMProtocols(kk->drv.p->dpy, kk->drv.p->window,
                        &kk->drv.p->wm_delete_window, 1);

    XStoreName(kk->drv.p->dpy, kk->drv.p->window, "caca for X");

    XSelectInput(kk->drv.p->dpy, kk->drv.p->window, StructureNotifyMask);
    XMapWindow(kk->drv.p->dpy, kk->drv.p->window);

    kk->drv.p->gc = XCreateGC(kk->drv.p->dpy, kk->drv.p->window, 0, NULL);
    XSetForeground(kk->drv.p->dpy, kk->drv.p->gc, kk->drv.p->colors[0x888]);
    XSetFont(kk->drv.p->dpy, kk->drv.p->gc, kk->drv.p->font);

    for(;;)
    {
        XEvent xevent;
        XNextEvent(kk->drv.p->dpy, &xevent);
        if (xevent.type == MapNotify)
            break;
    }

#if defined(HAVE_X11_XKBLIB_H)
    /* Disable autorepeat */
    XkbSetDetectableAutoRepeat(kk->drv.p->dpy, True, &kk->drv.p->autorepeat);
    if(!kk->drv.p->autorepeat)
        XAutoRepeatOff(kk->drv.p->dpy);
#endif

    kk->drv.p->event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask
          | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask
          | ExposureMask;

    XSelectInput(kk->drv.p->dpy, kk->drv.p->window, kk->drv.p->event_mask);

    XSync(kk->drv.p->dpy, False);

    kk->drv.p->pixmap = XCreatePixmap(kk->drv.p->dpy, kk->drv.p->window,
                                   kk->c->width * kk->drv.p->font_width,
                                   kk->c->height * kk->drv.p->font_height,
                                   DefaultDepth(kk->drv.p->dpy,
                                            DefaultScreen(kk->drv.p->dpy)));
    kk->drv.p->pointer = None;

    return 0;
}

static int x11_end_graphics(caca_t *kk)
{
    XSync(kk->drv.p->dpy, False);
#if defined(HAVE_X11_XKBLIB_H)
    if(!kk->drv.p->autorepeat)
        XAutoRepeatOn(kk->drv.p->dpy);
#endif
    XFreePixmap(kk->drv.p->dpy, kk->drv.p->pixmap);
    XFreeFont(kk->drv.p->dpy, kk->drv.p->font_struct);
    XFreeGC(kk->drv.p->dpy, kk->drv.p->gc);
    XUnmapWindow(kk->drv.p->dpy, kk->drv.p->window);
    XDestroyWindow(kk->drv.p->dpy, kk->drv.p->window);
    XCloseDisplay(kk->drv.p->dpy);

    free(kk->drv.p);

    return 0;
}

static int x11_set_window_title(caca_t *kk, char const *title)
{
    XStoreName(kk->drv.p->dpy, kk->drv.p->window, title);
    return 0;
}

static unsigned int x11_get_window_width(caca_t *kk)
{
    return kk->c->width * kk->drv.p->font_width;
}

static unsigned int x11_get_window_height(caca_t *kk)
{
    return kk->c->height * kk->drv.p->font_height;
}

static void x11_display(caca_t *kk)
{
    unsigned int x, y, len;

    /* First draw the background colours. Splitting the process in two
     * loops like this is actually slightly faster. */
    for(y = 0; y < kk->c->height; y++)
    {
        for(x = 0; x < kk->c->width; x += len)
        {
            uint32_t *attr = kk->c->attr + x + y * kk->c->width;
            uint16_t bg = _cucul_argb32_to_rgb12bg(*attr);

            len = 1;
            while(x + len < kk->c->width
                   && _cucul_argb32_to_rgb12bg(attr[len]) == bg)
                len++;

            XSetForeground(kk->drv.p->dpy, kk->drv.p->gc,
                           kk->drv.p->colors[bg]);
            XFillRectangle(kk->drv.p->dpy, kk->drv.p->pixmap, kk->drv.p->gc,
                           x * kk->drv.p->font_width, y * kk->drv.p->font_height,
                           len * kk->drv.p->font_width, kk->drv.p->font_height);
        }
    }

    /* Then print the foreground characters */
    for(y = 0; y < kk->c->height; y++)
    {
        unsigned int yoff = (y + 1) * kk->drv.p->font_height
                                    - kk->drv.p->font_offset;
        uint32_t *chars = kk->c->chars + y * kk->c->width;

        for(x = 0; x < kk->c->width; x++, chars++)
        {
            uint32_t *attr = kk->c->attr + x + y * kk->c->width;

            /* Skip spaces */
            if(*chars == 0x00000020)
                continue;

            XSetForeground(kk->drv.p->dpy, kk->drv.p->gc,
                           kk->drv.p->colors[_cucul_argb32_to_rgb12fg(*attr)]);

            /* Plain ASCII, no problem. */
            if(*chars > 0x00000020 && *chars < 0x00000080)
            {
                char c = (uint8_t)*chars;
                XDrawString(kk->drv.p->dpy, kk->drv.p->pixmap, kk->drv.p->gc,
                            x * kk->drv.p->font_width, yoff, &c, 1);
                continue;
            }

            /* We want to be able to print a few special Unicode characters
             * such as the CP437 gradients and half blocks. For unknown
             * characters, just print '?'. */
            switch(*chars)
            {
                case 0x00002580: /* ▀ */
                    XFillRectangle(kk->drv.p->dpy, kk->drv.p->pixmap,
                                   kk->drv.p->gc,
                                   x * kk->drv.p->font_width,
                                   y * kk->drv.p->font_height,
                                   kk->drv.p->font_width,
                                   kk->drv.p->font_height / 2);
                    break;
                case 0x00002584: /* ▄ */
                    XFillRectangle(kk->drv.p->dpy, kk->drv.p->pixmap,
                                   kk->drv.p->gc,
                                   x * kk->drv.p->font_width,
                                   (y + 1) * kk->drv.p->font_height
                                           - kk->drv.p->font_height / 2,
                                   kk->drv.p->font_width,
                                   kk->drv.p->font_height / 2);
                    break;
                case 0x00002588: /* █ */
                    XFillRectangle(kk->drv.p->dpy, kk->drv.p->pixmap,
                                   kk->drv.p->gc,
                                   x * kk->drv.p->font_width,
                                   y * kk->drv.p->font_height,
                                   kk->drv.p->font_width,
                                   kk->drv.p->font_height);
                    break;
                case 0x0000258c: /* ▌ */
                    XFillRectangle(kk->drv.p->dpy, kk->drv.p->pixmap,
                                   kk->drv.p->gc,
                                   x * kk->drv.p->font_width,
                                   y * kk->drv.p->font_height,
                                   kk->drv.p->font_width / 2,
                                   kk->drv.p->font_height);
                    break;
                case 0x00002590: /* ▐ */
                    XFillRectangle(kk->drv.p->dpy, kk->drv.p->pixmap,
                                   kk->drv.p->gc,
                                   (x + 1) * kk->drv.p->font_width
                                           - kk->drv.p->font_width / 2,
                                   y * kk->drv.p->font_height,
                                   kk->drv.p->font_width / 2,
                                   kk->drv.p->font_height);
                    break;
                case 0x00002593: /* ▓ */
                case 0x00002592: /* ▒ */
                case 0x00002591: /* ░ */
                {
                    /* FIXME: this sucks utterly */
                    int i, j, k = *chars - 0x00002591;
                    for(j = kk->drv.p->font_height; j--; )
                        for(i = kk->drv.p->font_width; i--; )
                    {
                        if(((i + 2 * (j & 1)) & 3) > k)
                            continue;

                        XDrawPoint(kk->drv.p->dpy, kk->drv.p->pixmap,
                                   kk->drv.p->gc,
                                   x * kk->drv.p->font_width + i,
                                   y * kk->drv.p->font_height + j);
                    }
                    break;
                }
                default:
                {
                    char c;
                    c = '?';
                    XDrawString(kk->drv.p->dpy, kk->drv.p->pixmap,
                                kk->drv.p->gc,
                                x * kk->drv.p->font_width, yoff, &c, 1);
                    break;
                }
            }
        }
    }

    XCopyArea(kk->drv.p->dpy, kk->drv.p->pixmap, kk->drv.p->window,
              kk->drv.p->gc, 0, 0,
              kk->c->width * kk->drv.p->font_width,
              kk->c->height * kk->drv.p->font_height,
              0, 0);
    XFlush(kk->drv.p->dpy);
}

static void x11_handle_resize(caca_t *kk)
{
    Pixmap new_pixmap;

    new_pixmap = XCreatePixmap(kk->drv.p->dpy, kk->drv.p->window,
                               kk->resize.w * kk->drv.p->font_width,
                               kk->resize.h * kk->drv.p->font_height,
                               DefaultDepth(kk->drv.p->dpy,
                                            DefaultScreen(kk->drv.p->dpy)));
    XCopyArea(kk->drv.p->dpy, kk->drv.p->pixmap, new_pixmap,
              kk->drv.p->gc, 0, 0,
              kk->resize.w * kk->drv.p->font_width,
              kk->resize.h * kk->drv.p->font_height, 0, 0);
    XFreePixmap(kk->drv.p->dpy, kk->drv.p->pixmap);
    kk->drv.p->pixmap = new_pixmap;
}

static int x11_get_event(caca_t *kk, caca_event_t *ev)
{
    XEvent xevent;
    char key;

    while(XCheckWindowEvent(kk->drv.p->dpy, kk->drv.p->window,
                            kk->drv.p->event_mask, &xevent) == True)
    {
        KeySym keysym;

        /* Expose event */
        if(xevent.type == Expose)
        {
            XCopyArea(kk->drv.p->dpy, kk->drv.p->pixmap,
                      kk->drv.p->window, kk->drv.p->gc, 0, 0,
                      kk->c->width * kk->drv.p->font_width,
                      kk->c->height * kk->drv.p->font_height, 0, 0);
            continue;
        }

        /* Resize event */
        if(xevent.type == ConfigureNotify)
        {
            unsigned int w, h;

            w = (xevent.xconfigure.width + kk->drv.p->font_width / 3)
                  / kk->drv.p->font_width;
            h = (xevent.xconfigure.height + kk->drv.p->font_height / 3)
                  / kk->drv.p->font_height;

            if(!w || !h || (w == kk->c->width && h == kk->c->height))
                continue;

            kk->resize.w = w;
            kk->resize.h = h;
            kk->resize.resized = 1;

            continue;
        }

        /* Check for mouse motion events */
        if(xevent.type == MotionNotify)
        {
            unsigned int newx = xevent.xmotion.x / kk->drv.p->font_width;
            unsigned int newy = xevent.xmotion.y / kk->drv.p->font_height;

            if(newx >= kk->c->width)
                newx = kk->c->width - 1;
            if(newy >= kk->c->height)
                newy = kk->c->height - 1;

            if(kk->mouse.x == newx && kk->mouse.y == newy)
                continue;

            kk->mouse.x = newx;
            kk->mouse.y = newy;

            ev->type = CACA_EVENT_MOUSE_MOTION;
            ev->data.mouse.x = kk->mouse.x;
            ev->data.mouse.y = kk->mouse.y;
            return 1;
        }

        /* Check for mouse press and release events */
        if(xevent.type == ButtonPress)
        {
            ev->type = CACA_EVENT_MOUSE_PRESS;
            ev->data.mouse.button = ((XButtonEvent *)&xevent)->button;
            return 1;
        }

        if(xevent.type == ButtonRelease)
        {
            ev->type = CACA_EVENT_MOUSE_RELEASE;
            ev->data.mouse.button = ((XButtonEvent *)&xevent)->button;
            return 1;
        }

        /* Check for key press and release events */
        if(xevent.type == KeyPress)
            ev->type = CACA_EVENT_KEY_PRESS;
        else if(xevent.type == KeyRelease)
            ev->type = CACA_EVENT_KEY_RELEASE;
        else
            continue;

        if(XLookupString(&xevent.xkey, &key, 1, NULL, NULL))
        {
            ev->data.key.ch = key;
            ev->data.key.ucs4 = key;
            ev->data.key.utf8[0] = key;
            ev->data.key.utf8[1] = '\0';
            return 1;
        }

        keysym = XKeycodeToKeysym(kk->drv.p->dpy, xevent.xkey.keycode, 0);
        switch(keysym)
        {
            case XK_F1:    ev->data.key.ch = CACA_KEY_F1;    break;
            case XK_F2:    ev->data.key.ch = CACA_KEY_F2;    break;
            case XK_F3:    ev->data.key.ch = CACA_KEY_F3;    break;
            case XK_F4:    ev->data.key.ch = CACA_KEY_F4;    break;
            case XK_F5:    ev->data.key.ch = CACA_KEY_F5;    break;
            case XK_F6:    ev->data.key.ch = CACA_KEY_F6;    break;
            case XK_F7:    ev->data.key.ch = CACA_KEY_F7;    break;
            case XK_F8:    ev->data.key.ch = CACA_KEY_F8;    break;
            case XK_F9:    ev->data.key.ch = CACA_KEY_F9;    break;
            case XK_F10:   ev->data.key.ch = CACA_KEY_F10;   break;
            case XK_F11:   ev->data.key.ch = CACA_KEY_F11;   break;
            case XK_F12:   ev->data.key.ch = CACA_KEY_F12;   break;
            case XK_F13:   ev->data.key.ch = CACA_KEY_F13;   break;
            case XK_F14:   ev->data.key.ch = CACA_KEY_F14;   break;
            case XK_F15:   ev->data.key.ch = CACA_KEY_F15;   break;
            case XK_Left:  ev->data.key.ch = CACA_KEY_LEFT;  break;
            case XK_Right: ev->data.key.ch = CACA_KEY_RIGHT; break;
            case XK_Up:    ev->data.key.ch = CACA_KEY_UP;    break;
            case XK_Down:  ev->data.key.ch = CACA_KEY_DOWN;  break;

            default: ev->type = CACA_EVENT_NONE; return 0;
        }

        ev->data.key.ucs4 = 0;
        ev->data.key.utf8[0] = '\0';
        return 1;
    }

    while(XCheckTypedEvent(kk->drv.p->dpy, ClientMessage, &xevent))
    {
        if(xevent.xclient.message_type != kk->drv.p->wm_protocols)
            continue;

        if((Atom)xevent.xclient.data.l[0] == kk->drv.p->wm_delete_window)
        {
            ev->type = CACA_EVENT_QUIT;
            return 1;
        }
    }

    ev->type = CACA_EVENT_NONE;
    return 0;
}

static void x11_set_mouse(caca_t *kk, int flags)
{
    Cursor no_ptr;
    Pixmap bm_no;
    XColor black, dummy;
    Colormap colormap;
    static char const empty[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    if(flags)
    {
        XDefineCursor(kk->drv.p->dpy,kk->drv.p->window, 0);
        return;
    }

    colormap = DefaultColormap(kk->drv.p->dpy, DefaultScreen(kk->drv.p->dpy));
    if(!XAllocNamedColor(kk->drv.p->dpy, colormap, "black", &black, &dummy))
    {
        return;
    }
    bm_no = XCreateBitmapFromData(kk->drv.p->dpy, kk->drv.p->window,
                                  empty, 8, 8);
    no_ptr = XCreatePixmapCursor(kk->drv.p->dpy, bm_no, bm_no,
                                 &black, &black, 0, 0);
    XDefineCursor(kk->drv.p->dpy, kk->drv.p->window, no_ptr);
    XFreeCursor(kk->drv.p->dpy, no_ptr);
    if(bm_no != None)
        XFreePixmap(kk->drv.p->dpy, bm_no);
    XFreeColors(kk->drv.p->dpy, colormap, &black.pixel, 1, 0);

    XSync(kk->drv.p->dpy, False);
}

/*
 * XXX: following functions are local
 */

static int x11_error_handler(Display *dpy, XErrorEvent *xevent)
{
    /* Ignore the error */
    return 0;
}

/*
 * Driver initialisation
 */

int x11_install(caca_t *kk)
{
#if defined(HAVE_GETENV)
    if(!getenv("DISPLAY") || !*(getenv("DISPLAY")))
        return -1;
#endif

    kk->drv.driver = CACA_DRIVER_X11;

    kk->drv.init_graphics = x11_init_graphics;
    kk->drv.end_graphics = x11_end_graphics;
    kk->drv.set_window_title = x11_set_window_title;
    kk->drv.get_window_width = x11_get_window_width;
    kk->drv.get_window_height = x11_get_window_height;
    kk->drv.display = x11_display;
    kk->drv.handle_resize = x11_handle_resize;
    kk->drv.get_event = x11_get_event;
    kk->drv.set_mouse = x11_set_mouse;

    return 0;
}

#endif /* USE_X11 */

