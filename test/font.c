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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cucul.h"

int main(int argc, char *argv[])
{
    cucul_t *qq;
    struct cucul_font *f;
    unsigned char *buf;
    unsigned int x, y, w, h;
    char const * const * fonts;

    /* Create a canvas */
    qq = cucul_create(8, 2);

    /* Draw stuff on our canvas */
    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 0, 0, "ABcde\\o/");
    cucul_putstr(qq, 0, 1, "&$âøÿ░▒█");

    /* Load a libcucul internal font */
    fonts = cucul_get_font_list();
    if(fonts[0] == NULL)
    {
        fprintf(stderr, "error: libcucul was compiled without any fonts\n");
        return -1;
    }
    f = cucul_load_font(fonts[0], 0);

    /* Create our bitmap buffer (32-bit ARGB) */
    w = cucul_get_width(qq) * cucul_get_font_width(f);
    h = cucul_get_height(qq) * cucul_get_font_height(f);
    buf = malloc(4 * w * h);

    /* Render the canvas onto our image buffer */
    cucul_render_canvas(qq, f, buf, w, h, 4 * w);

    /* Just for fun, output the image on the terminal using ASCII art */
    for(y = 0; y < h; y++)
    {
        for(x = 0; x < w; x++)
        {
            static const char list[] = {
                ' ', '.', ':', 't', 'S', 'R', '#', '@'
            };

            printf("%c", list[buf[4 * (y * w + x) + 3] / 0x20]);
        }
        printf("\n");
    }

    /* Free everything */
    free(buf);
    cucul_free_font(f);
    cucul_free(qq);

    return 0;
}

