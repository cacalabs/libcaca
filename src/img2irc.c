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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cucul.h"
#include "common-image.h"

int main(int argc, char **argv)
{
    /* libcucul context */
    cucul_t *qq;
    struct cucul_buffer *export;
    struct image *i;
    int cols = 56, lines;

    if(argc != 2)
    {
        fprintf(stderr, "%s: wrong argument count\n", argv[0]);
        return 1;
    }

    qq = cucul_create(0, 0);
    if(!qq)
    {
        fprintf(stderr, "%s: unable to initialise libcucul\n", argv[0]);
        return 1;
    }

    i = load_image(argv[1]);
    if(!i)
    {
        fprintf(stderr, "%s: unable to load %s\n", argv[0], argv[1]);
        cucul_free(qq);
        return 1;
    }

    /* Assume a 6×10 font */
    lines = cols * i->h * 6 / i->w / 10;

    cucul_set_size(qq, cols, lines);
    cucul_clear(qq);
    cucul_dither_bitmap(qq, 0, 0, cols - 1, lines - 1, i->dither, i->pixels);

    unload_image(i);

    export = cucul_create_export(qq, "irc");
    fwrite(export->data, export->size, 1, stdout);
    cucul_free_export(export);

    cucul_free(qq);

    return 0;
}

