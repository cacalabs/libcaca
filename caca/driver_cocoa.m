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
 *  This file contains the libcaca Cocoa input and output driver
 */

#include "config.h"
#include "common.h"

#if defined USE_COCOA

#include <stdio.h>
#include <stdlib.h>

#include <Cocoa/Cocoa.h>

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

struct driver_private
{
    /* Just testing stuff */
    NSAutoreleasePool *pool;
};

static int cocoa_init_graphics(caca_display_t *dp)
{
    dp->drv.p = malloc(sizeof(struct driver_private));

    dp->drv.p->pool = [NSAutoreleasePool new];

    return 0;
}

static int cocoa_end_graphics(caca_display_t *dp)
{
    [dp->drv.p->pool release];

    free(dp->drv.p);

    return 0;
}

static int cocoa_set_display_title(caca_display_t *dp, char const *title)
{
    return -1;
}

static unsigned int cocoa_get_display_width(caca_display_t *dp)
{
    return 0;
}

static unsigned int cocoa_get_display_height(caca_display_t *dp)
{
    return 0;
}

static void cocoa_display(caca_display_t *dp)
{
    ;
}

static void cocoa_handle_resize(caca_display_t *dp)
{
    ;
}

static int cocoa_get_event(caca_display_t *dp, caca_event_t *ev)
{
    ev->type = CACA_EVENT_NONE;
    return 0;
}

/*
 * Driver initialisation
 */

int cocoa_install(caca_display_t *dp)
{
    dp->drv.driver = CACA_DRIVER_RAW;

    dp->drv.init_graphics = cocoa_init_graphics;
    dp->drv.end_graphics = cocoa_end_graphics;
    dp->drv.set_display_title = cocoa_set_display_title;
    dp->drv.get_display_width = cocoa_get_display_width;
    dp->drv.get_display_height = cocoa_get_display_height;
    dp->drv.display = cocoa_display;
    dp->drv.handle_resize = cocoa_handle_resize;
    dp->drv.get_event = cocoa_get_event;
    dp->drv.set_mouse = NULL;

    return 0;
}

#endif /* USE_COCOA */
