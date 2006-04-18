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
#include "cucul.h"
#include "cucul_internals.h"

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
    _cucul_set_size(dp->cv, dp->drv.p->ti.screenwidth,
                            dp->drv.p->ti.screenheight);
    return 0;
}

static int conio_end_graphics(caca_display_t *dp)
{
    _wscroll = 1;
    textcolor((enum COLORS)WHITE);
    textbackground((enum COLORS)BLACK);
    gotoxy(dp->cv->width, dp->cv->height);
    cputs("\r\n");
    _setcursortype(_NORMALCURSOR);

    free(dp->drv.p->screen);
    free(dp->drv.p);

    return 0;
}

static int conio_set_window_title(caca_display_t *dp, char const *title)
{
    return 0;
}

static unsigned int conio_get_window_width(caca_display_t *dp)
{
    /* Fallback to a 6x10 font */
    return dp->cv->width * 6;
}

static unsigned int conio_get_window_height(caca_display_t *dp)
{
    /* Fallback to a 6x10 font */
    return dp->cv->height * 10;
}

static void conio_display(caca_display_t *dp)
{
    char *screen = dp->drv.p->screen;
    uint32_t *attr = dp->cv->attr;
    uint32_t *chars = dp->cv->chars;
    int n;

    for(n = dp->cv->height * dp->cv->width; n--; )
    {
        *screen++ = _cucul_utf32_to_cp437(*chars++);
        *screen++ = _cucul_argb32_to_ansi8(*attr++);
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
    dp->resize.w = dp->cv->width;
    dp->resize.h = dp->cv->height;
}

static int conio_get_event(caca_display_t *dp, caca_event_t *ev)
{
    unsigned char ch;
    caca_event_t release;

    if(!_conio_kbhit())
    {
        ev->type = CACA_EVENT_NONE;
        return 0;
    }

    ch = getch();

    ev->type = CACA_EVENT_KEY_PRESS;
    ev->data.key.ch = ch;
    ev->data.key.ucs4 = (uint32_t)ch;
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
    dp->drv.driver = CACA_DRIVER_CONIO;

    dp->drv.init_graphics = conio_init_graphics;
    dp->drv.end_graphics = conio_end_graphics;
    dp->drv.set_window_title = conio_set_window_title;
    dp->drv.get_window_width = conio_get_window_width;
    dp->drv.get_window_height = conio_get_window_height;
    dp->drv.display = conio_display;
    dp->drv.handle_resize = conio_handle_resize;
    dp->drv.get_event = conio_get_event;
    dp->drv.set_mouse = NULL;

    return 0;
}

#endif /* USE_CONIO */

