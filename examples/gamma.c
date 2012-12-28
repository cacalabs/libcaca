/*
 *  gamma         libcaca gamma test program
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
#   include <math.h>
#endif

#include "caca.h"

uint32_t buffer[256 * 4];

int main(int argc, char *argv[])
{
    caca_event_t ev;
    caca_canvas_t *cv, *cw, *mask;
    caca_display_t *dp;
    caca_dither_t *left, *right;
    float gam;
    int x;

    cv = caca_create_canvas(0, 0);
    if(cv == NULL)
    {
        printf("Can't created canvas\n");
        return -1;
    }
    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Can't create display\n");
        return -1;
    }

    cw = caca_create_canvas(caca_get_canvas_width(cv), caca_get_canvas_height(cv));
    mask = caca_create_canvas(caca_get_canvas_width(cv), caca_get_canvas_height(cv));

    for(x = 0; x < 256; x++)
    {
        buffer[x] = (x << 16) | (x << 8) | (x<< 0);
        buffer[x + 256] = (0xff << 16) |    (x << 8) | (0x00 << 0);
        buffer[x + 512] = (0x00 << 16) | (0xff << 8) |    (x << 0);
        buffer[x + 768] =    (x << 16) | (0x00 << 8) | (0xff << 0);
    }

    left = caca_create_dither(32, 256, 4, 4 * 256,
                               0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    right = caca_create_dither(32, 256, 4, 4 * 256,
                                0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    gam = caca_get_dither_gamma(right);
    caca_set_display_time(dp, 20000);

    for(x = 0; ; x++)
    {
        int ret = caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0);

        if(ret)
        {
            if(caca_get_event_key_ch(&ev) == CACA_KEY_LEFT)
                gam /= 1.03;
            else if(caca_get_event_key_ch(&ev) == CACA_KEY_RIGHT)
                gam *= 1.03;
            else if(caca_get_event_key_ch(&ev) == CACA_KEY_DOWN)
                gam = 1.0;
            else if(caca_get_event_key_ch(&ev) == CACA_KEY_ESCAPE)
                break;
        }

        /* Resize the spare canvas, just in case the main one changed */
        caca_set_canvas_size(cw, caca_get_canvas_width(cv), caca_get_canvas_height(cv));
        caca_set_canvas_size(mask, caca_get_canvas_width(cv), caca_get_canvas_height(cv));

        /* Draw the regular dither on the main canvas */
        caca_dither_bitmap(cv, 0, 0, caca_get_canvas_width(cv),
                            caca_get_canvas_height(cv), left, buffer);

        /* Draw the gamma-modified dither on the spare canvas */
        caca_set_dither_gamma(right, gam);
        caca_dither_bitmap(cw, 0, 0, caca_get_canvas_width(cw),
                            caca_get_canvas_height(cw), right, buffer);

        /* Draw something on the mask */
        caca_set_color_ansi(mask, CACA_LIGHTGRAY, CACA_BLACK);
        caca_clear_canvas(mask);
        caca_set_color_ansi(mask, CACA_WHITE, CACA_WHITE);
        caca_fill_ellipse(mask, (1.0 + sin(0.05 * (float)x))
                                   * 0.5 * caca_get_canvas_width(mask),
                                 (1.0 + cos(0.05 * (float)x))
                                   * 0.5 * caca_get_canvas_height(mask),
                                 caca_get_canvas_width(mask) / 2,
                                 caca_get_canvas_height(mask) / 2, '#');

        /* Blit the spare canvas onto the first one */
        caca_blit(cv, 0, 0, cw, mask);

        caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
        caca_printf(cv, 2, 1,
                     "gamma=%g - use arrows to change, Esc to quit", gam);

        caca_refresh_display(dp);
    }

    caca_free_dither(left);
    caca_free_dither(right);

    caca_free_display(dp);
    caca_free_canvas(mask);
    caca_free_canvas(cw);
    caca_free_canvas(cv);

    return 0;
}

