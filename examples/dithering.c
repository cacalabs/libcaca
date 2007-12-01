/*
 *  dithering     libcaca dithering test program
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

#define XRATIO 100*100
#define YRATIO 70*70
#define FUZZY 5000000

unsigned int points[] =
{
    CUCUL_BLACK, CUCUL_DARKGRAY, CUCUL_LIGHTGRAY,
    CUCUL_WHITE, CUCUL_RED, CUCUL_LIGHTRED
};

char density[] = " ',+:;o&%w$W@#";

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    caca_display_t *dp;
    int neara, dista, nearb, distb, dist;
    int x, y;

    cv = cucul_create_canvas(80, 24);
    if(cv == NULL)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Failed to create display\n");
        return 1;
    }



    for(x = 0; x < 100; x++)
        for(y = 0; y < 100; y++)
    {
        char ch = '?';

        /* distance to black */
        dista = XRATIO * x * x;
        neara = 0;

        /* distance to 40% */
        dist = XRATIO * (x - 40) * (x - 40) + YRATIO * y * y;
        if(cucul_rand(-FUZZY, FUZZY+1) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 1; dista = dist;
        }
        else
        {
            nearb = 1; distb = dist;
        }

        /* check dist to 70% */
        dist = XRATIO * (x - 70) * (x - 70) + YRATIO * y * y;
        if(cucul_rand(-FUZZY, FUZZY+1) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 2; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY+1) + dist < distb)
        {
            nearb = 2; distb = dist;
        }

        /* check dist to white */
        dist = XRATIO * (x - 100) * (x - 100) + YRATIO * y * y;
        if(cucul_rand(-FUZZY, FUZZY+1) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 3; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY+1) + dist < distb)
        {
            nearb = 3; distb = dist;
        }

#if 1
        /* check dist to dark */
        dist = XRATIO * (x - 40) * (x - 40) + YRATIO * (y - 100) * (y - 100);
        dist = dist * 12 / 16;
        if(cucul_rand(-FUZZY, FUZZY+1) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 4; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY+1) + dist < distb)
        {
            nearb = 4; distb = dist;
        }

        /* check dist to light */
        dist = XRATIO * (x - 100) * (x - 100) + YRATIO * (y - 100) * (y - 100);
        dist = dist * 8 / 16;
        if(cucul_rand(-FUZZY, FUZZY+1) + dist < dista)
        {
            nearb = neara; distb = dista; neara = 5; dista = dist;
        }
        else if(cucul_rand(-FUZZY, FUZZY+1) + dist < distb)
        {
            nearb = 5; distb = dist;
        }
#endif

        /* dista can be > distb because of dithering fuzziness */
        if(dista > distb)
            ch = density[distb * 2 * 13 / (dista + distb)];
        else
            ch = density[dista * 2 * 13 / (dista + distb)];
        cucul_set_color_ansi(cv, points[nearb], points[neara]);

        cucul_put_char(cv, x * cucul_get_canvas_width(cv) / 100,
                          (100 - y) * cucul_get_canvas_height(cv) / 100, ch);
    }

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}
