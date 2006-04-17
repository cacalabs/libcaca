/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains export functions for bitmap formats
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

void _cucul_get_tga(cucul_t *qq, cucul_buffer_t *ex)
{
    char const * const * fonts;
    char * cur;
    cucul_font_t *f;
    unsigned int i, w, h;

    fonts = cucul_get_font_list();
    if(!fonts[0])
        return;

    f = cucul_load_font(fonts[0], 0);

    w = cucul_get_width(qq) * cucul_get_font_width(f);
    h = cucul_get_height(qq) * cucul_get_font_height(f);

    ex->size = w * h * 4 + 18;
    ex->data = malloc(ex->size);

    cur = ex->data;

    /* ID Length */
    cur += sprintf(cur, "%c", 0);
    /* Color Map Type: no colormap */
    cur += sprintf(cur, "%c", 0);
    /* Image Type: uncompressed truecolor */
    cur += sprintf(cur, "%c", 2);
    /* Color Map Specification: no color map */
    memset(cur, 0, 5); cur += 5;

    /* Image Specification */
    cur += sprintf(cur, "%c%c", 0, 0); /* X Origin */
    cur += sprintf(cur, "%c%c", 0, 0); /* Y Origin */
    cur += sprintf(cur, "%c%c", w & 0xff, w >> 8); /* Width */
    cur += sprintf(cur, "%c%c", h & 0xff, h >> 8); /* Height */
    cur += sprintf(cur, "%c", 32); /* Pixel Depth */
    cur += sprintf(cur, "%c", 40); /* Image Descriptor */

    /* Image ID: no ID */
    /* Color Map Data: no colormap */

    /* Image Data */
    cucul_render_canvas(qq, f, cur, w, h, 4 * w);

    /* Swap bytes. What a waste of time. */
    for(i = 0; i < w * h * 4; i += 4)
    {
        char w;
        w = cur[i]; cur[i] = cur[i + 3]; cur[i + 3] = w;
        w = cur[i + 1]; cur[i + 1] = cur[i + 2]; cur[i + 2] = w;
    }

    cucul_free_font(f);
}

