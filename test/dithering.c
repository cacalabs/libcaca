/*
 *  dithering     libcaca dithering test program
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

#include "config.h"

#include "caca.h"

#define XRATIO 100*100
#define YRATIO 70*70
#define FUZZY 5000000

enum caca_color points[] =
{
    CACA_COLOR_BLACK,
    CACA_COLOR_DARKGRAY,
    CACA_COLOR_LIGHTGRAY,
    CACA_COLOR_WHITE,
    CACA_COLOR_RED,
    CACA_COLOR_LIGHTRED
};

char density[] = " -,+:;o&%w$W@#";

int main(void)
{
    int neara, dista, nearb, distb, dist;
    int x, y;

    caca_init();

    for(x = 0; x < 100; x++)
        for(y = 0; y < 100; y++)
    {
        char ch = '?';

        /* distance to black */
        dista = XRATIO * x * x;
        neara = 0;

        /* distance to 40% */
        dist = XRATIO * (x - 40) * (x - 40) + YRATIO * y * y;
        if(caca_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 1; dista = dist;
        }
        else
        {
            nearb = 1; distb = dist;
        }

        /* check dist to 70% */
        dist = XRATIO * (x - 70) * (x - 70) + YRATIO * y * y;
        if(caca_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 2; dista = dist;
        }
        else if(caca_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 2; distb = dist;
        }

        /* check dist to white */
        dist = XRATIO * (x - 100) * (x - 100) + YRATIO * y * y;
        if(caca_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 3; dista = dist;
        }
        else if(caca_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 3; distb = dist;
        }

#if 1
        /* check dist to dark */
        dist = XRATIO * (x - 40) * (x - 40) + YRATIO * (y - 100) * (y - 100);
        dist = dist * 12 / 16;
        if(caca_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 4; dista = dist;
        }
        else if(caca_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 4; distb = dist;
        }

        /* check dist to light */
        dist = XRATIO * (x - 100) * (x - 100) + YRATIO * (y - 100) * (y - 100);
        dist = dist * 8 / 16;
        if(caca_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 5; dista = dist;
        }
        else if(caca_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 5; distb = dist;
        }
#endif

        /* dista can be > distb because of dithering fuzziness */
        if(dista > distb)
            ch = density[distb * 2 * 13 / (dista + distb)];
        else
            ch = density[dista * 2 * 13 / (dista + distb)];
        caca_set_color(points[nearb], points[neara]);

        caca_putchar(x * caca_get_width() / 100, (100 - y) * caca_get_height() / 100, ch);
    }

    caca_refresh();

    while(!caca_get_event(CACA_EVENT_KEY_PRESS));

    caca_end();
    return 0;
}

