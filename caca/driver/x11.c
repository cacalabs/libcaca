/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2014 Sam Hocevar <sam@hocevar.net>
 *                2007 Ben Wiley Sittler <bsittler@gmail.com>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the libcaca X11 input and output driver
 */

#include "config.h"

#if defined(USE_X11)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#if defined HAVE_X11_XKBLIB_H
#   include <X11/XKBlib.h>
#endif

#include <stdio.h> /* BUFSIZ */
#include <stdlib.h>
#include <string.h>
#if defined HAVE_LOCALE_H
#   include <locale.h>
#endif

#include "caca.h"
#include "caca_internals.h"

/*
 * Local functions
 */
static int x11_error_handler(Display *, XErrorEvent *);
static void x11_put_glyph(caca_display_t *, int, int, int, int, int,
                          uint32_t, uint32_t);

struct driver_private
{
    Display *dpy;
    Window window;
    Pixmap pixmap;
    GC gc;
    long int event_mask;
    int font_width, font_height;
    int colors[4096];
#if defined X_HAVE_UTF8_STRING
    XFontSet font_set;
#endif
    Font font;
    XFontStruct *font_struct;
    int font_offset;
    Cursor pointer;
    Atom wm_protocols;
    Atom wm_delete_window;
#if defined HAVE_X11_XKBLIB_H
    Bool autorepeat;
#endif
    uint32_t max_char;
    int cursor_flags;
    int dirty_cursor_x, dirty_cursor_y;
#if defined X_HAVE_UTF8_STRING
    XIM im;
    XIC ic;
#endif
};

#define UNICODE_XLFD_SUFFIX "-iso10646-1"
#define LATIN_1_XLFD_SUFFIX "-iso8859-1"

static int x11_init_graphics(caca_display_t *dp)
{
    Colormap colormap;
    XSetWindowAttributes x11_winattr;
#if defined X_HAVE_UTF8_STRING
    XVaNestedList list;
#endif
    int (*old_error_handler)(Display *, XErrorEvent *);
    char const *fonts[] = { NULL, "8x13bold", "fixed", NULL }, **parser;
    char const *geometry;
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
    int i;

    dp->drv.p = malloc(sizeof(struct driver_private));

#if defined HAVE_GETENV
    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

    caca_add_dirty_rect(dp->cv, 0, 0, dp->cv->width, dp->cv->height);
    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, width ? width : 80, height ? height : 32);
    width = caca_get_canvas_width(dp->cv);
    height = caca_get_canvas_height(dp->cv);
    dp->resize.allow = 0;

#if defined HAVE_LOCALE_H
    setlocale(LC_ALL, "");
#endif

    dp->drv.p->dpy = XOpenDisplay(NULL);
    if(dp->drv.p->dpy == NULL)
        return -1;

#if defined HAVE_GETENV
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
#if defined X_HAVE_UTF8_STRING
        char **missing_charset_list;
        char *def_string;
        int missing_charset_count;
#endif
        uint32_t font_max_char;

        if(!*parser)
        {
            XSetErrorHandler(old_error_handler);
            XCloseDisplay(dp->drv.p->dpy);
            return -1;
        }

#if defined X_HAVE_UTF8_STRING
        dp->drv.p->font_set = XCreateFontSet(dp->drv.p->dpy, *parser,
                                             &missing_charset_list,
                                             &missing_charset_count,
                                             &def_string);
        if (missing_charset_list)
            XFreeStringList(missing_charset_list);

        if (!dp->drv.p->font_set || missing_charset_count)
        {
            char buf[BUFSIZ];

            if (dp->drv.p->font_set)
                XFreeFontSet(dp->drv.p->dpy, dp->drv.p->font_set);
            snprintf(buf, BUFSIZ - 1, "%s,*", *parser);
            dp->drv.p->font_set = XCreateFontSet(dp->drv.p->dpy, buf,
                                                 &missing_charset_list,
                                                 &missing_charset_count,
                                                 &def_string);
            if (missing_charset_list)
                XFreeStringList(missing_charset_list);
        }

        if (dp->drv.p->font_set)
            break;
