/*
 *  export        libcaca export test program
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
#   include <stdlib.h>
#   include <string.h>
#endif

#include "caca.h"

#define WIDTH 80
#define HEIGHT 32

uint32_t pixels[256*256];

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
    caca_dither_t *dither;
    void *buffer;
    char *file, *format;
    char const * const * exports, * const * p;
    size_t len;
    int x, y;

    exports = caca_get_export_list();

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
        cv = caca_create_canvas(0, 0);
        if(caca_import_canvas_from_file(cv, file, "") < 0)
        {
            fprintf(stderr, "%s: `%s' has unknown format\n", argv[0], file);
            exit(-1);
        }
    }
    else
    {
        cv = caca_create_canvas(WIDTH, HEIGHT);

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

        dither = caca_create_dither(32, 256, 256, 4 * 256,
                                     0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
        if(!strcmp(format, "ansi") || !strcmp(format, "utf8"))
            caca_set_dither_charset(dither, "shades");
        caca_dither_bitmap(cv, 0, 0, caca_get_canvas_width(cv),
                            caca_get_canvas_height(cv), dither, pixels);
        caca_free_dither(dither);

        caca_set_color_ansi(cv, CACA_WHITE, CACA_BLACK);
        caca_draw_thin_box(cv, 0, 0, WIDTH - 1, HEIGHT - 1);

        caca_set_color_ansi(cv, CACA_BLACK, CACA_WHITE);
        caca_fill_ellipse(cv, WIDTH / 2, HEIGHT / 2,
                               WIDTH / 4, HEIGHT / 4, ' ');

        caca_set_color_ansi(cv, CACA_LIGHTGRAY, CACA_BLACK);
        caca_put_str(cv, WIDTH / 2 - 12, HEIGHT / 2 - 6,
                      "   lightgray on black   ");
        caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
        caca_put_str(cv, WIDTH / 2 - 12, HEIGHT / 2 - 5,
                      " default on transparent ");
        caca_set_color_ansi(cv, CACA_BLACK, CACA_WHITE);
        caca_put_str(cv, WIDTH / 2 - 12, HEIGHT / 2 - 4,
                      "     black on white     ");

        caca_set_color_ansi(cv, CACA_BLACK, CACA_WHITE);
        caca_put_str(cv, WIDTH / 2 - 8, HEIGHT / 2 - 3, "[<><><><> <>--<>]");
        caca_put_str(cv, WIDTH / 2 - 8, HEIGHT / 2 - 2, "[ドラゴン ボーレ]");
        caca_put_str(cv, WIDTH / 2 - 7, HEIGHT / 2 + 2, "äβç ░▒▓█▓▒░ ΔЗҒ");
        caca_put_str(cv, WIDTH / 2 - 5, HEIGHT / 2 + 4, "(\") \\o/ <&>");

        caca_set_attr(cv, CACA_BOLD);
        caca_put_str(cv, WIDTH / 2 - 16, HEIGHT / 2 + 3, "Bold");
        caca_set_attr(cv, CACA_BLINK);
        caca_put_str(cv, WIDTH / 2 - 9, HEIGHT / 2 + 3, "Blink");
        caca_set_attr(cv, CACA_ITALICS);
        caca_put_str(cv, WIDTH / 2 - 1, HEIGHT / 2 + 3, "Italics");
        caca_set_attr(cv, CACA_UNDERLINE);
        caca_put_str(cv, WIDTH / 2 + 8, HEIGHT / 2 + 3, "Underline");
        caca_set_attr(cv, 0);

        caca_set_color_ansi(cv, CACA_WHITE, CACA_LIGHTBLUE);
        caca_put_str(cv, WIDTH / 2 - 7, HEIGHT / 2, "    LIBCACA    ");

        for(x = 0; x < 16; x++)
        {
            caca_set_color_argb(cv, 0xff00 | x, 0xf00f | (x << 4));
            caca_put_char(cv, WIDTH / 2 - 7 + x, HEIGHT / 2 + 6, '#');
        }
    }

    buffer = caca_export_canvas_to_memory(cv, format, &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    caca_free_canvas(cv);

    return 0;
}

