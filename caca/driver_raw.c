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
#include "common.h"

#if !defined(__KERNEL__)

#include <stdio.h>
#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

static int raw_init_graphics(caca_display_t *dp)
{
    unsigned int width = dp->cv->width, height = dp->cv->height;
    char const *geometry;

#if defined(HAVE_GETENV)
    geometry = getenv("CACA_GEOMETRY");
    if(geometry && *geometry)
        sscanf(geometry, "%ux%u", &width, &height);
#endif

    _cucul_set_canvas_size(dp->cv, width ? width : 80, height ? height : 24);

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

static unsigned int raw_get_display_width(caca_display_t *dp)
{
    return 0;
}

static unsigned int raw_get_display_height(caca_display_t *dp)
{
    return 0;
}

static void raw_display(caca_display_t *dp)
{
    void *buffer;
    unsigned long int len;

    buffer = cucul_export_memory(dp->cv, "caca", &len);
    if(!buffer)
        return;
    fwrite(buffer, len, 1, stdout);
    free(buffer);
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
    dp->drv.set_display_title = raw_set_display_title;
    dp->drv.get_display_width = raw_get_display_width;
    dp->drv.get_display_height = raw_get_display_height;
    dp->drv.display = raw_display;
    dp->drv.handle_resize = raw_handle_resize;
    dp->drv.get_event = raw_get_event;
    dp->drv.set_mouse = NULL;

    return 0;
}

#endif /* !__KERNEL__ */
