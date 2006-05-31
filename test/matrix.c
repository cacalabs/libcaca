/*
 *  matrix        matrix effect with HDAPS support
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
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
#include "common.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <math.h>
#endif

#include "cucul.h"
#include "caca.h"

#define MAXDROPS 500
#define MINLEN 15
#define MAXLEN 30

struct drop
{
    int x, y, speed, len;
    char str[MAXLEN];
}
drop[MAXDROPS];

int main(void)
{
    cucul_canvas_t *cv;
    caca_display_t *dp;
    int i, j;
    int w, h, xo, yo, dx = 0, dy = 0, newdx, newdy;
    int have_hdaps = 1;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    caca_set_display_time(dp, 20000);

    for(i = 0; i < MAXDROPS; i++)
    {
        drop[i].x = cucul_rand(0, 1000);
        drop[i].y = cucul_rand(0, 1000);
        drop[i].speed = 5 + cucul_rand(0, 30);
        drop[i].len = MINLEN + cucul_rand(0, (MAXLEN - MINLEN));
        for(j = 0; j < MAXLEN; j++)
            drop[i].str[j] = cucul_rand('0', 'z');
    }

    for(;;)
    {
        int sensorx = -387, sensory = -387;

        /* IBM HDAPS support */
        if(have_hdaps)
        {
            FILE *f = fopen("/sys/devices/platform/hdaps/position", "r");

            if(f)
            {
                fscanf(f, "(%d,%d)", &sensorx, &sensory);
                fclose(f);
            }
            else
                have_hdaps = 0;
        }

        w = cucul_get_canvas_width(cv);
        h = cucul_get_canvas_height(cv);
        xo = w / 2;
        yo = h / 2;
        //newdx = 1000.0 * cos((M_PI / 360.0) * (sensorx + 387));
        //newdy = -1000.0 * sin((M_PI / 360.0) * (sensorx + 387));
        newdx = sensorx + 387;
        newdy = 100;

        if(newdx > -10 && newdx < 10)
            newdx = 0;

        dx = (newdx + 3 * dx) / 4;
        dy = (newdy + 3 * dy) / 4;

        cucul_clear_canvas(cv);

        for(i = 0; i < MAXDROPS && i < (w * h / 32); i++)
        {
            int x, y;

            drop[i].y += drop[i].speed;
            if(drop[i].y > 1000)
            {
                drop[i].y -= 1000;
                drop[i].x = cucul_rand(0, 1000);
            }

            x = drop[i].x * w / 1000 / 2 * 2;
            y = drop[i].y * (h + MAXLEN) / 1000;
            x += (y - h / 2) * dx / dy;

            for(j = 0; j < drop[i].len; j++)
            {
                unsigned int fg;

                if(j < 2)
                    fg = CUCUL_COLOR_WHITE;
                else if(j < drop[i].len / 4)
                    fg = CUCUL_COLOR_LIGHTGREEN;
                else if(j < drop[i].len * 4 / 5)
                    fg = CUCUL_COLOR_GREEN;
                else
                    fg = CUCUL_COLOR_DARKGRAY;
                cucul_set_color(cv, fg, CUCUL_COLOR_BLACK);

                cucul_putchar(cv, (x * dy + dy / 2 - j * dx) / dy, y - j,
                              drop[i].str[(y - j) % drop[i].len]);
            }
        }

        caca_refresh_display(dp);

        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, 0))
            break;
    }

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

