/*
 *  hsv           libcaca HSV rendering test program
 *  Copyright (c) 2003-2012 Sam Hocevar <sam@hocevar.net>
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

uint32_t buffer[256*256];

int main(int argc, char *argv[])
{
    caca_display_t *dp;
    caca_canvas_t *cv;

    caca_dither_t *dither;
    int x, y;

    dp = caca_create_display(NULL);
    if(dp == NULL)
    {
        printf("Can't create display\n");
        return -1;
    }

    cv = caca_get_canvas(dp);

    for(y = 0; y < 256; y++)
        for(x = 0; x < 256; x++)
    {
        buffer[y * 256 + x] = ((y * x / 256) << 16) | ((y * x / 256) << 8) | (x<< 0);
    }

    dither = caca_create_dither(32, 256, 256, 4 * 256,
                                 0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    caca_dither_bitmap(caca_get_canvas(dp), 0, 0, caca_get_canvas_width(cv),
                        caca_get_canvas_height(cv), dither, buffer);
    caca_free_dither(dither);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);

    return 0;
}

