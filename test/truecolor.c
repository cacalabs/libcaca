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
    cucul_t *qq;
    caca_t *kk;

    int x, y;

    qq = cucul_create(32, 16);
    kk = caca_attach(qq);

    for(y = 0; y < 16; y++)
        for(x = 0; x < 16; x++)
    {
        uint16_t bgcolor = 0xff00 | (y << 4) | x;
        uint16_t fgcolor = 0xf000 | ((15 - y) << 4) | ((15 - x) << 8);

        cucul_set_truecolor(qq, fgcolor, bgcolor);
        cucul_putstr(qq, x * 2, y, "CA");
    }

    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_LIGHTBLUE);
    cucul_putstr(qq, 2, 1, " truecolor libcaca ");

    caca_display(kk);

    caca_get_event(kk, CACA_EVENT_KEY_PRESS, &ev, -1);

    caca_detach(kk);
    cucul_free(qq);

    return 0;
}

