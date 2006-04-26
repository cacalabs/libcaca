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
#include "common.h"

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

static int x11_init_graphics(caca_display_t *dp)
{
    Colormap colormap;
    XSetWindowAttributes x11_winattr;
    int (*old_error_handler)(Display *, XErrorEvent *);
    char const *fonts[] = { NULL, "8x13bold", "fixed" }, **parser;
    char const *geometry;
    unsigned int width = 0, height = 0;
    int i;

    dp->drv.p = malloc(sizeof(struct driver_private));

#if defined(HAVE_GETENV)
    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

    if(width && height)
        _cucul_set_canvas_size(dp->cv, width, height);

    dp->drv.p->dpy = XOpenDisplay(NULL);
    if(dp->drv.p->dpy == NULL)
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
            XCloseDisplay(dp->drv.p->dpy);
            return -1;
        }

        dp->drv.p->font = XLoadFont(dp->drv.p->dpy, *parser);
        if(!dp->drv.p->font)
            continue;

        dp->drv.p->font_struct = XQueryFont(dp->drv.p->dpy, dp->drv.p->font);
        if(!dp->drv.p->font_struct)
        {
            XUnloadFont(dp->drv.p->dpy, dp->drv.p->font);
            continue;
        }

        break;
    }

    /* Reset the default X11 error handler */
    XSetErrorHandler(old_error_handler);

    dp->drv.p->font_width = dp->drv.p->font_struct->max_bounds.width;
    dp->drv.p->font_height = dp->drv.p->font_struct->max_bounds.ascent
                         + dp->drv.p->font_struct->max_bounds.descent;
    dp->drv.p->font_offset = dp->drv.p->font_struct->max_bounds.descent;

    colormap = DefaultColormap(dp->drv.p->dpy, DefaultScreen(dp->drv.p->dpy));
    for(i = 0x000; i < 0x1000; i++)
    {
        XColor color;
        color.red = ((i & 0xf00) >> 8) * 0x1111;
        color.green = ((i & 0x0f0) >> 4) * 0x1111;
        color.blue = (i & 0x00f) * 0x1111;
        XAllocColor(dp->drv.p->dpy, colormap, &color);
        dp->drv.p->colors[i] = color.pixel;
    }

    x11_winattr.backing_store = Always;
    x11_winattr.background_pixel = dp->drv.p->colors[0x000];
    x11_winattr.event_mask = ExposureMask | StructureNotifyMask;

    dp->drv.p->window =
        XCreateWindow(dp->drv.p->dpy, DefaultRootWindow(dp->drv.p->dpy), 0, 0,
                      dp->cv->width * dp->drv.p->font_width,
                      dp->cv->height * dp->drv.p->font_height,
                      0, 0, InputOutput, 0,
                      CWBackingStore | CWBackPixel | CWEventMask,
                      &x11_winattr);

    dp->drv.p->wm_protocols =
        XInternAtom(dp->drv.p->dpy, "WM_PROTOCOLS", True);
    dp->drv.p->wm_delete_window =
        XInternAtom(dp->drv.p->dpy, "WM_DELETE_WINDOW", True);

    if(dp->drv.p->wm_protocols != None && dp->drv.p->wm_delete_window != None)
        XSetWMProtocols(dp->drv.p->dpy, dp->drv.p->window,
                        &dp->drv.p->wm_delete_window, 1);

    XStoreName(dp->drv.p->dpy, dp->drv.p->window, "caca for X");

    XSelectInput(dp->drv.p->dpy, dp->drv.p->window, StructureNotifyMask);
    XMapWindow(dp->drv.p->dpy, dp->drv.p->window);

    dp->drv.p->gc = XCreateGC(dp->drv.p->dpy, dp->drv.p->window, 0, NULL);
    XSetForeground(dp->drv.p->dpy, dp->drv.p->gc, dp->drv.p->colors[0x888]);
    XSetFont(dp->drv.p->dpy, dp->drv.p->gc, dp->drv.p->font);

    for(;;)
    {
        XEvent xevent;
        XNextEvent(dp->drv.p->dpy, &xevent);
        if (xevent.type == MapNotify)
            break;
    }

#if defined(HAVE_X11_XKBLIB_H)
    /* Disable autorepeat */
    XkbSetDetectableAutoRepeat(dp->drv.p->dpy, True, &dp->drv.p->autorepeat);
    if(!dp->drv.p->autorepeat)
        XAutoRepeatOff(dp->drv.p->dpy);
