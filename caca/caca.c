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
    kk->events.last_key_event.type = CACA_EVENT_NONE;
#endif
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
    kk->events.queue = 0;
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
        if(!strcasecmp(var, "win32")) return win32_install(kk);
#endif
#if defined(USE_CONIO)
        if(!strcasecmp(var, "conio")) return conio_install(kk);
#endif
#if defined(USE_X11)
        if(!strcasecmp(var, "x11")) return x11_install(kk);
#endif
#if defined(USE_GL)
        if(!strcasecmp(var, "gl")) return gl_install(kk);
#endif
        if(!strcasecmp(var, "raw")) return raw_install(kk);
#if defined(USE_SLANG)
        if(!strcasecmp(var, "slang")) return slang_install(kk);
#endif
#if defined(USE_NCURSES)
        if(!strcasecmp(var, "ncurses")) return ncurses_install(kk);
#endif
#if defined(USE_VGA)
        if(!strcasecmp(var, "vga")) return vga_install(kk);
#endif
        return -1;
    }
#endif

#if defined(USE_WIN32)
    if(win32_install(kk) == 0) return 0;
#endif
#if defined(USE_CONIO)
    if(conio_install(kk) == 0) return 0;
#endif
#if defined(USE_VGA)
    if(vga_install(kk) == 0) return 0;
#endif
#if defined(USE_X11)
    if(x11_install(kk) == 0) return 0;
#endif
#if defined(USE_GL)
    if(gl_install(kk) == 0) return 0;
#endif
#if defined(USE_SLANG)
    if(slang_install(kk) == 0) return 0;
#endif
#if defined(USE_NCURSES)
    if(ncurses_install(kk) == 0) return 0;
#endif

    return -1;
}

