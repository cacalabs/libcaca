/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains the libcaca DOS/conio.h input and output driver
 */

#include "config.h"

#if defined(USE_CONIO)

#include <dos.h>
#include <conio.h>
#if defined(SCREENUPDATE_IN_PC_H)
#   include <pc.h>
#endif

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

struct driver_private
{
    struct text_info ti;
    char *screen;
};

static int conio_init_graphics(caca_display_t *dp)
{
    dp->drv.p = malloc(sizeof(struct driver_private));

    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();

    gettextinfo(&dp->drv.p->ti);
    dp->drv.p->screen = malloc(2 * dp->drv.p->ti.screenwidth
                                 * dp->drv.p->ti.screenheight * sizeof(char));
    if(dp->drv.p->screen == NULL)
        return -1;
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenRetrieve(dp->drv.p->screen);
#   else
    /* FIXME */
#   endif
    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, dp->drv.p->ti.screenwidth,
                                  dp->drv.p->ti.screenheight);
    dp->resize.allow = 0;

    return 0;
}

static int conio_end_graphics(caca_display_t *dp)
{
    _wscroll = 1;
    textcolor((enum COLORS)WHITE);
    textbackground((enum COLORS)BLACK);
    gotoxy(caca_get_canvas_width(dp->cv), caca_get_canvas_height(dp->cv));
    cputs("\r\n");
    _setcursortype(_NORMALCURSOR);

    free(dp->drv.p->screen);
    free(dp->drv.p);

    return 0;
}

static int conio_set_display_title(caca_display_t *dp, char const *title)
{
    return -1;
}

static int conio_get_display_width(caca_display_t const *dp)
{
    /* Fallback to a 6x10 font */
    return caca_get_canvas_width(dp->cv) * 6;
}

static int conio_get_display_height(caca_display_t const *dp)
{
    /* Fallback to a 6x10 font */
    return caca_get_canvas_height(dp->cv) * 10;
}

static void conio_display(caca_display_t *dp)
{
    char *screen = dp->drv.p->screen;
    uint32_t const *chars = caca_get_canvas_chars(dp->cv);
    uint32_t const *attrs = caca_get_canvas_attrs(dp->cv);
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
    int n;

    for(n = height * width; n--; )
    {
        char ch = caca_utf32_to_cp437(*chars++);
        if(n && *chars == CACA_MAGIC_FULLWIDTH)
        {
            *screen++ = '[';
            *screen++ = caca_attr_to_ansi(*attrs++);
            ch = ']';
            chars++;
            n--;
        }
        *screen++ = ch;
        *screen++ = caca_attr_to_ansi(*attrs++);
    }
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenUpdate(dp->drv.p->screen);
#   else
    /* FIXME */
#   endif
}

static void conio_handle_resize(caca_display_t *dp)
{
    /* We know nothing about our window */
    dp->resize.w = caca_get_canvas_width(dp->cv);
    dp->resize.h = caca_get_canvas_height(dp->cv);
}

static int conio_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    uint8_t ch;
    caca_privevent_t release;

    if(!_conio_kbhit())
    {
        ev->type = CACA_EVENT_NONE;
        return 0;
    }

    ch = getch();

    ev->type = CACA_EVENT_KEY_PRESS;
    ev->data.key.ch = ch;
    ev->data.key.utf32 = ch;
    ev->data.key.utf8[0] = ch;
    ev->data.key.utf8[1] = '\0';

    release = *ev;
    release.type = CACA_EVENT_KEY_RELEASE;
    _push_event(dp, &release);

    return 1;
}

/*
 * Driver initialisation
 */

int conio_install(caca_display_t *dp)
{
    dp->drv.id = CACA_DRIVER_CONIO;
    dp->drv.driver = "conio";

    dp->drv.init_graphics = conio_init_graphics;
    dp->drv.end_graphics = conio_end_graphics;
    dp->drv.set_display_title = conio_set_display_title;
    dp->drv.get_display_width = conio_get_display_width;
    dp->drv.get_display_height = conio_get_display_height;
    dp->drv.display = conio_display;
    dp->drv.handle_resize = conio_handle_resize;
    dp->drv.get_event = conio_get_event;
    dp->drv.set_mouse = NULL;
    dp->drv.set_cursor = NULL;

    return 0;
}

#endif /* USE_CONIO */

