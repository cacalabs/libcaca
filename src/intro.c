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
    struct caca_sprite *foo_sprite = caca_load_sprite("data/foofight.txt");
    struct caca_sprite *bar_sprite = caca_load_sprite("data/barfight.txt");
    struct caca_sprite *baz_sprite = caca_load_sprite("data/bazfight.txt");

    int frame = 0;

    while(caca_get_key() == 0)
    {
        int i, xo, yo, x[5], y[5];

        frame++;

        caca_clear();

        xo = caca_get_width() / 2;
        yo = caca_get_height() / 2;

        caca_set_color(EE_RED);
        caca_fill_ellipse(xo, yo, 16, 8, '#');
        caca_set_color(EE_GREEN);
        caca_draw_thin_ellipse(xo, yo, 16, 8);

        for(i = 0; i < 4; i ++)
        {
            x[i] = xo + 0.5 + 12 * cos(0.05 * frame + i * M_PI / 2);
            y[i] = yo + 0.5 + 6 * sin(0.05 * frame + i * M_PI / 2);
        }
        x[4] = x[0];
        y[4] = y[0];

        caca_set_color(EE_BLACK);
        caca_fill_triangle(x[0], y[0], x[1], y[1], x[2], y[2], ' ');
        caca_fill_triangle(x[0], y[0], x[3], y[3], x[2], y[2], ' ');
        caca_draw_line(x[0], y[0], x[2], y[2], ' ');
        caca_set_color(EE_GREEN);
        caca_draw_thin_polyline(x, y, 4);

        caca_draw_sprite(xo, yo, foo_sprite, frame % 5);

        caca_refresh();

        usleep(40000);
    }
}

