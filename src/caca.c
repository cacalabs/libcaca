/*
 *   libcaca       ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307  USA
 */

/**  \file caca.c
 *   \version \$Id$
 *   \author Sam Hocevar <sam@zoy.org>
 *   \brief Main \e libcaca functions
 *
 *   This file contains the main functions used by \e libcaca applications to
 *   initialise the library, get the screen properties, set the framerate and
 *   so on.
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
#include <string.h>

#include "caca.h"
#include "caca_internals.h"

static void caca_init_terminal(void);

char *_caca_empty_line;
char *_caca_scratch_line;

#if defined(USE_NCURSES)
static mmask_t oldmask;
int _caca_attr[16*16];
#endif

#if defined(USE_CONIO)
static struct text_info ti;
char *_caca_screen;
#endif

int caca_init(void)
{
#if defined(USE_NCURSES)
    mmask_t newmask;
#endif

    caca_init_terminal();

#if defined(USE_SLANG)
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
    SLkp_define_keysym("\e[M", 1001);
    SLtt_set_mouse_mode(1, 0);
    SLsmg_refresh();

    /* Disable scrolling so that hashmap scrolling optimization code
     * does not cause ugly refreshes due to slow terminals */
    SLtt_Term_Cannot_Scroll = 1;

#elif defined(USE_NCURSES)
    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    /* Activate mouse */
    newmask = ALL_MOUSE_EVENTS;
    mousemask(newmask, &oldmask);

#elif defined(USE_CONIO)
    _wscroll = 0;
    _setcursortype(_NOCURSOR);
    clrscr();

#endif
    if(_caca_init_graphics())
        return -1;

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

const char *caca_get_color_name(enum caca_color color)
{
    static const char *color_names[] =
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
        return "unknown";

    return color_names[color];
}

const char *caca_get_dithering_name(enum caca_dithering dithering)
{
    static const char *dithering_names[] =
    {
        "no",
        "2x2 ordered",
        "4x4 ordered",
        "8x8 ordered",
        "random"
    };

    if(dithering < 0 || dithering > 4)
        return "unknown";

    return dithering_names[dithering];
}

void caca_end(void)
{
#if defined(USE_SLANG)
    SLtt_set_mouse_mode(0, 0);
    SLtt_set_cursor_visibility(1);
    SLang_reset_tty();
    SLsmg_reset_smg();
#elif defined(USE_NCURSES)
    mousemask(oldmask, NULL);
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

static void caca_init_terminal(void)
{
#if defined(HAVE_GETENV) && defined(HAVE_PUTENV)
    char *term, *colorterm, *other;

    term = getenv("TERM");
    colorterm = getenv("COLORTERM");

    if(term && !strcmp(term, "xterm"))
    {
        /* If we are using gnome-terminal, it's really a 16 colour terminal */
        if(colorterm && !strcmp(colorterm, "gnome-terminal"))
        {
#if defined(USE_NCURSES)
            SCREEN *screen;
            screen = newterm("xterm-16color", stdout, stdin);
            if(screen == NULL)
                return;
            endwin();
#endif
            (void)putenv("TERM=xterm-16color");
            return;
        }

        /* Ditto if we are using Konsole */
        other = getenv("KONSOLE_DCOP_SESSION");
        if(other)
        {
#if defined(USE_NCURSES)
            SCREEN *screen;
            screen = newterm("xterm-16color", stdout, stdin);
            if(screen == NULL)
                return;
            endwin();
#endif
            (void)putenv("TERM=xterm-16color");
            return;
        }
    }
#endif
}

