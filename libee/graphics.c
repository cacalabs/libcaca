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
#endif

#include <string.h>
#include <stdlib.h>

#include "ee.h"

void ee_color(int color)
{
#ifdef USE_SLANG
    SLsmg_set_color(color);
#elif USE_NCURSES
    attrset(COLOR_PAIR(color));
#else
    /* Use dummy driver */
#endif
}

void ee_putchar(int x, int y, char c)
{
#ifdef USE_SLANG
    SLsmg_gotorc(y,x);
    SLsmg_write_char(c);
#elif USE_NCURSES
    move(y,x);
    addch(c);
#else
    /* Use dummy driver */
#endif
}

void ee_putstr(int x, int y, char *s)
{
#ifdef USE_SLANG
    SLsmg_gotorc(y,x);
    SLsmg_write_string(s);
#elif USE_NCURSES
    move(y,x);
    addstr(s);
#else
    /* Use dummy driver */
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
        ee_putstr(0, y, empty_line);
    }

    free(empty_line);
#else
    /* Use dummy driver */
#endif
}

