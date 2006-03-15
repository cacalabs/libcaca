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

#define WIDTH 60
#define HEIGHT 32

uint32_t pixels[256*256];

int main(int argc, char *argv[])
{
    cucul_t *qq;
    enum cucul_format format;
    struct cucul_bitmap *bitmap;
    struct cucul_buffer *buffer;
    int x, y;

    if(argc != 2)
    {
        fprintf(stderr, "%s: wrong argument count\n", argv[0]);
        fprintf(stderr, "usage: %s <format>\n", argv[0]);
        fprintf(stderr, "where <format> is one of: ansi, html, html3, irc, ps, svg\n");
        exit(-1);
    }

    if(!strcasecmp(argv[1], "ansi"))
        format = CUCUL_FORMAT_ANSI;
    else if(!strcasecmp(argv[1], "html"))
        format = CUCUL_FORMAT_HTML;
    else if(!strcasecmp(argv[1], "html3"))
        format = CUCUL_FORMAT_HTML3;
    else if(!strcasecmp(argv[1], "irc"))
        format = CUCUL_FORMAT_IRC;
    else if(!strcasecmp(argv[1], "ps"))
        format = CUCUL_FORMAT_PS;
    else if(!strcasecmp(argv[1], "svg"))
        format = CUCUL_FORMAT_SVG;
    else
    {
        fprintf(stderr, "%s: unknown format `%s'\n", argv[0], argv[1]);
        exit(-1);
    }

    qq = cucul_init();
    cucul_set_size(qq, WIDTH, HEIGHT);

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

    bitmap = cucul_create_bitmap(qq, 32, 256, 256, 4 * 256,
                                 0x00ff0000, 0x0000ff00, 0x000000ff, 0x0);
    cucul_draw_bitmap(qq, 0, 0,
                      cucul_get_width(qq) - 1, cucul_get_height(qq) - 1,
                      bitmap, pixels);
    cucul_free_bitmap(qq, bitmap);

    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLACK);
    cucul_draw_thin_box(qq, 0, 0, WIDTH - 1, HEIGHT - 1);

    cucul_set_color(qq, CUCUL_COLOR_BLACK, CUCUL_COLOR_WHITE);
    cucul_fill_ellipse(qq, WIDTH / 2, HEIGHT / 2, WIDTH / 4, HEIGHT / 4, ' ');
    cucul_putstr(qq, WIDTH / 2 - 1, HEIGHT / 2 - 4, "\\o/");
    cucul_putstr(qq, WIDTH / 2 - 1, HEIGHT / 2 + 4, "\\o/");

    cucul_set_color(qq, CUCUL_COLOR_WHITE, CUCUL_COLOR_LIGHTBLUE);
    cucul_putstr(qq, WIDTH / 2 - 7, HEIGHT / 2, "    LIBCACA    ");

    buffer = cucul_export(qq, format);
    fwrite(buffer->buffer, buffer->size, 1, stdout);
    cucul_free(buffer);

    cucul_end(qq);

    return 0;
}

