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
#include <math.h>
#include <unistd.h>

#include "common.h"

void intro(void)
{
    struct ee_sprite *foo_sprite = ee_load_sprite("data/foofight.txt");
    struct ee_sprite *bar_sprite = ee_load_sprite("data/barfight.txt");
    struct ee_sprite *baz_sprite = ee_load_sprite("data/bazfight.txt");

    int frame = 0;

    while(ee_get_key() == 0)
    {
        int i, xo, yo, x[5], y[5];

        frame++;

        ee_clear();

        xo = ee_get_width() / 2;
        yo = ee_get_height() / 2;

        ee_set_color(EE_RED);
        ee_fill_ellipse(xo, yo, 16, 8, '#');
        ee_set_color(EE_GREEN);
        ee_draw_thin_ellipse(xo, yo, 16, 8);

        for(i = 0; i < 4; i ++)
        {
            x[i] = xo + 0.5 + 12 * cos(0.05 * frame + i * M_PI / 2);
            y[i] = yo + 0.5 + 6 * sin(0.05 * frame + i * M_PI / 2);
        }
        x[4] = x[0];
        y[4] = y[0];

        ee_set_color(EE_BLACK);
        ee_fill_triangle(x[0], y[0], x[1], y[1], x[2], y[2], '#');
        ee_fill_triangle(x[0], y[0], x[3], y[3], x[2], y[2], '#');
        ee_draw_line(x[0], y[0], x[2], y[2], '#');
        ee_set_color(EE_GREEN);
        ee_draw_thin_polyline(x, y, 4);

        ee_draw_sprite(xo, yo, foo_sprite, frame % 5);

        ee_refresh();

        usleep(40000);
    }
}