#endif

        dp->drv.p->font = XLoadFont(dp->drv.p->dpy, *parser);
        if(!dp->drv.p->font)
            continue;

        dp->drv.p->font_struct = XQueryFont(dp->drv.p->dpy, dp->drv.p->font);
        if(!dp->drv.p->font_struct)
        {
            XUnloadFont(dp->drv.p->dpy, dp->drv.p->font);
            continue;
        }

        if((strlen(*parser) > sizeof(UNICODE_XLFD_SUFFIX))
             && !strcasecmp(*parser + strlen(*parser)
                          - strlen(UNICODE_XLFD_SUFFIX), UNICODE_XLFD_SUFFIX))
            dp->drv.p->max_char = 0xffff;
        else if((strlen(*parser) > sizeof(LATIN_1_XLFD_SUFFIX))
                 && !strcasecmp(*parser + strlen(*parser)
                        - strlen(LATIN_1_XLFD_SUFFIX), LATIN_1_XLFD_SUFFIX))
            dp->drv.p->max_char = 0xff;
        else
            dp->drv.p->max_char = 0x7f;

        font_max_char =
            (dp->drv.p->font_struct->max_byte1 << 8)
             | dp->drv.p->font_struct->max_char_or_byte2;
        if(font_max_char && (font_max_char < dp->drv.p->max_char))
            dp->drv.p->max_char = font_max_char;

        break;
    }

    /* Reset the default X11 error handler */
    XSetErrorHandler(old_error_handler);

    /* Set font width to the largest character in the set */
#if defined X_HAVE_UTF8_STRING
    if (dp->drv.p->font_set)
    {
        /* Do not trust the fontset, because some fonts may have
         * irrelevantly large glyphs. Pango and rxvt use the same method. */
        static wchar_t const test[] =
        {
            '0', '1', '8', 'a', 'd', 'x', 'm', 'y', 'g', 'W', 'X', '\'', '_',
            0x00cd, 0x00e7, 0x00d5, 0x0114, 0x0177, /* Í ç Õ Ĕ ŷ */
            /* 0x0643, */ /* ﻙ */
            0x304c, 0x672c, /* が本 */
            0,
        };

        XRectangle ink, logical;

        dp->drv.p->font_width =
             (XmbTextEscapement(dp->drv.p->font_set, "CAca", 4) + 2) / 4;
        XwcTextExtents(dp->drv.p->font_set, test, sizeof(test) / sizeof(*test),
                       &ink, &logical);
        dp->drv.p->font_height = ink.height;
        dp->drv.p->font_offset = ink.height + ink.y;
    }
    else
#endif
    {
        dp->drv.p->font_width = 0;
        if(dp->drv.p->font_struct->per_char
            && !dp->drv.p->font_struct->min_byte1
            && dp->drv.p->font_struct->min_char_or_byte2 <= 0x21
            && dp->drv.p->font_struct->max_char_or_byte2 >= 0x7e)
        {
            for(i = 0x21; i < 0x7f; i++)
            {
                int cw = dp->drv.p->font_struct->per_char[i
                            - dp->drv.p->font_struct->min_char_or_byte2].width;
                if(cw > dp->drv.p->font_width)
                    dp->drv.p->font_width = cw;
            }
        }

        if(!dp->drv.p->font_width)
            dp->drv.p->font_width = dp->drv.p->font_struct->max_bounds.width;

        dp->drv.p->font_height = dp->drv.p->font_struct->max_bounds.ascent
                                  + dp->drv.p->font_struct->max_bounds.descent;
        dp->drv.p->font_offset = dp->drv.p->font_struct->max_bounds.descent;
    }

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
                      width * dp->drv.p->font_width,
                      height * dp->drv.p->font_height,
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
#if defined X_HAVE_UTF8_STRING
    if (!dp->drv.p->font_set)
#endif
        XSetFont(dp->drv.p->dpy, dp->drv.p->gc, dp->drv.p->font);

    for(;;)
    {
        XEvent xevent;
        XNextEvent(dp->drv.p->dpy, &xevent);
        if(xevent.type == MapNotify)
            break;
    }

#if defined HAVE_X11_XKBLIB_H
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
                                      width * dp->drv.p->font_width,
                                      height * dp->drv.p->font_height,
                                      DefaultDepth(dp->drv.p->dpy,
                                      DefaultScreen(dp->drv.p->dpy)));
    dp->drv.p->pointer = None;

    dp->drv.p->cursor_flags = 0;
    dp->drv.p->dirty_cursor_x = -1;
    dp->drv.p->dirty_cursor_y = -1;

