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

#ifdef HAVE_INTTYPES_H
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
#endif

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

static enum caca_dithering _caca_dithering = CACA_DITHER_NONE;

void caca_set_dithering(enum caca_dithering dither)
{
    if(dither < 0 || dither > 1)
        return;

    _caca_dithering = dither;
}

void caca_blit(int x1, int y1, int x2, int y2, void *pixels, int w, int h)
{
    char foo[] = { ' ', '.', ':', ';', '=', '%', '$', 'W', '#', '8', '@' };
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

    for(y = y1 > 0 ? y1 : 0; y <= y2 && y <= (int)caca_get_height(); y++)
        for(x = x1 > 0 ? x1 : 0; x <= x2 && x <= (int)caca_get_width(); x++)
        {
static int light_colors[] = {CACA_COLOR_LIGHTMAGENTA, CACA_COLOR_LIGHTRED, CACA_COLOR_YELLOW, CACA_COLOR_LIGHTGREEN, CACA_COLOR_LIGHTCYAN, CACA_COLOR_LIGHTBLUE, CACA_COLOR_LIGHTMAGENTA};
static int dark_colors[] = {CACA_COLOR_MAGENTA, CACA_COLOR_RED, CACA_COLOR_BROWN, CACA_COLOR_GREEN, CACA_COLOR_CYAN, CACA_COLOR_BLUE, CACA_COLOR_MAGENTA};
            int fromx = w * (x - x1) / (x2 - x1 + 1);
            int fromy = h * (y - y1) / (y2 - y1 + 1);
            int r = ((unsigned char *)pixels)[3 * fromx + pitch * fromy];
            int g = ((unsigned char *)pixels)[3 * fromx + 1 + pitch * fromy];
            int b = ((unsigned char *)pixels)[3 * fromx + 2 + pitch * fromy];
            int hue, sat, val;

            int min = r, max = r, delta;
            if(min > g) min = g; if(max < g) max = g;
            if(min > b) min = b; if(max < b) max = b;

            delta = max - min;
            val = max; /* 0 - 255 */
            sat = max ? 256 * delta / max : 0; /* 0 - 255 */

            if(sat > caca_rand(64, 128))
            {
                /* XXX: Values are automatically clipped between 0 and 6
                 * because of delta/2 */
                if( r == max )
                    hue = 1 + (float)(g - b + delta / 2 + caca_rand(-40, 40)) / delta;
                else if( g == max )
                    hue = 3 + (float)(b - r + delta / 2 + caca_rand(-40, 40)) / delta;
                else
                    hue = 5 + (float)(r - g + delta / 2 + caca_rand(-40, 40)) / delta;

                if(val > caca_rand(128, 192))
                    caca_set_color(light_colors[hue]);
                else
                    caca_set_color(dark_colors[hue]);
            }
            else
            {
                caca_set_color(CACA_COLOR_LIGHTGRAY);
            }

            caca_putchar(x, y, foo[(r + g + b + caca_rand(-10, 10)) / 3 / 25]);
        }
}

