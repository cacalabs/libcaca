/*
 *  export        libcucul export test program
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
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"

#define WIDTH 80
#define HEIGHT 32

uint32_t pixels[256*256];

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    cucul_dither_t *dither;
    cucul_buffer_t *buffer;
    char *file, *format;
    char const * const * exports, * const * p;
    int x, y;

    exports = cucul_get_export_list();

    if(argc < 2 || argc > 3)
    {
        fprintf(stderr, "%s: wrong argument count\n", argv[0]);
        fprintf(stderr, "usage: %s [file] <format>\n", argv[0]);
        fprintf(stderr, "where <format> is one of:\n");
        for(p = exports; *p; p += 2)
            fprintf(stderr, " \"%s\" (%s)\n", *p, *(p + 1));
        exit(-1);
    }

    if(argc == 2)
    {
        file = NULL;
        format = argv[1];
    }
    else
    {
        file = argv[1];
        format = argv[2];
    }

    for(p = exports; *p; p += 2)
        if(!strcasecmp(format, *p))
            break;

    if(!*p)
    {
        fprintf(stderr, "%s: unknown format `%s'\n", argv[0], format);
        fprintf(stderr, "please use one of:\n");
        for(p = exports; *p; p += 2)
            fprintf(stderr, " \"%s\" (%s)\n", *p, *(p + 1));
        exit(-1);
    }

    if(file)
    {
        cucul_buffer_t *tmp;
        tmp = cucul_load_file(file);
        if(!tmp)
        {
            fprintf(stderr, "%s: could not load `%s'\n", argv[0], file);
            exit(-1);
        }
        cv = cucul_import_canvas(tmp, "");
        if(!cv)
        {
            fprintf(stderr, "%s: `%s' has unknown format\n", argv[0], file);
            exit(-1);
        }
        cucul_free_buffer(tmp);
    }
    else
    {
        cv = cucul_create_canvas(WIDTH, HEIGHT);

        for(y = 0; y < 256; y++)
        {
            for(x = 0; x < 256; x++)
            {
                uint32_t r = x;
                uint32_t g = (255 - y + x) / 2;
                uint32_t b = y * (255 - x) / 256;
                pixels[y * 256 + x] = (r << 16) | (g << 8) | (b << 0);
            }
        }

        dither = cucul_create_dither(32, 256, 256, 4 * 256,
                                     0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
        if(!strcmp(format, "ansi") || !strcmp(format, "utf8"))
            cucul_set_dither_charset(dither, "shades");
        cucul_dither_bitmap(cv, 0, 0, cucul_get_canvas_width(cv),
                            cucul_get_canvas_height(cv), dither, pixels);
        cucul_free_dither(dither);

        cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLACK);
        cucul_draw_thin_box(cv, 0, 0, WIDTH - 1, HEIGHT - 1);

        cucul_set_color_ansi(cv, CUCUL_BLACK, CUCUL_WHITE);
        cucul_fill_ellipse(cv, WIDTH / 2, HEIGHT / 2,
                               WIDTH / 4, HEIGHT / 4, " ");
        cucul_putstr(cv, WIDTH / 2 - 5, HEIGHT / 2 - 5, "(\") \\o/ <&>");
        cucul_putstr(cv, WIDTH / 2 - 8, HEIGHT / 2 - 3, "[<><><><> <>--<>]");
        cucul_putstr(cv, WIDTH / 2 - 8, HEIGHT / 2 - 2, "[ドラゴン ボーレ]");
        cucul_putstr(cv, WIDTH / 2 - 7, HEIGHT / 2 + 2, "äβç ░▒▓█▓▒░ ΔЗҒ");

        cucul_set_attr(cv, CUCUL_BOLD);
        cucul_putstr(cv, WIDTH / 2 - 16, HEIGHT / 2 + 3, "Bold");
        cucul_set_attr(cv, CUCUL_BLINK);
        cucul_putstr(cv, WIDTH / 2 - 9, HEIGHT / 2 + 3, "Blink");
        cucul_set_attr(cv, CUCUL_ITALICS);
        cucul_putstr(cv, WIDTH / 2 - 1, HEIGHT / 2 + 3, "Italics");
        cucul_set_attr(cv, CUCUL_UNDERLINE);
        cucul_putstr(cv, WIDTH / 2 + 8, HEIGHT / 2 + 3, "Underline");
        cucul_set_attr(cv, 0);

        cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_LIGHTBLUE);
        cucul_putstr(cv, WIDTH / 2 - 7, HEIGHT / 2, "    LIBCACA    ");

        for(x = 0; x < 16; x++)
        {
            cucul_set_color_argb(cv, 0xff00 | x, 0xf00f | (x << 4));
            cucul_putstr(cv, WIDTH / 2 - 7 + x, HEIGHT / 2 + 5, "#");
        }
    }

    buffer = cucul_export_canvas(cv, format);
    fwrite(cucul_get_buffer_data(buffer),
           cucul_get_buffer_size(buffer), 1, stdout);
    cucul_free_buffer(buffer);

    cucul_free_canvas(cv);

    return 0;
}

