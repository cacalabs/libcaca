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
#   include <conio.h>
#else
#   error "no graphics library detected"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ee.h"
#include "ee_internals.h"

static int _ee_color = 0;

void ee_set_color(int color)
{
    if(color < 0 || color > 15)
        return;

    _ee_color = color;
#if defined(USE_SLANG)
    SLsmg_set_color(color + 1);
#elif defined(USE_NCURSES)
    attrset(_ee_attr[color]);
#elif defined(USE_CONIO)
    textcolor(color);
#endif
}

int ee_get_color(void)
{
    return _ee_color;
}

void ee_putchar(int x, int y, char c)
{
    if(x < 0 || x >= ee_get_width() || y < 0 || y >= ee_get_height())
        return;

#if defined(USE_SLANG)
    SLsmg_gotorc(y, x);
    SLsmg_write_char(c);
#elif defined(USE_NCURSES)
    move(y, x);
    addch(c);
#elif defined(USE_CONIO)
    _ee_screen[2 * (x + y * ee_get_width())] = c;
    _ee_screen[2 * (x + y * ee_get_width()) + 1] = _ee_color;
//    gotoxy(x + 1, y + 1);
//    putch(c);
#endif
}

void ee_putstr(int x, int y, const char *s)
{
    int len;

    if(y < 0 || y >= ee_get_height() || x >= ee_get_width())
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

    if(x + len >= ee_get_width())
    {
        memcpy(_ee_scratch_line, s, ee_get_width() - x);
        _ee_scratch_line[ee_get_width() - x] = '\0';
        s = _ee_scratch_line;
    }

#if defined(USE_SLANG)
    SLsmg_gotorc(y, x);
    SLsmg_write_string(s);
#elif defined(USE_NCURSES)
    move(y, x);
    addstr(s);
#elif defined(USE_CONIO)
    char *buf = _ee_screen + 2 * (x + y * ee_get_width());
    while(*s)
    {
        *buf++ = *s++;
        *buf++ = _ee_color;
    }
//    gotoxy(x + 1, y + 1);
//    cputs(s);
#endif
}

void ee_printf(int x, int y, const char *format, ...)
{
    char tmp[BUFSIZ];
    char *buf = tmp;
    va_list args;

    if(y < 0 || y >= ee_get_height() || x >= ee_get_width())
        return;

    if(ee_get_width() - x + 1 > BUFSIZ)
        buf = malloc(ee_get_width() - x + 1);

    va_start(args, format);
    vsnprintf(buf, ee_get_width() - x + 1, format, args);
    buf[ee_get_width() - x] = '\0';
    va_end(args);

    ee_putstr(x, y, buf);

    if(buf != tmp)
        free(buf);
}

void ee_clear(void)
{
    /* We could use SLsmg_cls() etc., but drawing empty lines is much faster */
    int y = ee_get_height();

    while(y--)
        ee_putstr(0, y, _ee_empty_line);
}

