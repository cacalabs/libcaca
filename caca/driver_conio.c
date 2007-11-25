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

/*
 *  This file contains the libcaca DOS/conio.h input and output driver
 */

#include "config.h"
#include "common.h"

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
    dp->resize.allow = 1;
    cucul_set_canvas_size(dp->cv, dp->drv.p->ti.screenwidth,
                                  dp->drv.p->ti.screenheight);
    dp->resize.allow = 0;

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

static int conio_set_display_title(caca_display_t *dp, char const *title)
{
    return -1;
}

static unsigned int conio_get_display_width(caca_display_t const *dp)
{
    /* Fallback to a 6x10 font */
    return dp->cv->width * 6;
}

static unsigned int conio_get_display_height(caca_display_t const *dp)
{
    /* Fallback to a 6x10 font */
    return dp->cv->height * 10;
}

static void conio_display(caca_display_t *dp)
{
    char *screen = dp->drv.p->screen;
    uint32_t *attrs = dp->cv->attrs;
    uint32_t *chars = dp->cv->chars;
    unsigned int n;

    for(n = dp->cv->height * dp->cv->width; n--; )
    {
        char ch = cucul_utf32_to_cp437(*chars++);
        if(n && *chars == CUCUL_MAGIC_FULLWIDTH)
        {
            *screen++ = '[';
            *screen++ = cucul_attr_to_ansi(*attrs++);
            ch = ']';
            chars++;
            n--;
        }
        *screen++ = ch;
        *screen++ = cucul_attr_to_ansi(*attrs++);
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

static int conio_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    unsigned char ch;
    caca_privevent_t release;

    if(!_conio_kbhit())
    {
        ev->type = CACA_EVENT_NONE;
        return 0;
    }

    ch = getch();

    ev->type = CACA_EVENT_KEY_PRESS;
    ev->data.key.ch = ch;
    ev->data.key.utf32 = (uint32_t)ch;
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

