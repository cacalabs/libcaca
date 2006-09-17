/*
 *  hsv           libcaca HSV rendering test program
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
#include "common.h"

#if !defined(__KERNEL__)
#   if defined(HAVE_INTTYPES_H)
#      include <inttypes.h>
#   endif
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

uint32_t buffer[256*256];

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    caca_display_t *dp;

    cucul_dither_t *dither;
    int x, y;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    for(y = 0; y < 256; y++)
        for(x = 0; x < 256; x++)
    {
        buffer[y * 256 + x] = ((y * x / 256) << 16) | ((y * x / 256) << 8) | (x<< 0);
    }

    dither = cucul_create_dither(32, 256, 256, 4 * 256,
                                 0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    cucul_dither_bitmap(cv, 0, 0, cucul_get_canvas_width(cv),
                        cucul_get_canvas_height(cv), dither, buffer);
    cucul_free_dither(dither);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

