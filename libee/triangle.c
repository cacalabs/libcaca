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

#include <stdlib.h>

#include "ee.h"

void ee_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, char c)
{
    int x, y, xa, xb, xmax, ymax;

    /* Bubble-sort y1 <= y2 <= y3 */
    if(y1 > y2)
    {
        ee_fill_triangle(x2, y2, x1, y1, x3, y3, c);
        return;
    }

    if(y2 > y3)
    {
        ee_fill_triangle(x1, y1, x3, y3, x2, y2, c);
        return;
    }

    /* Promote precision */
    x1 *= 4;
    x2 *= 4;
    x3 *= 4;

    xmax = ee_get_width() - 1;
    ymax = ee_get_height() - 1;

    /* Rasterize our triangle */
    for(y = y1 < 0 ? 0 : y1; y <= y3 && y <= ymax; y++)
    {
        if(y <= y2)
        {
            xa = (y1 == y2) ? x2 : x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            xb = (y1 == y3) ? x3 : x1 + (x3 - x1) * (y - y1) / (y3 - y1);
        }
        else
        {
            xa = (y3 == y2) ? x2 : x3 + (x2 - x3) * (y - y3) / (y2 - y3);
            xb = (y3 == y1) ? x1 : x3 + (x1 - x3) * (y - y3) / (y1 - y3);
        }

        if(xb < xa)
        {
            int tmp = xb;
            xb = xa; xa = tmp;
        }

        /* Rescale xa and xb, slightly cropping */
        xa = (xa + 2) / 4;
        xb = (xb - 2) / 4;

        if(xb < 0) continue;
        if(xa > xmax) continue;
        if(xa < 0) xa = 0;
        if(xb > xmax) xb = xmax;

        for(x = xa; x <= xb; x++)
            ee_putchar(x, y, c);
    }
}

