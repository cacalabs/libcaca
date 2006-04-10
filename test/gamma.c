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

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <math.h>
#endif

#include "cucul.h"
#include "caca.h"

uint32_t buffer[256 * 4];

int main(void)
{
    struct caca_event ev;
    cucul_t *qq, *gg, *mask;
    caca_t *kk;
    struct cucul_dither *left, *right;
    float gam = 1.0;
    int x;

    qq = cucul_create(0, 0);
    kk = caca_attach(qq);

    gg = cucul_create(cucul_get_width(qq), cucul_get_height(qq));
    mask = cucul_create(cucul_get_width(qq), cucul_get_height(qq));

    for(x = 0; x < 256; x++)
    {
        buffer[x] = (x << 16) | (x << 8) | (x<< 0);
        buffer[x + 256] = (0xff << 16) |    (x << 8) | (0x00 << 0);
        buffer[x + 512] = (0x00 << 16) | (0xff << 8) |    (x << 0);
        buffer[x + 768] =    (x << 16) | (0x00 << 8) | (0xff << 0);
    }

    left = cucul_create_dither(32, 256, 4, 4 * 256,
                               0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    right = cucul_create_dither(32, 256, 4, 4 * 256,
                                0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    caca_set_delay(kk, 20000);

    for(x = 0; ; x++)
    {
        int ret = caca_get_event(kk, CACA_EVENT_KEY_PRESS, &ev, 0);

        if(ret)
        {
            if(ev.data.key.c == CACA_KEY_LEFT)
                gam /= 1.03;
            else if(ev.data.key.c == CACA_KEY_RIGHT)
                gam *= 1.03;
            else if(ev.data.key.c == CACA_KEY_DOWN)
                gam = 1.0;
            else if(ev.data.key.c == CACA_KEY_ESCAPE)
                break;
        }

        /* Resize the spare canvas, just in case the main one changed */
        cucul_set_size(gg, cucul_get_width(qq), cucul_get_height(qq));
        cucul_set_size(mask, cucul_get_width(qq), cucul_get_height(qq));

        /* Draw the regular dither on the main canvas */
        cucul_dither_bitmap(qq, 0, 0,
                            cucul_get_width(qq) - 1, cucul_get_height(qq) - 1,
                            left, buffer);

        /* Draw the gamma-modified dither on the spare canvas */
        cucul_set_dither_gamma(right, gam);
        cucul_dither_bitmap(gg, 0, 0,
                            cucul_get_width(gg) - 1, cucul_get_height(gg) - 1,
                            right, buffer);

        /* Draw something on the mask */
        cucul_clear(mask);
        cucul_set_color(mask, CUCUL_COLOR_WHITE, CUCUL_COLOR_WHITE);
        cucul_fill_ellipse(mask, (1.0 + sin(0.05 * (float)x))
                                   * 0.5 * cucul_get_width(mask),
                                 (1.0 + cos(0.05 * (float)x))
                                   * 0.5 * cucul_get_height(mask),
                                 cucul_get_width(mask) / 2,
                                 cucul_get_height(mask) / 2, "#");

        /* Blit the spare canvas onto the first one */
        cucul_blit(qq, 0, 0, gg, mask);

        cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_printf(qq, 2, 1,
                     "gamma=%g - use arrows to change, Esc to quit", gam);

        caca_display(kk);
    }

    cucul_free_dither(left);
    cucul_free_dither(right);

    caca_detach(kk);
    cucul_free(qq);

    return 0;
}

