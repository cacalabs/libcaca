/*
 *   libcaca       ASCII-Art library
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

#ifdef HAVE_INTTYPES_H
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
#endif

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

#include <stdio.h>
void caca_blit(int x1, int y1, int x2, int y2, void *pixels, int w, int h)
{
    char foo[] = { ' ', '.', ':', ';', '=', '$', '%', '@', '#', '8', 'W' };
    int x, y, pitch;

    if(x1 > x2)
    {
        int tmp = x2; x2 = x1; x1 = tmp;
    }

    if(y1 > y2)
    {
        int tmp = y2; y2 = y1; y1 = tmp;
    }

    pitch = (3 * w + 3) / 4 * 4;

    for(y = y1 > 0 ? y1 : 0; y <= y2 && y <= caca_get_height(); y++)
        for(x = x1 > 0 ? x1 : 0; x <= x2 && x <= caca_get_width(); x++)
        {
            int fromx = w * (x - x1) / (x2 - x1 + 1);
            int fromy = h * (y - y1) / (y2 - y1 + 1);
            int r = ((unsigned char *)pixels)[3 * fromx + pitch * fromy];
            int g = ((unsigned char *)pixels)[3 * fromx + 1 + pitch * fromy];
            int b = ((unsigned char *)pixels)[3 * fromx + 2 + pitch * fromy];

            if(r == g && g == b)
            {
                caca_set_color(EE_LIGHTGRAY);
            }
            else
            {
                static int foo_colors[6] = {EE_LIGHTRED, EE_YELLOW, EE_LIGHTGREEN, EE_LIGHTCYAN, EE_LIGHTBLUE, EE_LIGHTMAGENTA};
                float min = r, max = r, delta, hue, sat;
                if(min > g) min = g; if(max < g) max = g;
                if(min > b) min = b; if(max < b) max = b;

                delta = max - min;

		sat = max / delta;

                if(delta > 20)
                {
                    if( r == max )
                        hue = (g - b) / delta; // between yellow & magenta
                    else if( g == max )
                        hue = 2 + (b - r) / delta; // between cyan & yellow
                    else
                        hue = 4 + (r - g) / delta; // between magenta & cyan

                    hue *= 60; // degrees
                    if( hue < 0 )
                        hue += 360;

                    caca_set_color(foo_colors[(int)(hue + 30) / 60]);
                }
                else
                {
                    caca_set_color(EE_LIGHTGRAY);
                }
            }

            caca_putchar(x, y, foo[(r + g + b) / 3 / 25]);
        }
}

