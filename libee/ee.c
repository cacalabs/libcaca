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

#if defined(USE_SLANG)
#   include <slang.h>
#elif defined(USE_NCURSES)
#   include <curses.h>
#elif defined(USE_CONIO)
#   include <dos.h>
#   include <conio.h>
#   if defined(SCREENUPDATE_IN_PC_H)
#       include <pc.h>
#   endif
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
#if defined(USE_CONIO)
static struct text_info ti;
char *_screen_buffer;
#endif

int ee_init(void)
{
#if defined(USE_SLANG)
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

#elif defined(USE_NCURSES)
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

#elif defined(USE_CONIO)
    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();
    gettextinfo(&ti);
    _screen_buffer = malloc(ee_get_width() * ee_get_height() * 2);
    ScreenRetrieve(_screen_buffer);

#endif
    _ee_delay = 0;

    return 0;
}

void ee_set_delay(int usec)
{
    _ee_delay = usec;
}

int ee_get_width(void)
{
#if defined(USE_SLANG)
    return SLtt_Screen_Cols;
#elif defined(USE_NCURSES)
    return COLS;
#elif defined(USE_CONIO)
    return ti.screenwidth;
#endif
}

int ee_get_height(void)
{
#if defined(USE_SLANG)
    return SLtt_Screen_Rows;
#elif defined(USE_NCURSES)
    return LINES;
#else
    return ti.screenheight;
#endif
}

#if !defined(USE_CONIO)
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
#if !defined(USE_CONIO)
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

#if defined(USE_SLANG)
    SLsmg_refresh();
#elif defined(USE_NCURSES)
    refresh();
#elif defined(USE_CONIO)
    ScreenUpdate(_screen_buffer);
#endif

#if !defined(USE_CONIO)
    now = local_time();

    if(now < local_clock + _ee_delay - 10000)
    {
        usleep(local_clock + _ee_delay - 10000 - now);
    }

    local_clock += _ee_delay;
#else
    delay(5);
#endif
}

void ee_end(void)
{
#if defined(USE_SLANG)
    SLtt_set_cursor_visibility(1);
    SLang_reset_tty();
    SLsmg_reset_smg();
#elif defined(USE_NCURSES)
    curs_set(1);
    endwin();
#elif defined(USE_CONIO)
    ScreenUpdate(_screen_buffer);
    _wscroll = 1;
    textcolor((enum COLORS)WHITE);
    textbackground((enum COLORS)BLACK);
    gotoxy(ee_get_width(), ee_get_height());
    cputs("\r\n");
    _setcursortype(_NORMALCURSOR);
#endif
}

