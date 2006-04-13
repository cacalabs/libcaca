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

extern uint8_t font_monospace9[];

int main(int argc, char *argv[])
{
    cucul_t *qq;
    struct cucul_font *f;
    unsigned char *buf;
    unsigned int x, y, w, h;

    qq = cucul_create(5, 2);
    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLACK);
    cucul_putstr(qq, 0, 0, "ABcde");
    cucul_putstr(qq, 0, 1, "&$âøÿ");

    f = cucul_load_font(font_monospace9, 700000);

    w = cucul_get_width(qq) * cucul_get_font_width(f);
    h = cucul_get_height(qq) * cucul_get_font_height(f);
    buf = malloc(4 * w * h);

    cucul_render_canvas(qq, f, buf, w, h, 4 * w);

    for(y = 0; y < h; y++)
    {
        for(x = 0; x < w; x++)
        {
            printf("%.02x", buf[4 * (y * w + x) + 3]);
        }
        printf("\n");
    }

    free(buf);
    cucul_free_font(f);
    cucul_free(qq);

    return 0;
}

