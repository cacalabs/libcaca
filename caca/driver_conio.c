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

/** \file driver_conio.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief DOS/conio.h driver
 *
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

static int conio_init_graphics(caca_t *kk)
{
    kk->drv.p = malloc(sizeof(struct driver_private));

    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();

    gettextinfo(&kk->drv.p->ti);
    kk->drv.p->screen = malloc(2 * kk->drv.p->ti.screenwidth
                                 * kk->drv.p->ti.screenheight * sizeof(char));
    if(kk->drv.p->screen == NULL)
        return -1;
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenRetrieve(kk->drv.p->screen);
#   else
    /* FIXME */
#   endif
    _cucul_set_size(kk->qq, kk->drv.p->ti.screenwidth,
                            kk->drv.p->ti.screenheight);
    return 0;
}

static int conio_end_graphics(caca_t *kk)
{
    _wscroll = 1;
    textcolor((enum COLORS)WHITE);
    textbackground((enum COLORS)BLACK);
    gotoxy(kk->qq->width, kk->qq->height);
    cputs("\r\n");
    _setcursortype(_NORMALCURSOR);

    free(kk->drv.p->screen);
    free(kk->drv.p);

    return 0;
}

static int conio_set_window_title(caca_t *kk, char const *title)
{
    return 0;
}

static unsigned int conio_get_window_width(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->width * 6;
}

static unsigned int conio_get_window_height(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->height * 10;
}

static void conio_display(caca_t *kk)
{
    char *screen = kk->drv.p->screen;
    uint8_t *attr = kk->qq->attr;
    uint32_t *chars = kk->qq->chars;
    int n;

    for(n = kk->qq->height * kk->qq->width; n--; )
    {
        *screen++ = _cucul_utf32_to_cp437(*chars++);
        *screen++ = *attr++;
    }
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenUpdate(kk->drv.p->screen);
#   else
    /* FIXME */
#   endif
}

static void conio_handle_resize(caca_t *kk)
{
    /* We know nothing about our window */
    kk->resize.w = kk->qq->width;
    kk->resize.h = kk->qq->height;
}

static unsigned int conio_get_event(caca_t *kk)
{
    unsigned int event;

    if(!_conio_kbhit())
        return CACA_EVENT_NONE;

    event = getch();
    _push_event(kk, CACA_EVENT_KEY_RELEASE | event);
    return CACA_EVENT_KEY_PRESS | event;
}

/*
 * Driver initialisation
 */

void conio_init_driver(caca_t *kk)
{
    kk->drv.driver = CACA_DRIVER_CONIO;

    kk->drv.init_graphics = conio_init_graphics;
    kk->drv.end_graphics = conio_end_graphics;
    kk->drv.set_window_title = conio_set_window_title;
    kk->drv.get_window_width = conio_get_window_width;
    kk->drv.get_window_height = conio_get_window_height;
    kk->drv.display = conio_display;
    kk->drv.handle_resize = conio_handle_resize;
    kk->drv.get_event = conio_get_event;
}

#endif /* USE_CONIO */

