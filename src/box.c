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

/**  \file box.c
 *   \version \$Id$
 *   \author Sam Hocevar <sam@zoy.org>
 *   \brief Simple box drawing functions
 *
 *   This file contains box drawing functions, both filled and outline.
 */

#include "config.h"

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

void caca_draw_box(int x1, int y1, int x2, int y2, char c)
{
    caca_draw_line(x1, y1, x1, y2, c);
    caca_draw_line(x1, y2, x2, y2, c);
    caca_draw_line(x2, y2, x2, y1, c);
    caca_draw_line(x2, y1, x1, y1, c);
}

void caca_draw_thin_box(int x1, int y1, int x2, int y2)
{
    int x, y, xmax, ymax;

    if(x1 > x2)
    {
        int tmp = x1;
        x1 = x2; x2 = tmp;
    }

    if(y1 > y2)
    {
        int tmp = y1;
        y1 = y2; y2 = tmp;
    }

    xmax = caca_get_width() - 1;
    ymax = caca_get_height() - 1;

    if(x2 < 0 || y2 < 0 || x1 > xmax || y1 > ymax)
        return;

    /* Draw edges */
    if(y1 >= 0)
        for(x = x1 < 0 ? 1 : x1 + 1; x < x2 && x < xmax; x++)
            caca_putchar(x, y1, '-');

    if(y2 <= ymax)
        for(x = x1 < 0 ? 1 : x1 + 1; x < x2 && x < xmax; x++)
            caca_putchar(x, y2, '-');

    if(x1 >= 0)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            caca_putchar(x1, y, '|');

    if(x2 <= xmax)
        for(y = y1 < 0 ? 1 : y1 + 1; y < y2 && y < ymax; y++)
            caca_putchar(x2, y, '|');

    /* Draw corners */
    if(x1 >= 0 && y1 >= 0)
        caca_putchar(x1, y1, ',');

    if(x1 >= 0 && y2 <= ymax)
        caca_putchar(x1, y2, '`');

    if(x2 <= xmax && y1 >= 0)
        caca_putchar(x2, y1, '.');

    if(x2 <= xmax && y2 <= ymax)
        caca_putchar(x2, y2, '\'');
}

void caca_fill_box(int x1, int y1, int x2, int y2, char c)
{
    int x, y, xmax, ymax;

    if(x1 > x2)
    {
        int tmp = x1;
        x1 = x2; x2 = tmp;
    }

    if(y1 > y2)
    {
        int tmp = y1;
        y1 = y2; y2 = tmp;
    }

    xmax = caca_get_width() - 1;
    ymax = caca_get_height() - 1;

    if(x2 < 0 || y2 < 0 || x1 > xmax || y1 > ymax)
        return;

    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;
    if(x2 > xmax) x2 = xmax;
    if(y2 > ymax) y2 = ymax;

    for(y = y1; y <= y2; y++)
        for(x = x1; x <= x2; x++)
            caca_putchar(x, y, c);
}