#if defined X_HAVE_UTF8_STRING
    list = XVaCreateNestedList(0, XNFontSet, dp->drv.p->font_set, NULL);
    dp->drv.p->im = XOpenIM(dp->drv.p->dpy, NULL, NULL, NULL);
    dp->drv.p->ic = XCreateIC(dp->drv.p->im,
                          XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                          XNClientWindow, dp->drv.p->window,
                          XNPreeditAttributes, list,
                          XNStatusAttributes, list,
                          NULL);
#endif

    return 0;
}

static int x11_end_graphics(caca_display_t *dp)
{
    XSync(dp->drv.p->dpy, False);
#if defined HAVE_X11_XKBLIB_H
    if(!dp->drv.p->autorepeat)
        XAutoRepeatOn(dp->drv.p->dpy);
#endif
    XFreePixmap(dp->drv.p->dpy, dp->drv.p->pixmap);
#if defined X_HAVE_UTF8_STRING
    if (dp->drv.p->font_set)
        XFreeFontSet(dp->drv.p->dpy, dp->drv.p->font_set);
    else
#endif
        XFreeFont(dp->drv.p->dpy, dp->drv.p->font_struct);
    XFreeGC(dp->drv.p->dpy, dp->drv.p->gc);
    XUnmapWindow(dp->drv.p->dpy, dp->drv.p->window);
    XDestroyWindow(dp->drv.p->dpy, dp->drv.p->window);
#if defined X_HAVE_UTF8_STRING
    XDestroyIC(dp->drv.p->ic);
    XCloseIM(dp->drv.p->im);
#endif
    XCloseDisplay(dp->drv.p->dpy);

    free(dp->drv.p);

    return 0;
}

static int x11_set_display_title(caca_display_t *dp, char const *title)
{
    XStoreName(dp->drv.p->dpy, dp->drv.p->window, title);
    return 0;
}

static int x11_get_display_width(caca_display_t const *dp)
{
    return caca_get_canvas_width(dp->cv) * dp->drv.p->font_width;
}

static int x11_get_display_height(caca_display_t const *dp)
{
    return caca_get_canvas_height(dp->cv) * dp->drv.p->font_height;
}

