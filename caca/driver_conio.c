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

#if !defined(_DOXYGEN_SKIP_ME)
int conio_init_graphics(caca_t *kk)
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

int conio_end_graphics(caca_t *kk)
{
    _wscroll = 1;
    textcolor((enum COLORS)WHITE);
    textbackground((enum COLORS)BLACK);
    gotoxy(_caca_width, _caca_height);
    cputs("\r\n");
    _setcursortype(_NORMALCURSOR);

    free(kk->conio.screen);

    return 0;
}
#endif /* _DOXYGEN_SKIP_ME */

int conio_set_window_title(caca_t *kk, char const *title)
{
    return 0;
}

unsigned int conio_get_window_width(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->width * 6;
}

unsigned int conio_get_window_height(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->height * 10;
}

void conio_display(caca_t *kk)
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

void conio_handle_resize(caca_t *kk)
{
    return;
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
}

#endif /* USE_CONIO */

