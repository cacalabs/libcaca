/*
 *   libcaca       ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
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

#include "caca.h"
#include "caca_internals.h"

static int _caca_color = 0;

void caca_set_color(int color)
{
    if(color < 0 || color > 15)
        return;

    _caca_color = color;
#if defined(USE_SLANG)
    SLsmg_set_color(color + 1);
#elif defined(USE_NCURSES)
    attrset(_caca_attr[color]);
#elif defined(USE_CONIO)
    textcolor(color);
#endif
}

int caca_get_color(void)
{
    return _caca_color;
}

void caca_putchar(int x, int y, char c)
{
    if(x < 0 || x >= (int)caca_get_width() ||
       y < 0 || y >= (int)caca_get_height())
        return;

#if defined(USE_SLANG)
    SLsmg_gotorc(y, x);
    SLsmg_write_char(c);
#elif defined(USE_NCURSES)
    move(y, x);
    addch(c);
#elif defined(USE_CONIO)
    _caca_screen[2 * (x + y * caca_get_width())] = c;
    _caca_screen[2 * (x + y * caca_get_width()) + 1] = _caca_color;
//    gotoxy(x + 1, y + 1);
//    putch(c);
#endif
}

void caca_putstr(int x, int y, const char *s)
{
    unsigned int len;

    if(y < 0 || y >= (int)caca_get_height() || x >= (int)caca_get_width())
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

    if(x + len >= caca_get_width())
    {
        memcpy(_caca_scratch_line, s, caca_get_width() - x);
        _caca_scratch_line[caca_get_width() - x] = '\0';
        s = _caca_scratch_line;
    }

#if defined(USE_SLANG)
    SLsmg_gotorc(y, x);
    SLsmg_write_string(s);
#elif defined(USE_NCURSES)
    move(y, x);
    addstr(s);
#elif defined(USE_CONIO)
    char *buf = _caca_screen + 2 * (x + y * caca_get_width());
    while(*s)
    {
        *buf++ = *s++;
        *buf++ = _caca_color;
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

    if(y < 0 || y >= (int)caca_get_height() || x >= (int)caca_get_width())
        return;

    if(caca_get_width() - x + 1 > BUFSIZ)
        buf = malloc(caca_get_width() - x + 1);

    va_start(args, format);
    vsnprintf(buf, caca_get_width() - x + 1, format, args);
    buf[caca_get_width() - x] = '\0';
    va_end(args);

    caca_putstr(x, y, buf);

    if(buf != tmp)
        free(buf);
}

void caca_clear(void)
{
    /* We could use SLsmg_cls() etc., but drawing empty lines is much faster */
    int y = caca_get_height();

    while(y--)
        caca_putstr(0, y, _caca_empty_line);
}

