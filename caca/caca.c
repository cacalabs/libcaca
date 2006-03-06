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

#if defined(USE_SLANG)
#   if defined(HAVE_SLANG_SLANG_H)
#       include <slang/slang.h>
#   else
#       include <slang.h>
#   endif
#endif
#if defined(USE_NCURSES)
#   if defined(HAVE_NCURSES_H)
#       include <ncurses.h>
#   else
#       include <curses.h>
#   endif
#endif
#if defined(USE_CONIO)
#   include <dos.h>
#   include <conio.h>
#endif
#if defined(USE_X11)
#   include <X11/Xlib.h>
#endif
#if defined(USE_WIN32)
#   include <windows.h>
#endif
#if defined(USE_GL)
#   include <GL/gl.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "cucul.h"
#include "cucul_internals.h"
#include "caca.h"
#include "caca_internals.h"

static void caca_init_driver(caca_t *kk);
static void caca_init_terminal(caca_t *kk);

#if defined(USE_NCURSES)
static mmask_t oldmask;
#endif

#if defined(USE_WIN32)
static CONSOLE_CURSOR_INFO cci;
#endif

caca_t * caca_attach(cucul_t * qq)
{
    caca_t *kk = malloc(sizeof(caca_t));

#if defined(USE_NCURSES)
    mmask_t newmask;
#endif

    caca_init_driver(kk);

    if(kk->driver == CACA_DRIVER_NONE)
        return NULL;

    caca_init_terminal(kk);

#if defined(USE_SLANG)
    if(kk->driver == CACA_DRIVER_SLANG)
    {
        /* Initialise slang library */
        SLsig_block_signals();
        SLtt_get_terminfo();

        if(SLkp_init() == -1)
        {
            SLsig_unblock_signals();
            return NULL;
        }

        SLang_init_tty(-1, 0, 1);

        if(SLsmg_init_smg() == -1)
        {
            SLsig_unblock_signals();
            return NULL;
        }

        SLsig_unblock_signals();

        SLsmg_cls();
        SLtt_set_cursor_visibility(0);
        SLkp_define_keysym("\e[M", 1001);
        SLtt_set_mouse_mode(1, 0);
        SLsmg_refresh();

        /* Disable scrolling so that hashmap scrolling optimization code
         * does not cause ugly refreshes due to slow terminals */
        SLtt_Term_Cannot_Scroll = 1;
    }
    else
#endif
#if defined(USE_NCURSES)
    if(kk->driver == CACA_DRIVER_NCURSES)
    {
        initscr();
        keypad(stdscr, TRUE);
        nonl();
        raw();
        noecho();
        nodelay(stdscr, TRUE);
        curs_set(0);

        /* Activate mouse */
        newmask = REPORT_MOUSE_POSITION | ALL_MOUSE_EVENTS;
        mousemask(newmask, &oldmask);
        mouseinterval(-1); /* No click emulation */

        /* Set the escape delay to a ridiculously low value */
        ESCDELAY = 10;
    }
    else
#endif
#if defined(USE_CONIO)
    if(kk->driver == CACA_DRIVER_CONIO)
    {
        _wscroll = 0;
        _setcursortype(_NOCURSOR);
        clrscr();
    }
    else
#endif
#if defined(USE_X11)
    if(kk->driver == CACA_DRIVER_X11)
    {
        /* Nothing to do */
        kk->x11.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask
              | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask
              | ExposureMask;
    }
    else
#endif
#if defined(USE_WIN32)
    if(kk->driver == CACA_DRIVER_WIN32)
    {
        /* This call is allowed to fail in case we already have a console */
        AllocConsole();

        kk->win32.hin = GetStdHandle(STD_INPUT_HANDLE);
        kk->win32.hout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if(kk->win32.hout == INVALID_HANDLE_VALUE)
            return NULL;

        GetConsoleCursorInfo(kk->win32.hout, &cci);
        cci.bVisible = FALSE;
        SetConsoleCursorInfo(kk->win32.hout, &cci);

        SetConsoleMode(kk->win32.hout, ENABLE_MOUSE_INPUT);
    }
    else
#endif
#if defined(USE_GL)
    if(kk->driver == CACA_DRIVER_GL)
    {
        /* Nothing to do */
    }
    else
#endif
    {
        /* Dummy */
    }

    /* Initialise events stuff */
    kk->events.key_timer.last_sec = 0;
    kk->events.key_timer.last_usec = 0;
    kk->events.last_key_ticks = 0;
    kk->events.autorepeat_ticks = 0;
    kk->events.last_key = 0;

    qq->refcount++;
    kk->qq = qq;

    kk->timer.last_sec = 0;
    kk->timer.last_usec = 0;
    kk->lastticks = 0;

    kk->resize = 0;
    kk->resize_event = 0;

    if(_caca_init_graphics(kk))
        return NULL;

    return kk;
}

