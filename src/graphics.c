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

/**  \file graphics.c
 *   \version \$Id$
 *   \author Sam Hocevar <sam@zoy.org>
 *   \brief Character drawing functions
 *
 *   This file contains character and string drawing functions.
 */

#include "config.h"

#if defined(USE_SLANG)
#   include <slang.h>
#elif defined(USE_NCURSES)
#   include <curses.h>
#elif defined(USE_CONIO)
#   include <conio.h>
#   if defined(SCREENUPDATE_IN_PC_H)
#       include <pc.h>
#   endif
#else
#   error "no graphics library detected"
#endif

#ifdef HAVE_INTTYPES_H
#   include <inttypes.h>
#endif

#include <stdio.h> /* BUFSIZ */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>

#include "caca.h"
#include "caca_internals.h"

#ifdef USE_CONIO
static struct text_info ti;
#endif

unsigned int _caca_width;
unsigned int _caca_height;

static unsigned int _caca_delay;
static unsigned int _caca_rendertime;

static enum caca_color _caca_fgcolor = CACA_COLOR_LIGHTGRAY;
static enum caca_color _caca_bgcolor = CACA_COLOR_BLACK;

void caca_set_color(enum caca_color fgcolor, enum caca_color bgcolor)
{
    if(fgcolor < 0 || fgcolor > 15 || bgcolor < 0 || bgcolor > 15)
        return;

    _caca_fgcolor = fgcolor;
    _caca_bgcolor = bgcolor;
#if defined(USE_SLANG)
    SLsmg_set_color((bgcolor + 16 * fgcolor) /*% 128*/);
#elif defined(USE_NCURSES)
    attrset(_caca_attr[fgcolor + 16 * bgcolor]);
#elif defined(USE_CONIO)
    textbackground(bgcolor);
    textcolor(fgcolor);
#endif
}

enum caca_color caca_get_fg_color(void)
{
    return _caca_fgcolor;
}

enum caca_color caca_get_bg_color(void)
{
    return _caca_bgcolor;
}

void caca_putchar(int x, int y, char c)
{
#if defined(USE_CONIO)
    char *data;
#endif
    if(x < 0 || x >= (int)_caca_width ||
       y < 0 || y >= (int)_caca_height)
        return;

#if defined(USE_SLANG)
    SLsmg_gotorc(y, x);
    SLsmg_write_char(c);
#elif defined(USE_NCURSES)
    move(y, x);
    addch(c);
#elif defined(USE_CONIO)
    data = _caca_screen + 2 * (x + y * _caca_width);
    data[0] = c;
    data[1] = (_caca_bgcolor << 4) | _caca_fgcolor;
//    gotoxy(x + 1, y + 1);
//    putch(c);
#endif
}

void caca_putstr(int x, int y, const char *s)
{
    unsigned int len;

    if(y < 0 || y >= (int)_caca_height || x >= (int)_caca_width)
        return;

    len = strlen(s);

    if(x < 0)
    {
        len -= -x;
        if(len < 0)
            return;
        s += -x;
        x = 0;
    }

    if(x + len >= _caca_width)
    {
        memcpy(_caca_scratch_line, s, _caca_width - x);
        _caca_scratch_line[_caca_width - x] = '\0';
        s = _caca_scratch_line;
    }

#if defined(USE_SLANG)
    SLsmg_gotorc(y, x);
    SLsmg_write_string((char *)(intptr_t)s);
#elif defined(USE_NCURSES)
    move(y, x);
    addstr(s);
#elif defined(USE_CONIO)
    char *buf = _caca_screen + 2 * (x + y * _caca_width);
    while(*s)
    {
        *buf++ = *s++;
        *buf++ = (_caca_bgcolor << 4) | _caca_fgcolor;
    }
//    gotoxy(x + 1, y + 1);
//    cputs(s);
#endif
}

