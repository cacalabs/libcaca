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

void ee_draw_circle(int x, int y, int r, char c)
{
    int test, dx, dy;

    /* Optimized Bresenham. Kick ass. */
    for(test = 0, dx = 0, dy = r ; dx <= dy ; dx++)
    {
        ee_putcharTO(x + dx, y + dy / 2, c);
        ee_putcharTO(x - dx, y + dy / 2, c);
        ee_putcharTO(x + dx, y - dy / 2, c);
        ee_putcharTO(x - dx, y - dy / 2, c);

        ee_putcharTO(x + dy, y + dx / 2, c);
        ee_putcharTO(x - dy, y + dx / 2, c);
        ee_putcharTO(x + dy, y - dx / 2, c);
        ee_putcharTO(x - dy, y - dx / 2, c);

        test += test > 0 ? dx - dy-- : dx;
    }
}