#endif

    dp->drv.p->event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask
          | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask
          | ExposureMask;

    XSelectInput(dp->drv.p->dpy, dp->drv.p->window, dp->drv.p->event_mask);

    XSync(dp->drv.p->dpy, False);

    dp->drv.p->pixmap = XCreatePixmap(dp->drv.p->dpy, dp->drv.p->window,
                                   dp->cv->width * dp->drv.p->font_width,
                                   dp->cv->height * dp->drv.p->font_height,
                                   DefaultDepth(dp->drv.p->dpy,
                                            DefaultScreen(dp->drv.p->dpy)));
    dp->drv.p->pointer = None;

    return 0;
}

static int x11_end_graphics(caca_display_t *dp)
{
    XSync(dp->drv.p->dpy, False);
#if defined(HAVE_X11_XKBLIB_H)
    if(!dp->drv.p->autorepeat)
        XAutoRepeatOn(dp->drv.p->dpy);
#endif
    XFreePixmap(dp->drv.p->dpy, dp->drv.p->pixmap);
    XFreeFont(dp->drv.p->dpy, dp->drv.p->font_struct);
    XFreeGC(dp->drv.p->dpy, dp->drv.p->gc);
    XUnmapWindow(dp->drv.p->dpy, dp->drv.p->window);
    XDestroyWindow(dp->drv.p->dpy, dp->drv.p->window);
    XCloseDisplay(dp->drv.p->dpy);

    free(dp->drv.p);

    return 0;
}

static int x11_set_display_title(caca_display_t *dp, char const *title)
{
    XStoreName(dp->drv.p->dpy, dp->drv.p->window, title);
    return 0;
}

static unsigned int x11_get_display_width(caca_display_t *dp)
{
    return dp->cv->width * dp->drv.p->font_width;
}

static unsigned int x11_get_display_height(caca_display_t *dp)
{
    return dp->cv->height * dp->drv.p->font_height;
}

