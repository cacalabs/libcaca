/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
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
#include <sys/time.h>
#include <time.h>

#include "common.h"

int init_graphics( void )
{
#ifdef USE_SLANG
    /* Initialize slang library */
    SLsig_block_signals();
    SLtt_get_terminfo();

    if( SLkp_init() == -1 )
    {
        SLsig_unblock_signals();
        return 1;
    }

    SLang_init_tty (-1, 0, 1);

    if( SLsmg_init_smg() == -1 )
    {
        SLsig_unblock_signals();
        return 1;
    }

    SLsig_unblock_signals();

    SLsmg_cls();
    SLtt_set_cursor_visibility( 0 );
    SLsmg_refresh();
#elif USE_NCURSES
    /* Initialize ncurses library */
    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set( 0 );
#else
    /* Dummy driver */
#endif

    return 0;
}

void init_game( game *g )
{
#ifdef USE_SLANG
    static char * const colors[] =
    {
        "black", "green", "yellow", "white",
        "red", "gray", "lightgray", "blue", "cyan", "magenta", NULL
    };

    int i;

    for( i = 0; colors[i] ; i++ )
    {
        SLtt_set_color( i+1, NULL, colors[i], "black" );
    }

    g->w = SLtt_Screen_Cols;
    g->h = SLtt_Screen_Rows;
#elif USE_NCURSES
    start_color();

    init_pair( BLACK, COLOR_BLACK, COLOR_BLACK );
    init_pair( GREEN, COLOR_GREEN, COLOR_BLACK );
    init_pair( YELLOW, COLOR_YELLOW, COLOR_BLACK );
    init_pair( WHITE, COLOR_WHITE, COLOR_BLACK );
    init_pair( RED, COLOR_RED, COLOR_BLACK );
    init_pair( GRAY, COLOR_WHITE, COLOR_BLACK ); // XXX
    init_pair( LIGHTGRAY, COLOR_WHITE, COLOR_BLACK ); // XXX
    init_pair( BLUE, COLOR_BLUE, COLOR_BLACK );
    init_pair( CYAN, COLOR_CYAN, COLOR_BLACK );
    init_pair( MAGENTA, COLOR_MAGENTA, COLOR_BLACK );

    g->w = COLS;
    g->h = LINES;
#else
    /* Use dummy driver */
    g->w = 80;
    g->h = 25;
#endif
}

char get_key( void )
{
#ifdef USE_SLANG
    if( SLang_input_pending (0) )
    {
        return SLang_getkey();
    }
#elif USE_NCURSES
    char key;

    if( ( key = getch() ) != ERR )
    {
        return key;
    }
#else
    /* Use dummy driver */
    char key = GET_RAND(0,256);

    if( key != 'q' && key != 'p' && key != '\t' )
    {
        return key;
    }
#endif

    return 0;
}

void clear_graphics( game *g )
{
#ifdef USE_SLANG
    //SLsmg_cls();
    int y;
    for( y = 0; y < g->h; y++ )
    {
        gfx_goto( 0, y );
        gfx_putstr( "                                                                                " );
    }
#elif USE_NCURSES
    //clear();
    int y;
    for( y = 0; y < g->h; y++ )
    {
        gfx_goto( 0, y );
        gfx_putstr( "                                                                                " );
    }
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

void refresh_graphics( void )
{
    static int64_t local_clock = 0;
    int64_t now;

    gfx_goto( 0, 0 );

    if( !local_clock )
    {
        /* Initialize local_clock */
        local_clock = local_time();
    }

    if( local_time() > local_clock + 10000 )
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

    if( now < local_clock + DELAY - 10000 )
    {
        usleep( local_clock + DELAY - 10000 - now );
    }

    local_clock += DELAY;
}

void end_graphics( void )
{
#ifdef USE_SLANG
    SLtt_set_cursor_visibility( 1 );
    SLang_reset_tty();
    SLsmg_reset_smg();
#elif USE_NCURSES
    curs_set( 1 );
    endwin();
#else
    /* Use dummy driver */
#endif
}

