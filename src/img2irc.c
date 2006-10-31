/*
 *  pic2irc       image to IRC converter
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
#   include <stdio.h>
#   include <string.h>
#   include <stdlib.h>
#endif

#include "cucul.h"
#include "common-image.h"

int main(int argc, char **argv)
{
    /* libcucul context */
    cucul_canvas_t *cv;
    cucul_buffer_t *export;
    struct image *i;
    int cols = 56, lines;

    if(argc != 2)
    {
        fprintf(stderr, "%s: wrong argument count\n", argv[0]);
        return 1;
    }

    cv = cucul_create_canvas(0, 0);
    if(!cv)
    {
        fprintf(stderr, "%s: unable to initialise libcucul\n", argv[0]);
        return 1;
    }

    i = load_image(argv[1]);
    if(!i)
    {
        fprintf(stderr, "%s: unable to load %s\n", argv[0], argv[1]);
        cucul_free_canvas(cv);
        return 1;
    }

    /* Assume a 6×10 font */
    lines = cols * i->h * 6 / i->w / 10;

    cucul_set_canvas_size(cv, cols, lines);
    cucul_set_color_ansi(cv, CUCUL_DEFAULT, CUCUL_TRANSPARENT);
    cucul_clear_canvas(cv);
    cucul_dither_bitmap(cv, 0, 0, cols, lines, i->dither, i->pixels);

    unload_image(i);

    export = cucul_export_canvas(cv, "irc");
    fwrite(cucul_get_buffer_data(export),
           cucul_get_buffer_size(export), 1, stdout);
    cucul_free_buffer(export);

    cucul_free_canvas(cv);

    return 0;
}

