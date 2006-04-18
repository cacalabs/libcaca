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
 *  This file contains the libcaca raw input and output driver
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

static int raw_init_graphics(caca_display_t *dp)
{
    return 0;
}

static int raw_end_graphics(caca_display_t *dp)
{
    return 0;
}

static int raw_set_window_title(caca_display_t *dp, char const *title)
{
    return 0;
}

static unsigned int raw_get_window_width(caca_display_t *dp)
{
    return 0;
}

static unsigned int raw_get_window_height(caca_display_t *dp)
{
    return 0;
}

static void raw_display(caca_display_t *dp)
{
    uint32_t *attr = dp->cv->attr;
    uint32_t *chars = dp->cv->chars;
    uint32_t w, h;
    unsigned int n;

    w = dp->cv->width;
    h = dp->cv->height;

    fprintf(stdout, "CACA%c%c%c%c%c%c%c%c",
                    (w >> 24), (w >> 16) & 0xff, (w >> 8) & 0xff, w & 0xff,
                    (h >> 24), (h >> 16) & 0xff, (h >> 8) & 0xff, h & 0xff);

    for(n = dp->cv->height * dp->cv->width; n--; )
    {
        uint32_t ch = *chars++;
        uint32_t a = *attr++;

        fprintf(stdout, "%c%c%c%c%c%c%c%c",
                (ch >> 24), (ch >> 16) & 0xff, (ch >> 8) & 0xff, ch & 0xff,
                (a >> 24), (a >> 16) & 0xff, (a >> 8) & 0xff, a & 0xff);
    }

    fprintf(stdout, "ACAC");
    fflush(stdout);
}

static void raw_handle_resize(caca_display_t *dp)
{
    ;
}

static int raw_get_event(caca_display_t *dp, caca_event_t *ev)
{
    ev->type = CACA_EVENT_NONE;
    return 0;
}

/*
 * Driver initialisation
 */

int raw_install(caca_display_t *dp)
{
    dp->drv.driver = CACA_DRIVER_RAW;

    dp->drv.init_graphics = raw_init_graphics;
    dp->drv.end_graphics = raw_end_graphics;
    dp->drv.set_window_title = raw_set_window_title;
    dp->drv.get_window_width = raw_get_window_width;
    dp->drv.get_window_height = raw_get_window_height;
    dp->drv.display = raw_display;
    dp->drv.handle_resize = raw_handle_resize;
    dp->drv.get_event = raw_get_event;
    dp->drv.set_mouse = NULL;

    return 0;
}

