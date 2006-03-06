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

#if defined(HAVE_INTTYPES_H) || defined(_DOXYGEN_SKIP_ME)
#   include <inttypes.h>
#else
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#endif

#include <stdlib.h>
#include <string.h>

#include "cucul.h"
#include "cucul_internals.h"
#include "caca.h"
#include "caca_internals.h"

static int caca_init_driver(caca_t *kk);
static void caca_init_terminal(caca_t *kk);

caca_t * caca_attach(cucul_t * qq)
{
    int ret;
    caca_t *kk = malloc(sizeof(caca_t));

    ret = caca_init_driver(kk);

    if(ret)
    {
        free(kk);
        return NULL;
    }

    qq->refcount++;
    kk->qq = qq;

    /* Only for slang and ncurses */
    caca_init_terminal(kk);

    if(_caca_init_graphics(kk))
    {
        qq->refcount--;
        free(kk);
        return NULL;
    }

    /* Initialise events stuff */
    kk->events.key_timer.last_sec = 0;
    kk->events.key_timer.last_usec = 0;
    kk->events.last_key_ticks = 0;
    kk->events.autorepeat_ticks = 0;
    kk->events.last_key = 0;

    kk->timer.last_sec = 0;
    kk->timer.last_usec = 0;
    kk->lastticks = 0;

    kk->resize = 0;
    kk->resize_event = 0;

    return kk;
}

void caca_detach(caca_t *kk)
{
    kk->driver.end_graphics(kk);
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

    return -1;
}

static void caca_init_terminal(caca_t *kk)
{
#if defined(HAVE_GETENV) && defined(HAVE_PUTENV) && \
     (defined(USE_SLANG) || defined(USE_NCURSES))
    char *term, *colorterm, *other;
#endif

#if defined(USE_SLANG)
    if(kk->driver.driver != CACA_DRIVER_SLANG)
#endif
#if defined(USE_NCURSES)
    if(kk->driver.driver != CACA_DRIVER_NCURSES)
#endif
    return;

#if defined(HAVE_GETENV) && defined(HAVE_PUTENV) && \
     (defined(USE_SLANG) || defined(USE_NCURSES))
    term = getenv("TERM");
    colorterm = getenv("COLORTERM");

    if(term && !strcmp(term, "xterm"))
    {
        /* If we are using gnome-terminal, it's really a 16 colour terminal */
        if(colorterm && !strcmp(colorterm, "gnome-terminal"))
        {
#if defined(USE_NCURSES)
            if(kk->driver.driver == CACA_DRIVER_NCURSES)
            {
                SCREEN *screen;
                screen = newterm("xterm-16color", stdout, stdin);
                if(screen == NULL)
                    return;
                endwin();
            }
#endif
            (void)putenv("TERM=xterm-16color");
            return;
        }

        /* Ditto if we are using Konsole */
        other = getenv("KONSOLE_DCOP_SESSION");
        if(other)
        {
#if defined(USE_NCURSES)
            if(kk->driver.driver == CACA_DRIVER_NCURSES)
            {
                SCREEN *screen;
                screen = newterm("xterm-16color", stdout, stdin);
                if(screen == NULL)
                    return;
                endwin();
            }
#endif
            (void)putenv("TERM=xterm-16color");
            return;
        }
    }
#endif
}

