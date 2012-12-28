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
 *  This file contains a null libcaca input and output driver
 */

#include "config.h"

#if !defined(__KERNEL__)

#include <stdio.h>

#include "caca.h"
#include "caca_internals.h"

static int null_init_graphics(caca_display_t *dp)
{
    return 0;
}

static int null_end_graphics(caca_display_t *dp)
{
    return 0;
}

static int null_set_display_title(caca_display_t *dp, char const *title)
{
    return -1;
}

static int null_get_display_width(caca_display_t const *dp)
{
    return 0;
}

static int null_get_display_height(caca_display_t const *dp)
{
    return 0;
}

static void null_display(caca_display_t *dp)
{
    ;
}

static void null_handle_resize(caca_display_t *dp)
{
    ;
}

static int null_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    ev->type = CACA_EVENT_NONE;
    return 0;
}

/*
 * Driver initialisation
 */

int null_install(caca_display_t *dp)
{
    dp->drv.id = CACA_DRIVER_NULL;
    dp->drv.driver = "null";

    dp->drv.init_graphics = null_init_graphics;
    dp->drv.end_graphics = null_end_graphics;
    dp->drv.set_display_title = null_set_display_title;
    dp->drv.get_display_width = null_get_display_width;
    dp->drv.get_display_height = null_get_display_height;
    dp->drv.display = null_display;
    dp->drv.handle_resize = null_handle_resize;
    dp->drv.get_event = null_get_event;
    dp->drv.set_mouse = NULL;
    dp->drv.set_cursor = NULL;

    return 0;
}

#endif /* !__KERNEL__ */
