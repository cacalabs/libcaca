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

/** \file caca.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Main \e libcaca functions
 *
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

static int caca_init_driver(caca_t *kk);

/** \brief Attach a caca graphical context to a cucul backend context.
 *
 *  Create a graphical context using device-dependent features (ncurses for
 *  terminals, an X11 window, a DOS command window...) that attaches to a
 *  libcucul canvas. Everything that gets drawn in the libcucul canvas can
 *  then be displayed by the libcaca driver.
 *
 *  \param qq The cucul backend context.
 *  \return The caca graphical context or NULL if an error occurred.
 */
caca_t * caca_attach(cucul_t * qq)
{
    caca_t *kk = malloc(sizeof(caca_t));

    kk->qq = qq;

    if(caca_init_driver(kk))
    {
        free(kk);
        return NULL;
    }

    if(kk->drv.init_graphics(kk))
    {
        free(kk);
        return NULL;
    }

    /* Attached! */
    kk->qq->refcount++;

    /* Graphics stuff */
    kk->delay = 0;
    kk->rendertime = 0;

    /* Events stuff */
#if defined(USE_SLANG) || defined(USE_NCURSES)
    kk->events.key_timer.last_sec = 0;
    kk->events.key_timer.last_usec = 0;
    kk->events.last_key_ticks = 0;
    kk->events.autorepeat_ticks = 0;
    kk->events.last_key = 0;
#endif

    kk->timer.last_sec = 0;
    kk->timer.last_usec = 0;
    kk->lastticks = 0;

    /* Mouse position */
    kk->mouse.x = kk->qq->width / 2;
    kk->mouse.y = kk->qq->height / 2;

    /* Resize events */
    kk->resize.resized = 0;

    return kk;
}

/** \brief Detach a caca graphical context from a cucul backend context.
 *
 *  Detach a graphical context from its cucul backend and destroy it. The
 *  libcucul canvas continues to exist and other graphical contexts can be
 *  attached to it afterwards.
 *
 *  \param qq The caca graphical context.
 */
void caca_detach(caca_t *kk)
{
    kk->drv.end_graphics(kk);
    kk->qq->refcount--;
    free(kk);
}

/*
 * XXX: The following functions are local.
 */

static int caca_init_driver(caca_t *kk)
{
#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    char *var = getenv("CACA_DRIVER");

    /* If the environment variable was set, use it */
    if(var && *var)
    {
#if defined(USE_WIN32)
        if(!strcasecmp(var, "win32"))
            win32_init_driver(kk);
        else
#endif
#if defined(USE_CONIO)
        if(!strcasecmp(var, "conio"))
            conio_init_driver(kk);
        else
#endif
#if defined(USE_X11)
        if(!strcasecmp(var, "x11"))
            x11_init_driver(kk);
        else
#endif
#if defined(USE_GL)
        if(!strcasecmp(var, "gl"))
            gl_init_driver(kk);
        else
#endif
#if defined(USE_SLANG)
        if(!strcasecmp(var, "slang"))
            slang_init_driver(kk);
        else
#endif
#if defined(USE_NCURSES)
        if(!strcasecmp(var, "ncurses"))
            ncurses_init_driver(kk);
        else
#endif
#if defined(USE_NETWORK)
        if(!strcasecmp(var, "network"))
            network_init_driver(kk);
        else
#endif
#if defined(USE_VGA)
        if(!strcasecmp(var, "vga"))
            vga_init_driver(kk);
        else
#endif
            return -1;

        return 0;
    }
#endif

#if defined(USE_WIN32)
    win32_init_driver(kk);
    return 0;
#endif
#if defined(USE_CONIO)
    conio_init_driver(kk);
    return 0;
#endif
#if defined(USE_VGA)
    vga_init_driver(kk);
    return 0;
#endif
#if defined(USE_X11)
#if defined(HAVE_GETENV)
    if(getenv("DISPLAY") && *(getenv("DISPLAY")))
#endif
    {
        x11_init_driver(kk);
        return 0;
    }
#endif
#if defined(USE_GL)
#if defined(HAVE_GETENV) && defined(GLUT_XLIB_IMPLEMENTATION)
    if(getenv("DISPLAY") && *(getenv("DISPLAY")))
#endif
    {
        gl_init_driver(kk);
        return 0;
    }
#endif
#if defined(USE_SLANG)
    slang_init_driver(kk);
    return 0;
#endif
#if defined(USE_NCURSES)
    slang_init_driver(kk);
    return 0;
#endif
#if defined(USE_NETWORK)
    network_init_driver(kk);
    return 0;
#endif

    return -1;
}