static void x11_display(caca_display_t *dp)
{
    unsigned int x, y, len;

    /* First draw the background colours. Splitting the process in two
     * loops like this is actually slightly faster. */
    for(y = 0; y < dp->cv->height; y++)
    {
        for(x = 0; x < dp->cv->width; x += len)
        {
            uint32_t *attr = dp->cv->attr + x + y * dp->cv->width;
            uint16_t bg = _cucul_argb32_to_rgb12bg(*attr);

            len = 1;
            while(x + len < dp->cv->width
                   && _cucul_argb32_to_rgb12bg(attr[len]) == bg)
                len++;

            XSetForeground(dp->drv.p->dpy, dp->drv.p->gc,
                           dp->drv.p->colors[bg]);
            XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap, dp->drv.p->gc,
                           x * dp->drv.p->font_width, y * dp->drv.p->font_height,
                           len * dp->drv.p->font_width, dp->drv.p->font_height);
        }
    }

    /* Then print the foreground characters */
    for(y = 0; y < dp->cv->height; y++)
    {
        unsigned int yoff = (y + 1) * dp->drv.p->font_height
                                    - dp->drv.p->font_offset;
        uint32_t *chars = dp->cv->chars + y * dp->cv->width;

        for(x = 0; x < dp->cv->width; x++, chars++)
        {
            uint32_t *attr = dp->cv->attr + x + y * dp->cv->width;

            /* Skip spaces */
            if(*chars == 0x00000020)
                continue;

            XSetForeground(dp->drv.p->dpy, dp->drv.p->gc,
                           dp->drv.p->colors[_cucul_argb32_to_rgb12fg(*attr)]);

            /* Plain ASCII, no problem. */
            if(*chars > 0x00000020 && *chars < 0x00000080)
            {
                char ch = (uint8_t)*chars;
                XDrawString(dp->drv.p->dpy, dp->drv.p->pixmap, dp->drv.p->gc,
                            x * dp->drv.p->font_width, yoff, &ch, 1);
                continue;
            }

            /* We want to be able to print a few special Unicode characters
             * such as the CP437 gradients and half blocks. For unknown
             * characters, just print '?'. */
            switch(*chars)
            {
                case 0x00002580: /* ▀ */
                    XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap,
                                   dp->drv.p->gc,
                                   x * dp->drv.p->font_width,
                                   y * dp->drv.p->font_height,
                                   dp->drv.p->font_width,
                                   dp->drv.p->font_height / 2);
                    break;
                case 0x00002584: /* ▄ */
                    XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap,
                                   dp->drv.p->gc,
                                   x * dp->drv.p->font_width,
                                   (y + 1) * dp->drv.p->font_height
                                           - dp->drv.p->font_height / 2,
                                   dp->drv.p->font_width,
                                   dp->drv.p->font_height / 2);
                    break;
                case 0x00002588: /* █ */
                    XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap,
                                   dp->drv.p->gc,
                                   x * dp->drv.p->font_width,
                                   y * dp->drv.p->font_height,
                                   dp->drv.p->font_width,
                                   dp->drv.p->font_height);
                    break;
                case 0x0000258c: /* ▌ */
                    XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap,
                                   dp->drv.p->gc,
                                   x * dp->drv.p->font_width,
                                   y * dp->drv.p->font_height,
                                   dp->drv.p->font_width / 2,
                                   dp->drv.p->font_height);
                    break;
                case 0x00002590: /* ▐ */
                    XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap,
                                   dp->drv.p->gc,
                                   (x + 1) * dp->drv.p->font_width
                                           - dp->drv.p->font_width / 2,
                                   y * dp->drv.p->font_height,
                                   dp->drv.p->font_width / 2,
                                   dp->drv.p->font_height);
                    break;
                case 0x00002593: /* ▓ */
                case 0x00002592: /* ▒ */
                case 0x00002591: /* ░ */
                {
                    /* FIXME: this sucks utterly */
                    int i, j, k = *chars - 0x00002591;
                    for(j = dp->drv.p->font_height; j--; )
                        for(i = dp->drv.p->font_width; i--; )
                    {
                        if(((i + 2 * (j & 1)) & 3) > k)
                            continue;

                        XDrawPoint(dp->drv.p->dpy, dp->drv.p->pixmap,
                                   dp->drv.p->gc,
                                   x * dp->drv.p->font_width + i,
                                   y * dp->drv.p->font_height + j);
                    }
                    break;
                }
                default:
                {
                    char ch;
                    ch = '?';
                    XDrawString(dp->drv.p->dpy, dp->drv.p->pixmap,
                                dp->drv.p->gc,
                                x * dp->drv.p->font_width, yoff, &ch, 1);
                    break;
                }
            }
        }
    }

    XCopyArea(dp->drv.p->dpy, dp->drv.p->pixmap, dp->drv.p->window,
              dp->drv.p->gc, 0, 0,
              dp->cv->width * dp->drv.p->font_width,
              dp->cv->height * dp->drv.p->font_height,
              0, 0);
    XFlush(dp->drv.p->dpy);
}

static void x11_handle_resize(caca_display_t *dp)
{
    Pixmap new_pixmap;

    new_pixmap = XCreatePixmap(dp->drv.p->dpy, dp->drv.p->window,
                               dp->resize.w * dp->drv.p->font_width,
                               dp->resize.h * dp->drv.p->font_height,
                               DefaultDepth(dp->drv.p->dpy,
                                            DefaultScreen(dp->drv.p->dpy)));
    XCopyArea(dp->drv.p->dpy, dp->drv.p->pixmap, new_pixmap,
              dp->drv.p->gc, 0, 0,
              dp->resize.w * dp->drv.p->font_width,
              dp->resize.h * dp->drv.p->font_height, 0, 0);
    XFreePixmap(dp->drv.p->dpy, dp->drv.p->pixmap);
    dp->drv.p->pixmap = new_pixmap;
}