void caca_printf(int x, int y, const char *format, ...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    if(y < 0 || y >= (int)_caca_height || x >= (int)_caca_width)
        return;

    if(_caca_width - x + 1 > BUFSIZ)
        buf = malloc(_caca_width - x + 1);

    va_start(args, format);
#if defined(HAVE_VSNPRINTF)
    vsnprintf(buf, _caca_width - x + 1, format, args);
#else
    vsprintf(buf, format, args);
#endif
    buf[_caca_width - x] = '\0';
    va_end(args);

    caca_putstr(x, y, buf);

    if(buf != tmp)
        free(buf);
}

void caca_clear(void)
{
    enum caca_color oldfg = caca_get_fg_color();
    enum caca_color oldbg = caca_get_bg_color();
    int y = _caca_height;

    caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);

    /* We could use SLsmg_cls() etc., but drawing empty lines is much faster */
    while(y--)
        caca_putstr(0, y, _caca_empty_line);

    caca_set_color(oldfg, oldbg);
}

int _caca_init_graphics(void)
{
#if defined(USE_SLANG)
    /* See SLang ref., 5.4.4. */
    static char *slang_colors[16] =
    {
        /* Standard colours */
        "black",
        "blue",
        "green",
        "cyan",
        "red",
        "magenta",
        "brown",
        "lightgray",
        /* Bright colours */
        "gray",
        "brightblue",
        "brightgreen",
        "brightcyan",
        "brightred",
        "brightmagenta",
        "yellow",
        "white",
    };

    int fg, bg;

    for(fg = 0; fg < 16; fg++)
        for(bg = 0; bg < 16; bg++)
        {
            int i = bg + 16 * fg;
            SLtt_set_color(i, NULL, slang_colors[fg], slang_colors[bg]);
        }

    /* Disable alt charset support so that we get all 256 colour pairs */
    SLtt_Has_Alt_Charset = 0;

    _caca_width = SLtt_Screen_Cols;
    _caca_height = SLtt_Screen_Rows;

#elif defined(USE_NCURSES)
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

    int fg, bg, max;

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
             * is light gray on black, since some terminals don't like
             * this colour pair to be redefined. */
            int col = ((max + 7 - fg) % max) + max * bg;
            init_pair(col, curses_colors[fg], curses_colors[bg]);
            _caca_attr[fg + 16 * bg] = COLOR_PAIR(col);

            if(max == 8)
            {
                /* Bright fg on simple bg */
                _caca_attr[fg + 8 + 16 * bg] = A_BOLD | COLOR_PAIR(col);
                /* Simple fg on bright bg */
                _caca_attr[fg + 16 * (bg + 8)] = A_BLINK | COLOR_PAIR(col);
                /* Bright fg on bright bg */
                _caca_attr[fg + 8 + 16 * (bg + 8)] = A_BLINK | A_BOLD
                                                             | COLOR_PAIR(col);
            }
        }

    _caca_width = COLS;
    _caca_height = LINES;

#elif defined(USE_CONIO)
    gettextinfo(&ti);
    _caca_screen = malloc(2 * ti.screenwidth * ti.screenheight);
    if(_caca_screen == NULL)
        return -1;
#   if defined(SCREENUPDATE_IN_PC_H)
    ScreenRetrieve(_caca_screen);
#   else
    /* FIXME */
#   endif
    _caca_width = ti.screenwidth;
    _caca_height = ti.screenheight;

#endif
    _caca_empty_line = malloc(_caca_width + 1);
    memset(_caca_empty_line, ' ', _caca_width);
    _caca_empty_line[_caca_width] = '\0';

    _caca_scratch_line = malloc(_caca_width + 1);

    _caca_delay = 0;
    _caca_rendertime = 0;

    return 0;
}

void caca_set_delay(unsigned int usec)
{
    _caca_delay = usec;
}

unsigned int caca_get_rendertime(void)
{
    return _caca_rendertime;
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

