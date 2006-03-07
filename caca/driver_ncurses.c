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

/** \file driver_ncurses.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Ncurses driver
 *
 *  This file contains the libcaca Ncurses input and output driver
 */

#include "config.h"

#if defined(USE_NCURSES)

#if defined(HAVE_NCURSES_H)
#   include <ncurses.h>
#else
#   include <curses.h>
#endif

#include <stdio.h> /* BUFSIZ */
#include <string.h>
#include <stdlib.h>
#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif
#include <stdarg.h>

#if defined(HAVE_SIGNAL_H)
#   include <signal.h>
#endif
#if defined(HAVE_SYS_IOCTL_H)
#   include <sys/ioctl.h>
#endif

#include "caca.h"
#include "caca_internals.h"
#include "cucul.h"
#include "cucul_internals.h"

/*
 * Local functions
 */

#if defined(HAVE_SIGNAL)
static RETSIGTYPE sigwinch_handler(int);
static caca_t *sigwinch_kk; /* FIXME: we ought to get rid of this */
#endif

static int ncurses_init_graphics(caca_t *kk)
{
    static int curses_colors[] =
    {
        /* Standard curses colours */
        COLOR_BLACK,
        COLOR_BLUE,
        COLOR_GREEN,
        COLOR_CYAN,
        COLOR_RED,
        COLOR_MAGENTA,
        COLOR_YELLOW,
        COLOR_WHITE,
        /* Extra values for xterm-16color */
        COLOR_BLACK + 8,
        COLOR_BLUE + 8,
        COLOR_GREEN + 8,
        COLOR_CYAN + 8,
        COLOR_RED + 8,
        COLOR_MAGENTA + 8,
        COLOR_YELLOW + 8,
        COLOR_WHITE + 8
    };

    mmask_t newmask;
    int fg, bg, max;

#if defined(HAVE_SIGNAL)
    sigwinch_kk = kk;
    signal(SIGWINCH, sigwinch_handler);
#endif

    initscr();
    keypad(stdscr, TRUE);
    nonl();
    raw();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    /* Activate mouse */
    newmask = REPORT_MOUSE_POSITION | ALL_MOUSE_EVENTS;
    mousemask(newmask, &kk->ncurses.oldmask);
    mouseinterval(-1); /* No click emulation */

    /* Set the escape delay to a ridiculously low value */
    ESCDELAY = 10;

    /* Activate colour */
    start_color();

    /* If COLORS == 16, it means the terminal supports full bright colours
     * using setab and setaf (will use \e[90m \e[91m etc. for colours >= 8),
     * we can build 16*16 colour pairs.
     * If COLORS == 8, it means the terminal does not know about bright
     * colours and we need to get them through A_BOLD and A_BLINK (\e[1m
     * and \e[5m). We can only build 8*8 colour pairs. */
    max = COLORS >= 16 ? 16 : 8;

    for(bg = 0; bg < max; bg++)
        for(fg = 0; fg < max; fg++)
        {
            /* Use ((max + 7 - fg) % max) instead of fg so that colour 0
             * is light gray on black. Some terminals don't like this
             * colour pair to be redefined. */
            int col = ((max + 7 - fg) % max) + max * bg;
            init_pair(col, curses_colors[fg], curses_colors[bg]);
            kk->ncurses.attr[fg + 16 * bg] = COLOR_PAIR(col);

            if(max == 8)
            {
                /* Bright fg on simple bg */
                kk->ncurses.attr[fg + 8 + 16 * bg] = A_BOLD | COLOR_PAIR(col);
                /* Simple fg on bright bg */
                kk->ncurses.attr[fg + 16 * (bg + 8)] = A_BLINK
                                                    | COLOR_PAIR(col);
                /* Bright fg on bright bg */
                kk->ncurses.attr[fg + 8 + 16 * (bg + 8)] = A_BLINK | A_BOLD
                                                        | COLOR_PAIR(col);
            }
        }

    cucul_set_size(kk->qq, COLS, LINES);

    return 0;
}

static int ncurses_end_graphics(caca_t *kk)
{
    mousemask(kk->ncurses.oldmask, NULL);
    curs_set(1);
    noraw();
    endwin();

    return 0;
}

static int ncurses_set_window_title(caca_t *kk, char const *title)
{
    return 0;
}

static unsigned int ncurses_get_window_width(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->width * 6;
}

static unsigned int ncurses_get_window_height(caca_t *kk)
{
    /* Fallback to a 6x10 font */
    return kk->qq->height * 10;
}

static void ncurses_display(caca_t *kk)
{
    int x, y;
    uint8_t *attr = kk->qq->attr;
    uint32_t *chars = kk->qq->chars;
    for(y = 0; y < (int)kk->qq->height; y++)
    {
        move(y, 0);
        for(x = kk->qq->width; x--; )
        {
            attrset(kk->ncurses.attr[*attr++]);
            addch(*chars++ & 0x7f);
        }
    }
    refresh();
}

static void ncurses_handle_resize(caca_t *kk, unsigned int *new_width,
                                              unsigned int *new_height)
{
    struct winsize size;

    *new_width = kk->qq->width;
    *new_height = kk->qq->height;

    if(ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0)
    {
        *new_width = size.ws_col;
        *new_height = size.ws_row;
#if defined(HAVE_RESIZE_TERM)
        resize_term(*new_height, *new_width);
#else
        resizeterm(*new_height, *new_width);
#endif
        wrefresh(curscr);
    }
}

/*
 * XXX: following functions are local
 */

#if defined(HAVE_SIGNAL)
static RETSIGTYPE sigwinch_handler(int sig)
{
    sigwinch_kk->resize_event = 1;

    signal(SIGWINCH, sigwinch_handler);;
}
#endif

/*
 * Driver initialisation
 */

void ncurses_init_driver(caca_t *kk)
{
    kk->driver.driver = CACA_DRIVER_NCURSES;

    kk->driver.init_graphics = ncurses_init_graphics;
    kk->driver.end_graphics = ncurses_end_graphics;
    kk->driver.set_window_title = ncurses_set_window_title;
    kk->driver.get_window_width = ncurses_get_window_width;
    kk->driver.get_window_height = ncurses_get_window_height;
    kk->driver.display = ncurses_display;
    kk->driver.handle_resize = ncurses_handle_resize;
}

#endif /* USE_NCURSES */