static void x11_display(caca_display_t *dp)
{
    uint32_t const *cvchars = caca_get_canvas_chars(dp->cv);
    uint32_t const *cvattrs = caca_get_canvas_attrs(dp->cv);
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
    int x, y, i, len;

    /* XXX: the magic value -1 is used to handle the cursor area */
    for(i = -1; i < caca_get_dirty_rect_count(dp->cv); i++)
    {
        int dx, dy, dw, dh;

        /* Get the dirty rectangle coordinates, either from the previous
         * cursor position, or from the canvas's list. */
        if(i == -1)
        {
            if(dp->drv.p->dirty_cursor_x < 0 || dp->drv.p->dirty_cursor_y < 0
                || dp->drv.p->dirty_cursor_x >= width
                || dp->drv.p->dirty_cursor_y >= height)
                continue;

            dx = dp->drv.p->dirty_cursor_x;
            dy = dp->drv.p->dirty_cursor_y;
            dw = dh = 1;

            dp->drv.p->dirty_cursor_x = -1;
            dp->drv.p->dirty_cursor_y = -1;
        }
        else
        {
            caca_get_dirty_rect(dp->cv, i, &dx, &dy, &dw, &dh);
        }

        /* First draw the background colours. Splitting the process in two
         * loops like this is actually slightly faster. */
        for(y = dy; y < dy + dh; y++)
        {
            for(x = dx; x < dx + dw; x += len)
            {
                uint32_t const *attrs = cvattrs + x + y * width;
                uint16_t bg = caca_attr_to_rgb12_bg(*attrs);

                len = 1;
                while(x + len < dx + dw
                       && caca_attr_to_rgb12_bg(attrs[len]) == bg)
                    len++;

                XSetForeground(dp->drv.p->dpy, dp->drv.p->gc,
                               dp->drv.p->colors[bg]);
                XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap,
                               dp->drv.p->gc,
                               x * dp->drv.p->font_width,
                               y * dp->drv.p->font_height,
                               len * dp->drv.p->font_width,
                               dp->drv.p->font_height);
            }
        }

        /* Then print the foreground characters */
        for(y = dy; y < dy + dh; y++)
        {
            int yoff = (y + 1) * dp->drv.p->font_height
                                        - dp->drv.p->font_offset;
            uint32_t const *chars = cvchars + dx + y * width;
            uint32_t const *attrs = cvattrs + dx + y * width;

            for(x = dx; x < dx + dw; x++, chars++, attrs++)
            {
                XSetForeground(dp->drv.p->dpy, dp->drv.p->gc,
                           dp->drv.p->colors[caca_attr_to_rgb12_fg(*attrs)]);

                x11_put_glyph(dp, x * dp->drv.p->font_width,
                              y * dp->drv.p->font_height, yoff,
                              dp->drv.p->font_width, dp->drv.p->font_height,
                              *attrs, *chars);
            }
        }
    }

    /* Print the cursor if necessary. */
    if(dp->drv.p->cursor_flags)
    {
        XSetForeground(dp->drv.p->dpy, dp->drv.p->gc,
                       dp->drv.p->colors[0xfff]);
        x = caca_wherex(dp->cv);
        y = caca_wherey(dp->cv);
        XFillRectangle(dp->drv.p->dpy, dp->drv.p->pixmap, dp->drv.p->gc,
                       x * dp->drv.p->font_width, y * dp->drv.p->font_height,
                       dp->drv.p->font_width, dp->drv.p->font_height);

        /* Mark the area as dirty */
        dp->drv.p->dirty_cursor_x = x;
        dp->drv.p->dirty_cursor_y = y;
    }

    XCopyArea(dp->drv.p->dpy, dp->drv.p->pixmap, dp->drv.p->window,
              dp->drv.p->gc, 0, 0,
              width * dp->drv.p->font_width,
              height * dp->drv.p->font_height,
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

static int x11_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
    XEvent xevent;
    char key;

    while(XCheckWindowEvent(dp->drv.p->dpy, dp->drv.p->window,
                            dp->drv.p->event_mask, &xevent) == True)
    {
        KeySym keysym;
#if defined X_HAVE_UTF8_STRING
        int len;
#endif

        /* Expose event */
        if(xevent.type == Expose)
        {
            XCopyArea(dp->drv.p->dpy, dp->drv.p->pixmap,
                      dp->drv.p->window, dp->drv.p->gc, 0, 0,
                      width * dp->drv.p->font_width,
                      height * dp->drv.p->font_height, 0, 0);
            continue;
        }

        /* Resize event */
        if(xevent.type == ConfigureNotify)
        {
            int w, h;

            w = (xevent.xconfigure.width + dp->drv.p->font_width / 3)
                  / dp->drv.p->font_width;
            h = (xevent.xconfigure.height + dp->drv.p->font_height / 3)
                  / dp->drv.p->font_height;

            if(!w || !h || (w == width && h == height))
                continue;

            dp->resize.w = w;
            dp->resize.h = h;
            dp->resize.resized = 1;

            continue;
        }

        /* Check for mouse motion events */
        if(xevent.type == MotionNotify)
        {
            int newx = xevent.xmotion.x / dp->drv.p->font_width;
            int newy = xevent.xmotion.y / dp->drv.p->font_height;

            if(newx >= width)
                newx = width - 1;
            if(newy >= height)
                newy = height - 1;

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

        if(XFilterEvent(&xevent, None) == True)
            continue;

#if defined X_HAVE_UTF8_STRING
        len = Xutf8LookupString(dp->drv.p->ic, &xevent.xkey,
                                ev->data.key.utf8, 8, NULL, NULL);
        ev->data.key.utf8[len] = 0;
        if (len)
        {
            ev->data.key.utf32 = caca_utf8_to_utf32(ev->data.key.utf8, NULL);
            if(ev->data.key.utf32 <= 0xff)
                ev->data.key.ch = ev->data.key.utf32;
            else
                ev->data.key.ch = 0;
            return 1;
        }
#endif
        if(XLookupString(&xevent.xkey, &key, 1, NULL, NULL))
        {
            ev->data.key.ch = key;
            ev->data.key.utf32 = key;
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

        ev->data.key.utf32 = 0;
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

static void x11_set_cursor(caca_display_t *dp, int flags)
{
    dp->drv.p->cursor_flags = flags;
}

/*
 * XXX: following functions are local
 */

static int x11_error_handler(Display *dpy, XErrorEvent *xevent)
{
    /* Ignore the error */
    return 0;
}

static void x11_put_glyph(caca_display_t *dp, int x, int y, int yoff,
                          int w, int h, uint32_t attr, uint32_t ch)
{
    static uint8_t const udlr[] =
    {
        /* 0x2500 - 0x250f: ─ . │ . . . . . . . . . ┌ . . . */
        0x05, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,
        /* 0x2510 - 0x251f: ┐ . . . └ . . . ┘ . . . ├ . . . */
        0x14, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00,
        /* 0x2520 - 0x252f: . . . . ┤ . . . . . . . ┬ . . . */
        0x00, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
        /* 0x2530 - 0x253f: . . . . ┴ . . . . . . . ┼ . . . */
        0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x55, 0x00, 0x00, 0x00,
        /* 0x2540 - 0x254f: . . . . . . . . . . . . . . . . */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 0x2550 - 0x255f: ═ ║ ╒ ╓ ╔ ╕ ╖ ╗ ╘ ╙ ╚ ╛ ╜ ╝ ╞ ╟ */
        0x0a, 0xa0, 0x12, 0x21, 0x22, 0x18, 0x24, 0x28,
        0x42, 0x81, 0x82, 0x48, 0x84, 0x88, 0x52, 0xa1,
        /* 0x2560 - 0x256c: ╠ ╡ ╢ ╣ ╤ ╥ ╦ ╧ ╨ ╩ ╪ ╫ ╬ */
        0xa2, 0x58, 0xa4, 0xa8, 0x1a, 0x25, 0x2a, 0x4a,
        0x85, 0x8a, 0x5a, 0xa5, 0xaa,
    };

    Display *dpy = dp->drv.p->dpy;
    Pixmap px = dp->drv.p->pixmap;
    GC gc = dp->drv.p->gc;
    int fw;

    /* Underline */
    if(attr & CACA_UNDERLINE)
        XFillRectangle(dpy, px, gc, x, y + h - 1, w, 1);

    /* Skip spaces and magic stuff */
    if(ch <= 0x00000020)
        return;

    if(ch == CACA_MAGIC_FULLWIDTH)
        return;

    fw = w;
    if(caca_utf32_is_fullwidth(ch))
        fw *= 2;

    /* We want to be able to print a few special Unicode characters
     * such as the CP437 gradients and half blocks. For unknown
     * characters, print what caca_utf32_to_ascii() returns. */

    if(ch >= 0x2500 && ch <= 0x256c && udlr[ch - 0x2500])
    {
        uint16_t D = udlr[ch - 0x2500];

        if(D & 0x04)
            XFillRectangle(dpy, px, gc, x, y + h / 2, fw / 2 + 1, 1);

        if(D & 0x01)
            XFillRectangle(dpy, px, gc,
                           x + fw / 2, y + h / 2, (fw + 1) / 2, 1);

        if(D & 0x40)
            XFillRectangle(dpy, px, gc, x + fw / 2, y, 1, h / 2 + 1);

        if(D & 0x10)
            XFillRectangle(dpy, px, gc, x + fw / 2, y + h / 2, 1, (h + 1) / 2);

#define STEPIF(a,b) (D&(a)?-1:(D&(b))?1:0)

        if(D & 0x08)
        {
            XFillRectangle(dpy, px, gc, x, y - 1 + h / 2,
                           fw / 2 + 1 + STEPIF(0xc0,0x20), 1);
            XFillRectangle(dpy, px, gc, x, y + 1 + h / 2,
                           fw / 2 + 1 + STEPIF(0x30,0x80), 1);
        }

        if(D & 0x02)
        {
            XFillRectangle(dpy, px, gc, x - STEPIF(0xc0,0x20) + fw / 2,
                           y - 1 + h / 2, (fw + 1) / 2 + STEPIF(0xc0,0x20), 1);
            XFillRectangle(dpy, px, gc, x - STEPIF(0x30,0x80) + fw / 2,
                           y + 1 + h / 2, (fw + 1) / 2 + STEPIF(0x30,0x80), 1);
        }

        if(D & 0x80)
        {
            XFillRectangle(dpy, px, gc, x - 1 + fw / 2, y,
                           1, h / 2 + 1 + STEPIF(0x0c,0x02));
            XFillRectangle(dpy, px, gc, x + 1 + fw / 2, y,
                           1, h / 2 + 1 + STEPIF(0x03,0x08));
        }

        if(D & 0x20)
        {
            XFillRectangle(dpy, px, gc, x - 1 + fw / 2,
                           y - STEPIF(0x0c,0x02) + h / 2,
                           1, (h + 1) / 2 + STEPIF(0x0c,0x02));
            XFillRectangle(dpy, px, gc, x + 1 + fw / 2,
                           y - STEPIF(0x03,0x08) + h / 2,
                           1, (h + 1) / 2 + STEPIF(0x03,0x08));
        }

        return;
    }

    switch(ch)
    {
        case 0x000000b7: /* · */
        case 0x00002219: /* ∙ */
        case 0x000030fb: /* ・ */
            XFillRectangle(dpy, px, gc, x + fw / 2 - 1, y + h / 2 - 1, 2, 2);
            return;

        case 0x00002261: /* ≡ */
            XFillRectangle(dpy, px, gc, x + 1, y - 2 + h / 2, fw - 1, 1);
            XFillRectangle(dpy, px, gc, x + 1, y + h / 2, fw - 1, 1);
            XFillRectangle(dpy, px, gc, x + 1, y + 2 + h / 2, fw - 1, 1);
            return;

        case 0x00002580: /* ▀ */
            XFillRectangle(dpy, px, gc, x, y, fw, h / 2);
            return;

        case 0x00002584: /* ▄ */
            XFillRectangle(dpy, px, gc, x, y + h - h / 2, fw, h / 2);
            return;

        case 0x00002588: /* █ */
        case 0x000025ae: /* ▮ */
            XFillRectangle(dpy, px, gc, x, y, fw, h);
            return;

        case 0x0000258c: /* ▌ */
            XFillRectangle(dpy, px, gc, x, y, fw / 2, h);
            return;

        case 0x00002590: /* ▐ */
            XFillRectangle(dpy, px, gc, x + fw - fw / 2, y, fw / 2, h);
            return;

        case 0x000025a0: /* ■ */
        case 0x000025ac: /* ▬ */
            XFillRectangle(dpy, px, gc, x, y + h / 4, fw, h / 2);
            return;

        case 0x00002593: /* ▓ */
        case 0x00002592: /* ▒ */
        case 0x00002591: /* ░ */
        {
            /* FIXME: this sucks utterly */
            int i, j, k = ch - 0x00002591;
            for(j = h; j--; )
                for(i = fw; i--; )
            {
                if(((i + 2 * (j & 1)) & 3) > k)
                    continue;

                XDrawPoint(dpy, px, gc, x + i, y + j);
            }
            return;
        }

        case 0x000025cb: /* ○ */
        case 0x00002022: /* • */
        case 0x000025cf: /* ● */
        {
            int d, xo, yo;

            d = fw >> (~ch & 0x1); /* XXX: hack */
            if(h < fw)
                d = h;
            if(d < 1)
                d = 1;
            xo = (fw - d) / 2;
            yo = (h - d) / 2;
            if(ch == 0x000025cb)
                XDrawArc(dpy, px, gc, x + xo, y + yo, d, d, 0, 64 * 360);
            else
                XFillArc(dpy, px, gc, x + xo, y + yo, d, d, 0, 64 * 360);
            return;
        }
    }

#if defined X_HAVE_UTF8_STRING
    if (dp->drv.p->font_set)
    {
        wchar_t wch = ch;
        XwcDrawString(dpy, px, dp->drv.p->font_set, gc, x, yoff, &wch, 1);
    }
    else
#endif
    {
        XChar2b ch16;

#if !defined X_HAVE_UTF8_STRING
        if(ch > dp->drv.p->max_char)
        {
            ch16.byte1 = 0;
            ch16.byte2 = caca_utf32_to_ascii(ch);
        }
        else
#endif
        {
            ch16.byte1 = (uint8_t)(ch >> 8);
            ch16.byte2 = (uint8_t)ch;
        }

        XDrawString16(dpy, px, gc, x, yoff, &ch16, 1);
    }
}

/*
 * Driver initialisation
 */

int x11_install(caca_display_t *dp)
{
#if defined HAVE_GETENV
    if(!getenv("DISPLAY") || !*(getenv("DISPLAY")))
        return -1;
#endif

    dp->drv.id = CACA_DRIVER_X11;
    dp->drv.driver = "x11";

    dp->drv.init_graphics = x11_init_graphics;
    dp->drv.end_graphics = x11_end_graphics;
    dp->drv.set_display_title = x11_set_display_title;
    dp->drv.get_display_width = x11_get_display_width;
    dp->drv.get_display_height = x11_get_display_height;
    dp->drv.display = x11_display;
    dp->drv.handle_resize = x11_handle_resize;
    dp->drv.get_event = x11_get_event;
    dp->drv.set_mouse = x11_set_mouse;
    dp->drv.set_cursor = x11_set_cursor;

    return 0;
}

#endif /* USE_X11 */

