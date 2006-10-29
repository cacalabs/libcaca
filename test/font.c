/*
 *  font          libcucul font test program
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
#include "common.h"

#if !defined(__KERNEL__)
#   if defined(HAVE_INTTYPES_H)
#      include <inttypes.h>
#   endif

#   if defined(HAVE_ENDIAN_H)
#      include <endian.h>
#   endif

#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "caca.h"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    caca_display_t *dp;
    cucul_font_t *f;
    cucul_dither_t *d;
    unsigned char *buf;
    unsigned int w, h;
    char const * const * fonts;

    /* Create a canvas */
    cv = cucul_create_canvas(8, 2);

    /* Draw stuff on our canvas */
    cucul_set_attr(cv, cucul_ansi_to_attr(CUCUL_WHITE, CUCUL_BLACK));
    cucul_putstr(cv, 0, 0, "ABcde");
    cucul_set_attr(cv, cucul_ansi_to_attr(CUCUL_LIGHTRED, CUCUL_BLACK));
    cucul_putstr(cv, 5, 0, "\\o/");
    cucul_set_attr(cv, cucul_ansi_to_attr(CUCUL_WHITE, CUCUL_BLUE));
    cucul_putstr(cv, 0, 1, "&$âøÿØ?!");

    /* Load a libcucul internal font */
    fonts = cucul_get_font_list();
    if(fonts[0] == NULL)
    {
        fprintf(stderr, "error: libcucul was compiled without any fonts\n");
        return -1;
    }
    f = cucul_load_font(fonts[0], 0);
    if(f == NULL)
    {
        fprintf(stderr, "error: could not load font \"%s\"\n", fonts[0]);
        return -1;
    }

    /* Create our bitmap buffer (32-bit ARGB) */
    w = cucul_get_canvas_width(cv) * cucul_get_font_width(f);
    h = cucul_get_canvas_height(cv) * cucul_get_font_height(f);
    buf = malloc(4 * w * h);

    /* Render the canvas onto our image buffer */
    cucul_render_canvas(cv, f, buf, w, h, 4 * w);

    /* Just for fun, render the image using libcaca */
    cucul_set_canvas_size(cv, 80, 32);
    dp = caca_create_display(cv);

#if defined(HAVE_ENDIAN_H)
    if(__BYTE_ORDER == __BIG_ENDIAN)
#else
    /* This is compile-time optimised with at least -O1 or -Os */
    uint32_t const tmp = 0x12345678;
    if(*(uint8_t const *)&tmp == 0x12)
#endif
        d = cucul_create_dither(32, w, h, 4 * w,
                                0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    else
        d = cucul_create_dither(32, w, h, 4 * w,
                                0x0000ff00, 0x00ff0000, 0xff000000, 0x000000ff);

    cucul_dither_bitmap(cv, 0, 0, cucul_get_canvas_width(cv),
                                  cucul_get_canvas_height(cv), d, buf);
    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    /* Free everything */
    caca_free_display(dp);
    free(buf);
    cucul_free_dither(d);
    cucul_free_font(f);
    cucul_free_canvas(cv);

    return 0;
}

