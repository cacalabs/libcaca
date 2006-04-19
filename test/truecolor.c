/*
 *  truecolor     truecolor canvas features
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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include "cucul.h"
#include "caca.h"

int main(void)
{
    caca_event_t ev;
    cucul_canvas_t *cv;
    caca_display_t *dp;

    int x, y;

    cv = cucul_create_canvas(32, 16);
    dp = caca_create_display(cv);

    for(y = 0; y < 16; y++)
        for(x = 0; x < 16; x++)
    {
        uint16_t bgcolor = 0xff00 | (y << 4) | x;
        uint16_t fgcolor = 0xf000 | ((15 - y) << 4) | ((15 - x) << 8);

        cucul_set_truecolor(cv, fgcolor, bgcolor);
        cucul_putstr(cv, x * 2, y, "CA");
    }

    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_LIGHTBLUE);
    cucul_putstr(cv, 2, 1, " truecolor libcaca ");

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

