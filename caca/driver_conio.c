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

#include <string.h>
#include <stdlib.h>
#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif
#include <stdarg.h>

#if defined(HAVE_SYS_IOCTL_H)
#   include <sys/ioctl.h>
#endif

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

static int conio_init_graphics(caca_t *kk)
{
    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();

    gettextinfo(&kk->conio.ti);
    kk->conio.screen = malloc(2 * kk->conio.ti.screenwidth
                                * kk->conio.ti.screenheight * sizeof(char));
    if(kk->conio.screen == NULL)
        return -1;
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenRetrieve(kk->conio.screen);
#   else
    /* FIXME */
#   endif
    cucul_set_size(kk->qq, kk->conio.ti.screenwidth,
                           kk->conio.ti.screenheight);
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

    free(kk->conio.screen);

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
    int n;
    char *screen = kk->conio.screen;
    uint8_t *attr = kk->qq->attr;
    uint32_t *chars = kk->qq->chars;
    for(n = kk->qq->height * kk->qq->width; n--; )
    {
        *screen++ = *chars++ & 0x7f;
        *screen++ = *attr++;
    }
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenUpdate(kk->conio.screen);
#   else
    /* FIXME */
#   endif
}

static void conio_handle_resize(caca_t *kk, unsigned int *new_width,
                                            unsigned int *new_height)
{
    *new_width = kk->qq->width;
    *new_height = kk->qq->height;
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
    kk->driver.driver = CACA_DRIVER_CONIO;

    kk->driver.init_graphics = conio_init_graphics;
    kk->driver.end_graphics = conio_end_graphics;
    kk->driver.set_window_title = conio_set_window_title;
    kk->driver.get_window_width = conio_get_window_width;
    kk->driver.get_window_height = conio_get_window_height;
    kk->driver.display = conio_display;
    kk->driver.handle_resize = conio_handle_resize;
    kk->driver.get_event = conio_get_event;
}

#endif /* USE_CONIO */