static int x11_get_event(caca_display_t *dp, caca_event_t *ev)
{
    XEvent xevent;
    char key;

    while(XCheckWindowEvent(dp->drv.p->dpy, dp->drv.p->window,
                            dp->drv.p->event_mask, &xevent) == True)
    {
        KeySym keysym;

        /* Expose event */
        if(xevent.type == Expose)
        {
            XCopyArea(dp->drv.p->dpy, dp->drv.p->pixmap,
                      dp->drv.p->window, dp->drv.p->gc, 0, 0,
                      dp->cv->width * dp->drv.p->font_width,
                      dp->cv->height * dp->drv.p->font_height, 0, 0);
            continue;
        }

        /* Resize event */
        if(xevent.type == ConfigureNotify)
        {
            unsigned int w, h;

            w = (xevent.xconfigure.width + dp->drv.p->font_width / 3)
                  / dp->drv.p->font_width;
            h = (xevent.xconfigure.height + dp->drv.p->font_height / 3)
                  / dp->drv.p->font_height;

            if(!w || !h || (w == dp->cv->width && h == dp->cv->height))
                continue;

            dp->resize.w = w;
            dp->resize.h = h;
            dp->resize.resized = 1;

            continue;
        }

        /* Check for mouse motion events */
        if(xevent.type == MotionNotify)
        {
            unsigned int newx = xevent.xmotion.x / dp->drv.p->font_width;
            unsigned int newy = xevent.xmotion.y / dp->drv.p->font_height;

            if(newx >= dp->cv->width)
                newx = dp->cv->width - 1;
            if(newy >= dp->cv->height)
                newy = dp->cv->height - 1;

            if(dp->mouse.x == newx && dp->mouse.y == newy)
                continue;

            dp->mouse.x = newx;
            dp->mouse.y = newy;

            ev->type = CACA_EVENT_MOUSE_MOTION;
            ev->data.mouse.x = dp->mouse.x;
            ev->data.mouse.y = dp->mouse.y;
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

        keysym = XKeycodeToKeysym(dp->drv.p->dpy, xevent.xkey.keycode, 0);
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
            case XK_KP_Page_Up:
            case XK_Page_Up:      ev->data.key.ch = CACA_KEY_PAGEUP;   break;
            case XK_KP_Page_Down:
            case XK_Page_Down:    ev->data.key.ch = CACA_KEY_PAGEDOWN; break;
            case XK_KP_Home:
            case XK_Home:         ev->data.key.ch = CACA_KEY_HOME;     break;
            case XK_KP_End:
            case XK_End:          ev->data.key.ch = CACA_KEY_END;      break;

            default: ev->type = CACA_EVENT_NONE; return 0;
        }

        ev->data.key.ucs4 = 0;
        ev->data.key.utf8[0] = '\0';
        return 1;
    }

    while(XCheckTypedEvent(dp->drv.p->dpy, ClientMessage, &xevent))
    {
        if(xevent.xclient.message_type != dp->drv.p->wm_protocols)
            continue;

        if((Atom)xevent.xclient.data.l[0] == dp->drv.p->wm_delete_window)
        {
            ev->type = CACA_EVENT_QUIT;
            return 1;
        }
    }

    ev->type = CACA_EVENT_NONE;
    return 0;
}

static void x11_set_mouse(caca_display_t *dp, int flags)
{
    Cursor no_ptr;
    Pixmap bm_no;
    XColor black, dummy;
    Colormap colormap;
    static char const empty[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    if(flags)
    {
        XDefineCursor(dp->drv.p->dpy,dp->drv.p->window, 0);
        return;
    }

    colormap = DefaultColormap(dp->drv.p->dpy, DefaultScreen(dp->drv.p->dpy));
    if(!XAllocNamedColor(dp->drv.p->dpy, colormap, "black", &black, &dummy))
    {
        return;
    }
    bm_no = XCreateBitmapFromData(dp->drv.p->dpy, dp->drv.p->window,
                                  empty, 8, 8);
    no_ptr = XCreatePixmapCursor(dp->drv.p->dpy, bm_no, bm_no,
                                 &black, &black, 0, 0);
    XDefineCursor(dp->drv.p->dpy, dp->drv.p->window, no_ptr);
    XFreeCursor(dp->drv.p->dpy, no_ptr);
    if(bm_no != None)
        XFreePixmap(dp->drv.p->dpy, bm_no);
    XFreeColors(dp->drv.p->dpy, colormap, &black.pixel, 1, 0);

    XSync(dp->drv.p->dpy, False);
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

int x11_install(caca_display_t *dp)
{
#if defined(HAVE_GETENV)
    if(!getenv("DISPLAY") || !*(getenv("DISPLAY")))
        return -1;
#endif

    dp->drv.driver = CACA_DRIVER_X11;

    dp->drv.init_graphics = x11_init_graphics;
    dp->drv.end_graphics = x11_end_graphics;
    dp->drv.set_display_title = x11_set_display_title;
    dp->drv.get_display_width = x11_get_display_width;
    dp->drv.get_display_height = x11_get_display_height;
    dp->drv.display = x11_display;
    dp->drv.handle_resize = x11_handle_resize;
    dp->drv.get_event = x11_get_event;
    dp->drv.set_mouse = x11_set_mouse;

    return 0;
}

#endif /* USE_X11 */