void caca_detach(caca_t *kk)
{
    _caca_end_graphics(kk);

#if defined(USE_SLANG)
    if(kk->driver == CACA_DRIVER_SLANG)
    {
        SLtt_set_mouse_mode(0, 0);
        SLtt_set_cursor_visibility(1);
        SLang_reset_tty();
        SLsmg_reset_smg();
    }
    else
#endif
#if defined(USE_NCURSES)
    if(kk->driver == CACA_DRIVER_NCURSES)
    {
        mousemask(oldmask, NULL);
        curs_set(1);
        noraw();
        endwin();
    }
    else
#endif
#if defined(USE_CONIO)
    if(kk->driver == CACA_DRIVER_CONIO)
    {
        _wscroll = 1;
        textcolor((enum COLORS)WHITE);
        textbackground((enum COLORS)BLACK);
        gotoxy(_caca_width, _caca_height);
        cputs("\r\n");
        _setcursortype(_NORMALCURSOR);
    }
    else
#endif
#if defined(USE_X11)
    if(kk->driver == CACA_DRIVER_X11)
    {
        /* Nothing to do */
    }
    else
#endif
#if defined(USE_WIN32)
    if(kk->driver == CACA_DRIVER_WIN32)
    {
        SetConsoleTextAttribute(kk->win32.hout, FOREGROUND_INTENSITY
                                                 | FOREGROUND_RED
                                                 | FOREGROUND_GREEN
                                                 | FOREGROUND_BLUE);
        cci.bVisible = TRUE;
        SetConsoleCursorInfo(kk->win32.hout, &cci);
        CloseHandle(kk->win32.hout);
    }
    else
#endif
#if defined(USE_GL)
    if(kk->driver == CACA_DRIVER_GL)
    {
        /* Nothing to do */
    }
    else
#endif
    {
        /* Dummy */
    }

    kk->qq->refcount--;

    free(kk);
}

/*
 * XXX: The following functions are local.
 */

static void caca_init_driver(caca_t *kk)
{
#if defined(HAVE_GETENV) && defined(HAVE_STRCASECMP)
    char *var = getenv("CACA_DRIVER");

    /* If the environment variable was set, use it */
    if(var && *var)
    {
#if defined(USE_WIN32)
        if(!strcasecmp(var, "win32"))
            kk->driver = CACA_DRIVER_WIN32;
        else
#endif
#if defined(USE_CONIO)
        if(!strcasecmp(var, "conio"))
            kk->driver = CACA_DRIVER_CONIO;
        else
#endif
#if defined(USE_X11)
        if(!strcasecmp(var, "x11"))
            kk->driver = CACA_DRIVER_X11;
        else
#endif
#if defined(USE_GL)
        if(!strcasecmp(var, "gl"))
            kk->driver = CACA_DRIVER_GL;
        else
#endif
#if defined(USE_SLANG)
        if(!strcasecmp(var, "slang"))
            kk->driver = CACA_DRIVER_SLANG;
        else
#endif
#if defined(USE_NCURSES)
        if(!strcasecmp(var, "ncurses"))
            kk->driver = CACA_DRIVER_NCURSES;
        else
#endif

            kk->driver = CACA_DRIVER_NONE;

        return;
    }
#endif

#if defined(USE_WIN32)
    kk->driver = CACA_DRIVER_WIN32;
    return;
#endif
#if defined(USE_CONIO)
    kk->driver = CACA_DRIVER_CONIO;
    return;
#endif
#if defined(USE_X11)
#if defined(HAVE_GETENV)
    if(getenv("DISPLAY") && *(getenv("DISPLAY")))
#endif
    {
        kk->driver = CACA_DRIVER_X11;
        return;
    }
#endif
#if defined(USE_GL)
#if defined(HAVE_GETENV) && defined(GLUT_XLIB_IMPLEMENTATION)
    if(getenv("DISPLAY") && *(getenv("DISPLAY")))
#endif
    {
        kk->driver = CACA_DRIVER_GL;
        return;
    }
#endif
#if defined(USE_SLANG)
    kk->driver = CACA_DRIVER_SLANG;
    return;
#endif
#if defined(USE_NCURSES)
    kk->driver = CACA_DRIVER_NCURSES;
    return;
#endif

    kk->driver = CACA_DRIVER_NONE;
    return;
}

static void caca_init_terminal(caca_t *kk)
{
#if defined(HAVE_GETENV) && defined(HAVE_PUTENV) && \
     (defined(USE_SLANG) || defined(USE_NCURSES))
    char *term, *colorterm, *other;
#endif

#if defined(USE_SLANG)
    if(kk->driver != CACA_DRIVER_SLANG)
#endif
#if defined(USE_NCURSES)
    if(kk->driver != CACA_DRIVER_NCURSES)
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
            if(kk->driver == CACA_DRIVER_NCURSES)
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
            if(kk->driver == CACA_DRIVER_NCURSES)
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

