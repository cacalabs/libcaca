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

#include "ee.h"

static void ellipsepoints(int, int, int, int, char);

void ee_draw_circle(int x, int y, int r, char c)
{
    int test, dx, dy;

    /* Optimized Bresenham. Kick ass. */
    for(test = 0, dx = 0, dy = r ; dx <= dy ; dx++)
    {
        ellipsepoints(x, y, dx, dy, c);
        ellipsepoints(x, y, dy, dx, c);

        test += test > 0 ? dx - dy-- : dx;
    }
}

void ee_draw_ellipse(int xo, int yo, int a, int b, char c)
{
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    ellipsepoints(xo, yo, x, y, c);

    while( a*a*y - a*a/2 > b*b*(x+1))
    {
        if(d1 < 0)
        {
            d1 += b*b*(2*x+1); /* XXX: "Computer Graphics" has + 3 here. */
        }
        else
        {
            d1 += b*b*(2*x*1) + a*a*(-2*y+2);
            y--;
        }
        x++;
        ellipsepoints(xo, yo, x, y, c);
    }

    d2 = b*b*(x+0.5)*(x+0.5) + a*a*(y-1)*(y-1) - a*a*b*b;
    while(y > 0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++;
        }
        else
        {
            d2 += a*a*(-2*y+3);
        }

        y--;
        ellipsepoints(xo, yo, x, y, c);
    }
}

static void ellipsepoints(int xo, int yo, int x, int y, char c)
{
    ee_putcharTO(xo + x, yo + y, c);
    ee_putcharTO(xo - x, yo + y, c);
    ee_putcharTO(xo + x, yo - y, c);
    ee_putcharTO(xo - x, yo - y, c);
}

