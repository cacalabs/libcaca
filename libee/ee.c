/*
 *   libee         ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"

#ifdef USE_SLANG
#   include <slang.h>
#elif USE_NCURSES
#   include <curses.h>
#elif USE_CONIO
#   include <conio.h>
#else
#   error "no graphics library detected"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ee.h"

static int _ee_delay;
#ifdef USE_CONIO
static struct text_info ti;
#endif

int ee_init(void)
{
#ifdef USE_SLANG
    static char * colors[] = { "black", "green", "yellow", "white",
        "red", "gray", "lightgray", "blue", "cyan", "magenta", NULL };
    int i;

    /* Initialize slang library */
    SLsig_block_signals();
    SLtt_get_terminfo();

    if(SLkp_init() == -1)
    {
        SLsig_unblock_signals();
        return -1;
    }

    SLang_init_tty(-1, 0, 1);

    if(SLsmg_init_smg() == -1)
    {
        SLsig_unblock_signals();
        return -1;
    }

    SLsig_unblock_signals();

    SLsmg_cls();
    SLtt_set_cursor_visibility(0);
    SLsmg_refresh();

    for(i = 0; colors[i]; i++)
    {
        SLtt_set_color(i + 1, NULL, colors[i], "black");
    }

#elif USE_NCURSES
    /* Initialize ncurses library */
    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    start_color();

    init_pair(EE_BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(EE_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(EE_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(EE_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(EE_RED, COLOR_RED, COLOR_BLACK);
    init_pair(EE_GRAY, COLOR_WHITE, COLOR_BLACK); // XXX
    init_pair(EE_LIGHTGRAY, COLOR_WHITE, COLOR_BLACK); // XXX
    init_pair(EE_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(EE_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(EE_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);

#elif USE_CONIO
    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();
    gettextinfo(&ti);
//window(2, 2, 20, 20);

#endif
    _ee_delay = 0;

    return 0;
}

void ee_set_delay(int delay)
{
    _ee_delay = delay;
}

int ee_get_width(void)
{
#ifdef USE_SLANG
    return SLtt_Screen_Cols;
#elif USE_NCURSES
    return COLS;
#elif USE_CONIO
    return ti.screenwidth;
#endif
}

int ee_get_height(void)
{
#ifdef USE_SLANG
    return SLtt_Screen_Rows;
#elif USE_NCURSES
    return LINES;
#else
    return ti.screenheight;
#endif
}

#ifndef USE_CONIO
static int64_t local_time(void)
{
    struct timeval tv;
    int64_t now;

    gettimeofday(&tv, NULL);
    now = tv.tv_sec;
    now *= 1000000;
    now += tv.tv_usec;
    return now;
}
#endif

void ee_refresh(void)
{
#ifndef USE_CONIO
    static int64_t local_clock = 0;
    int64_t now;

    if(!local_clock)
    {
        /* Initialize local_clock */
        local_clock = local_time();
    }

    if(local_time() > local_clock + 10000)
    {
        /* If we are late, we shouldn't display anything */
    }
#endif

#ifdef USE_SLANG
    SLsmg_refresh();
#elif USE_NCURSES
    refresh();
#elif USE_CONIO
    /* Do nothing? */
#endif

#ifndef USE_CONIO
    now = local_time();

    if(now < local_clock + _ee_delay - 10000)
    {
        usleep(local_clock + _ee_delay - 10000 - now);
    }

    local_clock += _ee_delay;
#endif
}

void ee_end(void)
{
#ifdef USE_SLANG
    SLtt_set_cursor_visibility(1);
    SLang_reset_tty();
    SLsmg_reset_smg();
#elif USE_NCURSES
    curs_set(1);
    endwin();
#elif USE_CONIO
    _wscroll = 1;
    ee_set_color(EE_WHITE);
    ee_putstr(ee_get_width(), ee_get_height()-1, "\r\n");
    _setcursortype(_NORMALCURSOR);
#endif
}

