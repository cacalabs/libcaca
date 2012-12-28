/*
 *  font          libcaca font test program
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
#   if defined(HAVE_ENDIAN_H)
#      include <endian.h>
#   endif

#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "caca.h"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
    caca_display_t *dp;
    caca_font_t *f;
    caca_dither_t *d;
    uint8_t *buf;
    unsigned int w, h;
    char const * const * fonts;

    /* Create a canvas */
    cv = caca_create_canvas(8, 2);
    if(cv == NULL)
    {
        printf("Can't create canvas\n");
        return -1;
    }


    /* Draw stuff on our canvas */
    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLACK);
    caca_put_str(cv, 0, 0, "ABcde");
    caca_set_color_ansi(cv, CACA_LIGHTRED, CACA_BLACK);
    caca_put_str(cv, 5, 0, "\\o/");
    caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
    caca_put_str(cv, 0, 1, "&$âøÿØ?!");

    /* Load a libcaca internal font */
    fonts = caca_get_font_list();
    if(fonts[0] == NULL)
    {
        fprintf(stderr, "error: libcaca was compiled without any fonts\n");
        return -1;
    }
    f = caca_load_font(fonts[0], 0);
    if(f == NULL)
    {
        fprintf(stderr, "error: could not load font \"%s\"\n", fonts[0]);
        return -1;
    }

    /* Create our bitmap buffer (32-bit ARGB) */
    w = caca_get_canvas_width(cv) * caca_get_font_width(f);
    h = caca_get_canvas_height(cv) * caca_get_font_height(f);
    buf = malloc(4 * w * h);

    /* Render the canvas onto our image buffer */
    caca_render_canvas(cv, f, buf, w, h, 4 * w);

    /* Just for fun, render the image using libcaca */
    caca_set_canvas_size(cv, 80, 32);
    dp = caca_create_display(cv);

    {
#if defined(HAVE_ENDIAN_H)
        if(__BYTE_ORDER == __BIG_ENDIAN)
#else
        /* This is compile-time optimised with at least -O1 or -Os */
        uint32_t const tmp = 0x12345678;
        if(*(uint8_t const *)&tmp == 0x12)
#endif
            d = caca_create_dither(32, w, h, 4 * w,
                                    0xff0000, 0xff00, 0xff, 0xff000000);
        else
            d = caca_create_dither(32, w, h, 4 * w,
                                    0xff00, 0xff0000, 0xff000000, 0xff);
    }

    caca_dither_bitmap(cv, 0, 0, caca_get_canvas_width(cv),
                                  caca_get_canvas_height(cv), d, buf);
    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    /* Free everything */
    caca_free_display(dp);
    free(buf);
    caca_free_dither(d);
    caca_free_font(f);
    caca_free_canvas(cv);

    return 0;
}

