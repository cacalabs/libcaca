/*
 *  gamma         libcucul gamma test program
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
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
#   if defined(HAVE_INTTYPES_H)
#      include <inttypes.h>
#   endif
#   include <stdio.h>
#   include <math.h>
#endif

#include "cucul.h"
#include "caca.h"

uint32_t buffer[256 * 4];

int main(int argc, char *argv[])
{
    caca_event_t ev;
    cucul_canvas_t *cv, *cw, *mask;
    caca_display_t *dp;
    cucul_dither_t *left, *right;
    float gam = 1.0;
    int x;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    cw = cucul_create_canvas(cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));
    mask = cucul_create_canvas(cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));

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
    caca_set_display_time(dp, 20000);

    for(x = 0; ; x++)
    {
        int ret = caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0);

        if(ret)
        {
            if(ev.data.key.ch == CACA_KEY_LEFT)
                gam /= 1.03;
            else if(ev.data.key.ch == CACA_KEY_RIGHT)
                gam *= 1.03;
            else if(ev.data.key.ch == CACA_KEY_DOWN)
                gam = 1.0;
            else if(ev.data.key.ch == CACA_KEY_ESCAPE)
                break;
        }

        /* Resize the spare canvas, just in case the main one changed */
        cucul_set_canvas_size(cw, cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));
        cucul_set_canvas_size(mask, cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));

        /* Draw the regular dither on the main canvas */
        cucul_dither_bitmap(cv, 0, 0, cucul_get_canvas_width(cv),
                            cucul_get_canvas_height(cv), left, buffer);

        /* Draw the gamma-modified dither on the spare canvas */
        cucul_set_dither_gamma(right, gam);
        cucul_dither_bitmap(cw, 0, 0, cucul_get_canvas_width(cw),
                            cucul_get_canvas_height(cw), right, buffer);

        /* Draw something on the mask */
        cucul_set_color_ansi(mask, CUCUL_LIGHTGRAY, CUCUL_BLACK);
        cucul_clear_canvas(mask);
        cucul_set_color_ansi(mask, CUCUL_WHITE, CUCUL_WHITE);
        cucul_fill_ellipse(mask, (1.0 + sin(0.05 * (float)x))
                                   * 0.5 * cucul_get_canvas_width(mask),
                                 (1.0 + cos(0.05 * (float)x))
                                   * 0.5 * cucul_get_canvas_height(mask),
                                 cucul_get_canvas_width(mask) / 2,
                                 cucul_get_canvas_height(mask) / 2, '#');

        /* Blit the spare canvas onto the first one */
        cucul_blit(cv, 0, 0, cw, mask);

        cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLUE);
        cucul_printf(cv, 2, 1,
                     "gamma=%g - use arrows to change, Esc to quit", gam);

        caca_refresh_display(dp);
    }

    cucul_free_dither(left);
    cucul_free_dither(right);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

