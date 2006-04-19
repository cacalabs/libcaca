/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
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
 *  This file contains the main functions used by \e libcaca applications to
 *  initialise the library, get the screen properties, set the framerate and
 *  so on.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"
#include "caca.h"
#include "caca_internals.h"

static int caca_init_driver(caca_display_t *dp);

/** \brief Attach a caca graphical context to a cucul canvas.
 *
 *  Create a graphical context using device-dependent features (ncurses for
 *  terminals, an X11 window, a DOS command window...) that attaches to a
 *  libcucul canvas. Everything that gets drawn in the libcucul canvas can
 *  then be displayed by the libcaca driver.
 *
 *  \param cv The cucul cavas.
 *  \return The caca graphical context or NULL if an error occurred.
 */
caca_display_t * caca_create_display(cucul_canvas_t * cv)
{
    caca_display_t *dp = malloc(sizeof(caca_display_t));

    dp->cv = cv;

    if(caca_init_driver(dp))
    {
        free(dp);
        return NULL;
    }

    if(dp->drv.init_graphics(dp))
    {
        free(dp);
        return NULL;
    }

    /* Attached! */
    dp->cv->refcount++;

    /* Graphics stuff */
    dp->delay = 0;
    dp->rendertime = 0;

    /* Events stuff */
#if defined(USE_SLANG) || defined(USE_NCURSES)
    dp->events.key_timer.last_sec = 0;
    dp->events.key_timer.last_usec = 0;
    dp->events.last_key_ticks = 0;
    dp->events.autorepeat_ticks = 0;
    dp->events.last_key_event.type = CACA_EVENT_NONE;
#endif
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
    dp->events.queue = 0;
#endif

    dp->timer.last_sec = 0;
    dp->timer.last_usec = 0;
    dp->lastticks = 0;

    /* Mouse position */
    dp->mouse.x = dp->cv->width / 2;
    dp->mouse.y = dp->cv->height / 2;

    /* Resize events */
    dp->resize.resized = 0;

    return dp;
}

/** \brief Detach a caca graphical context from a cucul backend context.
 *
 *  Detach a graphical context from its cucul backend and destroy it. The
 *  libcucul canvas continues to exist and other graphical contexts can be
 *  attached to it afterwards.
 *
 *  \param dp The libcaca graphical context.
 */
void caca_free_display(caca_display_t *dp)
{
    dp->drv.end_graphics(dp);
    dp->cv->refcount--;
    free(dp);
}

/*
 * XXX: The following functions are local.
 */

static int caca_init_driver(caca_display_t *dp)
{
#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    char *var = getenv("CACA_DRIVER");
    
    /* If the environment variable was set, use it */
    if(var && *var)
    {
#if defined(USE_WIN32)
        if(!strcasecmp(var, "win32")) return win32_install(dp);
#endif
#if defined(USE_CONIO)
        if(!strcasecmp(var, "conio")) return conio_install(dp);
#endif
#if defined(USE_X11)
        if(!strcasecmp(var, "x11")) return x11_install(dp);
#endif
#if defined(USE_GL)
        if(!strcasecmp(var, "gl")) return gl_install(dp);
#endif
        if(!strcasecmp(var, "raw")) return raw_install(dp);
#if defined(USE_SLANG)
        if(!strcasecmp(var, "slang")) return slang_install(dp);
#endif
#if defined(USE_NCURSES)
        if(!strcasecmp(var, "ncurses")) return ncurses_install(dp);
#endif
#if defined(USE_VGA)
        if(!strcasecmp(var, "vga")) return vga_install(dp);
#endif
        return -1;
    }
#endif

#if defined(USE_WIN32)
    if(win32_install(dp) == 0) return 0;
#endif
#if defined(USE_CONIO)
    if(conio_install(dp) == 0) return 0;
#endif
#if defined(USE_VGA)
    if(vga_install(dp) == 0) return 0;
#endif
#if defined(USE_X11)
    if(x11_install(dp) == 0) return 0;
#endif
#if defined(USE_GL)
    if(gl_install(dp) == 0) return 0;
#endif
#if defined(USE_SLANG)
    if(slang_install(dp) == 0) return 0;
#endif
#if defined(USE_NCURSES)
    if(ncurses_install(dp) == 0) return 0;
#endif

    return -1;
}

