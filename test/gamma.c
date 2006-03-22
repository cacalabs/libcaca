/*
 *  gamma         libcucul gamma test program
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

uint32_t buffer[256 * 4];

int main(void)
{
    cucul_t *qq;
    caca_t *kk;

    struct cucul_bitmap *left, *right;
    int x;

    qq = cucul_init(0, 0);
    kk = caca_attach(qq);

    for(x = 0; x < 256; x++)
    {
        buffer[x] = (x << 16) | (x << 8) | (x<< 0);
        buffer[x + 256] = (0xff << 16) |    (x << 8) | (0x00 << 0);
        buffer[x + 512] = (0x00 << 16) | (0xff << 8) |    (x << 0);
        buffer[x + 768] =    (x << 16) | (0x00 << 8) | (0xff << 0);
    }

    left = cucul_create_bitmap(qq, 32, 256, 4, 4 * 256,
                               0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    right = cucul_create_bitmap(qq, 32, 256, 4, 4 * 256,
                                0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    caca_set_delay(kk, 20000);

    for(x = 0; ; x = (x + 1) % 256)
    {
        float g = (x > 128) ? (256.0 + 8.0 - x) / 64.0 : (8.0 + x) / 64.0;

        if(caca_get_event(kk, CACA_EVENT_KEY_PRESS))
            break;

        cucul_draw_bitmap(qq, 0, cucul_get_height(qq) / 2,
                          cucul_get_width(qq) - 1, cucul_get_height(qq) - 1,
                          left, buffer);

        cucul_set_bitmap_gamma(right, g);
        cucul_draw_bitmap(qq, 0, 0,
                          cucul_get_width(qq) - 1, cucul_get_height(qq) / 2 - 1,
                          right, buffer);

        cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_printf(qq, 2, 1, "gamma=%g", g);

        caca_display(kk);
    }

    cucul_free_bitmap(left);
    cucul_free_bitmap(right);

    caca_detach(kk);
    cucul_end(qq);

    return 0;
}

