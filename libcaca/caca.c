/*
 *   libcaca       ASCII-Art library
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

#include "caca.h"
#include "caca_internals.h"

static unsigned int _caca_delay;
static unsigned int _caca_rendertime;
char *_caca_empty_line;
char *_caca_scratch_line;

#if defined(USE_NCURSES)
int _caca_attr[16];
#endif

#if defined(USE_CONIO)
static struct text_info ti;
char *_caca_screen;
#endif

int caca_init(void)
{
#if defined(USE_SLANG)
    static char *slang_colors[16] =
    {
        "black",
        "blue",
        "green",
        "cyan",
        "red",
        "magenta",
        "brown",
        "lightgray",
        "gray",
        "brightblue",
        "brightgreen",
        "brightcyan",
        "brightred",
        "brightmagenta",
        "yellow",
        "white",
    };

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

    for(i = 0; i < 16; i++)
        SLtt_set_color(i + 1, NULL, slang_colors[i], "black");

#elif defined(USE_NCURSES)
    int i;

    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    start_color();

    init_pair(1 + EE_BLACK,        COLOR_BLACK,   COLOR_BLACK);
    init_pair(1 + EE_BLUE,         COLOR_BLUE,    COLOR_BLACK);
    init_pair(1 + EE_GREEN,        COLOR_GREEN,   COLOR_BLACK);
    init_pair(1 + EE_CYAN,         COLOR_CYAN,    COLOR_BLACK);
    init_pair(1 + EE_RED,          COLOR_RED,     COLOR_BLACK);
    init_pair(1 + EE_MAGENTA,      COLOR_MAGENTA, COLOR_BLACK);
    init_pair(1 + EE_BROWN,        COLOR_YELLOW,  COLOR_BLACK);
    init_pair(1 + EE_LIGHTGRAY,    COLOR_WHITE,   COLOR_BLACK);
    init_pair(1 + EE_DARKGRAY,     COLOR_BLACK,   COLOR_BLACK);
    init_pair(1 + EE_LIGHTBLUE,    COLOR_BLUE,    COLOR_BLACK);
    init_pair(1 + EE_LIGHTGREEN,   COLOR_GREEN,   COLOR_BLACK);
    init_pair(1 + EE_LIGHTCYAN,    COLOR_CYAN,    COLOR_BLACK);
    init_pair(1 + EE_LIGHTRED,     COLOR_RED,     COLOR_BLACK);
    init_pair(1 + EE_LIGHTMAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(1 + EE_YELLOW,       COLOR_YELLOW,  COLOR_BLACK);
    init_pair(1 + EE_WHITE,        COLOR_WHITE,   COLOR_BLACK);

    for(i = 0; i < 8; i++)
    {
        _caca_attr[i] = COLOR_PAIR(1 + i);
        _caca_attr[i + 8] = A_BOLD | COLOR_PAIR(1 + i);
    }

#elif defined(USE_CONIO)
    gettextinfo(&ti);
    _caca_screen = malloc(2 * ti.screenwidth * ti.screenheight);
    if(_caca_screen == NULL)
        return -1;
    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenRetrieve(_caca_screen);
#   else
    /* FIXME */
#   endif

#endif
    _caca_empty_line = malloc(caca_get_width() + 1);
    memset(_caca_empty_line, ' ', caca_get_width());
    _caca_empty_line[caca_get_width()] = '\0';

    _caca_scratch_line = malloc(caca_get_width() + 1);

    _caca_delay = 0;
    _caca_rendertime = 0;

    return 0;
}

unsigned int caca_get_width(void)
{
#if defined(USE_SLANG)
    return SLtt_Screen_Cols;
#elif defined(USE_NCURSES)
    return COLS;
#elif defined(USE_CONIO)
    return ti.screenwidth;
#endif
}

unsigned int caca_get_height(void)
{
#if defined(USE_SLANG)
    return SLtt_Screen_Rows;
#elif defined(USE_NCURSES)
    return LINES;
#else
    return ti.screenheight;
#endif
}

void caca_set_delay(unsigned int usec)
{
    _caca_delay = usec;
}

unsigned int caca_get_rendertime(void)
{
    return _caca_rendertime;
}

const char *caca_get_color_name(unsigned int color)
{
    static const char *color_names[16] =
    {
        "black",
        "blue",
        "green",
        "cyan",
        "red",
        "magenta",
        "brown",
        "light gray",
        "dark gray",
        "light blue",
        "light green",
        "light cyan",
        "light red",
        "light magenta",
        "yellow",
        "white",
    };

    if(color < 0 || color > 15)
        return "unknown color";

    return color_names[color];
}

static unsigned int _caca_getticks(void)
{
    static unsigned int last_sec = 0, last_usec = 0;

    struct timeval tv;
    unsigned int ticks = 0;

    gettimeofday(&tv, NULL);

    if(last_sec != 0)
    {
        ticks = (tv.tv_sec - last_sec) * 1000000 + (tv.tv_usec - last_usec);
    }

    last_sec = tv.tv_sec;
    last_usec = tv.tv_usec;

    return ticks;
}

void caca_refresh(void)
{
#define IDLE_USEC 10000
    static int lastticks = 0;
    int ticks = lastticks + _caca_getticks();

#if defined(USE_SLANG)
    SLsmg_refresh();
#elif defined(USE_NCURSES)
    refresh();
#elif defined(USE_CONIO)
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenUpdate(_caca_screen);
#   else
    /* FIXME */
#   endif
#endif

    /* Wait until _caca_delay + time of last call */
    ticks += _caca_getticks();
    for(; ticks + IDLE_USEC < (int)_caca_delay; ticks += _caca_getticks())
        usleep(IDLE_USEC);

    /* Update the sliding mean of the render time */
    _caca_rendertime = (7 * _caca_rendertime + ticks) / 8;

    lastticks = ticks - _caca_delay;

    /* If we drifted too much, it's bad, bad, bad. */
    if(lastticks > (int)_caca_delay)
        lastticks = 0;
}

void caca_end(void)
{
#if defined(USE_SLANG)
    SLtt_set_cursor_visibility(1);
    SLang_reset_tty();
    SLsmg_reset_smg();
#elif defined(USE_NCURSES)
    curs_set(1);
    endwin();
#elif defined(USE_CONIO)
    _wscroll = 1;
    textcolor((enum COLORS)WHITE);
    textbackground((enum COLORS)BLACK);
    gotoxy(caca_get_width(), caca_get_height());
    cputs("\r\n");
    _setcursortype(_NORMALCURSOR);
#endif
}

