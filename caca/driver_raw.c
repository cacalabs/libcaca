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
    cucul_buffer_t *buffer;

    buffer = cucul_create_export(dp->cv, "caca");
    fwrite(cucul_get_buffer_data(buffer),
           cucul_get_buffer_size(buffer), 1, stdout);
    fflush(stdout);
    cucul_free_buffer(buffer);
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

