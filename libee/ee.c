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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ee.h"

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

#else
    /* Dummy driver */

#endif

    return 0;
}

int ee_get_width(void)
{
#ifdef USE_SLANG
    return SLtt_Screen_Cols;
#elif USE_NCURSES
    return COLS;
#else
    return 80;
#endif
}

int ee_get_height(void)
{
#ifdef USE_SLANG
    return SLtt_Screen_Rows;
#elif USE_NCURSES
    return LINES;
#else
    return 25;
#endif
}

void ee_clear(void)
{
#if defined(USE_SLANG) || defined(USE_NCURSES)
    /* We could use SLsmg_cls(), but drawing empty lines is much faster */
    int x = ee_get_width(), y = ee_get_height();
    char *empty_line = malloc((x + 1) * sizeof(char));

    memset(empty_line, ' ', x);
    empty_line[x] = '\0';

    while(y--)
    {
        ee_goto(0, y);
        ee_putstr(empty_line);
    }

    free(empty_line);
#else
    /* Use dummy driver */
#endif
}

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

#define DELAY 40000
void ee_refresh(void)
{
    static int64_t local_clock = 0;
    int64_t now;

    ee_goto(0, 0);

    if(!local_clock)
    {
        /* Initialize local_clock */
        local_clock = local_time();
    }

    if(local_time() > local_clock + 10000)
    {
        /* If we are late, we shouldn't display anything */
    }

#ifdef USE_SLANG
    SLsmg_refresh();
#elif USE_NCURSES
    refresh();
#else
    /* Use dummy driver */
#endif

    now = local_time();

    if(now < local_clock + DELAY - 10000)
    {
        usleep(local_clock + DELAY - 10000 - now);
    }

    local_clock += DELAY;
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
#else
    /* Use dummy driver */
#endif
}

