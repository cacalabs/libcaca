/*
 *  truecolor     truecolor canvas features
 *  Copyright (c) 2006-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "caca.h"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
    caca_display_t *dp;

    int x, y;

    cv = caca_create_canvas(32, 16);
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

    for(y = 0; y < 16; y++)
        for(x = 0; x < 16; x++)
    {
        uint16_t bgcolor = 0xff00 | (y << 4) | x;
        uint16_t fgcolor = 0xf000 | ((15 - y) << 4) | ((15 - x) << 8);

        caca_set_color_argb(cv, fgcolor, bgcolor);
        caca_put_str(cv, x * 2, y, "CA");
    }

    caca_set_color_ansi(cv, CACA_WHITE, CACA_LIGHTBLUE);
    caca_put_str(cv, 2, 1, " truecolor libcaca ");

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    caca_free_canvas(cv);

    return 0;
}

