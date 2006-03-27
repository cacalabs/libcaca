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

/** \file driver_raw.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief raw driver
 *
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

static int raw_init_graphics(caca_t *kk)
{
    return 0;
}

static int raw_end_graphics(caca_t *kk)
{
    return 0;
}

static int raw_set_window_title(caca_t *kk, char const *title)
{
    return 0;
}

static unsigned int raw_get_window_width(caca_t *kk)
{
    return 0;
}

static unsigned int raw_get_window_height(caca_t *kk)
{
    return 0;
}

static void raw_display(caca_t *kk)
{
    uint8_t *attr = kk->qq->attr;
    uint32_t *chars = kk->qq->chars;
    int n;

    fprintf(stdout, "CACA %i %i\n", kk->qq->width, kk->qq->height);
    for(n = kk->qq->height * kk->qq->width; n--; )
    {
        uint32_t c = *chars++;
        uint8_t a = *attr++;

        fprintf(stdout, "%c%c%c%c %c", (c >> 24), (c >> 16) & 0xff,
                                       (c >> 8) & 0xff, c & 0xff, a);
    }
    fprintf(stdout, "ACAC\n");
    fflush(stdout);
}

static void raw_handle_resize(caca_t *kk)
{
    ;
}

static int raw_get_event(caca_t *kk, struct caca_event *ev)
{
    ev->type = CACA_EVENT_NONE;
    return 0;
}

/*
 * Driver initialisation
 */

int raw_install(caca_t *kk)
{
    kk->drv.driver = CACA_DRIVER_RAW;

    kk->drv.init_graphics = raw_init_graphics;
    kk->drv.end_graphics = raw_end_graphics;
    kk->drv.set_window_title = raw_set_window_title;
    kk->drv.get_window_width = raw_get_window_width;
    kk->drv.get_window_height = raw_get_window_height;
    kk->drv.display = raw_display;
    kk->drv.handle_resize = raw_handle_resize;
    kk->drv.get_event = raw_get_event;
    kk->drv.set_mouse = NULL;

    return 0;
}

