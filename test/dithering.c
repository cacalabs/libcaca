/*
 *  dithering     libcaca dithering test program
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#include "cucul.h"
#include "caca.h"

#define XRATIO 100*100
#define YRATIO 70*70
#define FUZZY 5000000

enum cucul_color points[] =
{
    CUCUL_COLOR_BLACK,
    CUCUL_COLOR_DARKGRAY,
    CUCUL_COLOR_LIGHTGRAY,
    CUCUL_COLOR_WHITE,
    CUCUL_COLOR_RED,
    CUCUL_COLOR_LIGHTRED
};

char density[] = " -,+:;o&%w$W@#";

int main(void)
{
    struct caca_event ev;
    cucul_t *qq;
    caca_t *kk;
    int neara, dista, nearb, distb, dist;
    int x, y;

    qq = cucul_create(0, 0);
    kk = caca_attach(qq);

    for(x = 0; x < 100; x++)
        for(y = 0; y < 100; y++)
    {
        char ch = '?';

        /* distance to black */
        dista = XRATIO * x * x;
        neara = 0;

        /* distance to 40% */
        dist = XRATIO * (x - 40) * (x - 40) + YRATIO * y * y;
        if(cucul_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 1; dista = dist;
        }
        else
        {
            nearb = 1; distb = dist;
        }

        /* check dist to 70% */
        dist = XRATIO * (x - 70) * (x - 70) + YRATIO * y * y;
        if(cucul_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 2; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 2; distb = dist;
        }

        /* check dist to white */
        dist = XRATIO * (x - 100) * (x - 100) + YRATIO * y * y;
        if(cucul_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 3; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 3; distb = dist;
        }

#if 1
        /* check dist to dark */
        dist = XRATIO * (x - 40) * (x - 40) + YRATIO * (y - 100) * (y - 100);
        dist = dist * 12 / 16;
        if(cucul_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 4; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 4; distb = dist;
        }

        /* check dist to light */
        dist = XRATIO * (x - 100) * (x - 100) + YRATIO * (y - 100) * (y - 100);
        dist = dist * 8 / 16;
        if(cucul_rand(-FUZZY, FUZZY) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 5; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY) + dist < distb)
        {
            nearb = 5; distb = dist;
        }
#endif

        /* dista can be > distb because of dithering fuzziness */
        if(dista > distb)
            ch = density[distb * 2 * 13 / (dista + distb)];
        else
            ch = density[dista * 2 * 13 / (dista + distb)];
        cucul_set_color(qq, points[nearb], points[neara]);

        cucul_putchar(qq, x * cucul_get_width(qq) / 100,
                          (100 - y) * cucul_get_height(qq) / 100, ch);
    }

    caca_display(kk);

    caca_get_event(kk, CACA_EVENT_KEY_PRESS, &ev, -1);

    caca_detach(kk);
    cucul_free(qq);

    return 0;
}

