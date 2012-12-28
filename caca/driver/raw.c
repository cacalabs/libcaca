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
 *  This file contains the libcaca raw input and output driver
 */

#include "config.h"

#if !defined(__KERNEL__)

#include <stdio.h>
#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

static int raw_init_graphics(caca_display_t *dp)
{
    int width = caca_get_canvas_width(dp->cv);
    int height = caca_get_canvas_height(dp->cv);
    char const *geometry;

#if defined(HAVE_GETENV)
    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

    dp->resize.allow = 1;
    caca_set_canvas_size(dp->cv, width ? width : 80, height ? height : 24);
    dp->resize.allow = 0;

    return 0;
}

static int raw_end_graphics(caca_display_t *dp)
{
    return 0;
}

static int raw_set_display_title(caca_display_t *dp, char const *title)
{
    return -1;
}

static int raw_get_display_width(caca_display_t const *dp)
{
    return 0;
}

static int raw_get_display_height(caca_display_t const *dp)
{
    return 0;
}

static void raw_display(caca_display_t *dp)
{
    void *buffer;
    size_t len;

    buffer = caca_export_canvas_to_memory(dp->cv, "caca", &len);
    if(!buffer)
        return;
    fwrite(buffer, len, 1, stdout);
    fflush(stdout);
    free(buffer);
}

static void raw_handle_resize(caca_display_t *dp)
{
    ;
}

static int raw_get_event(caca_display_t *dp, caca_privevent_t *ev)
{
    ev->type = CACA_EVENT_NONE;
    return 0;
}

/*
 * Driver initialisation
 */

int raw_install(caca_display_t *dp)
{
    dp->drv.id = CACA_DRIVER_RAW;
    dp->drv.driver = "raw";

    dp->drv.init_graphics = raw_init_graphics;
    dp->drv.end_graphics = raw_end_graphics;
    dp->drv.set_display_title = raw_set_display_title;
    dp->drv.get_display_width = raw_get_display_width;
    dp->drv.get_display_height = raw_get_display_height;
    dp->drv.display = raw_display;
    dp->drv.handle_resize = raw_handle_resize;
    dp->drv.get_event = raw_get_event;
    dp->drv.set_mouse = NULL;
    dp->drv.set_cursor = NULL;

    return 0;
}

#endif /* !__KERNEL__ */
