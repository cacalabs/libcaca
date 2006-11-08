/*
 *  font2tga      libcucul font test program
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
#endif

#include "cucul.h"

#define WIDTH 64

int main(int argc, char *argv[])
{
    unsigned long int const *blocks;
    cucul_font_t *f;
    char const * const * fonts;
    cucul_canvas_t *cv;
    void *buffer;
    unsigned long int len;
    unsigned int i, j, x, y, glyphs;

    fonts = cucul_get_font_list();
    f = cucul_load_font(fonts[0], 0);
    blocks = cucul_get_font_blocks(f);

    for(i = 0, glyphs = 0; blocks[i + 1]; i += 2)
        glyphs += blocks[i + 1] - blocks[i];

    /* Create a canvas */
    cv = cucul_create_canvas(WIDTH, (glyphs + WIDTH - 1) / WIDTH);
    cucul_set_color_ansi(cv, CUCUL_BLACK, CUCUL_WHITE);

    /* Put all glyphs on the canvas */
    x = y = 0;

    for(i = 0; blocks[i + 1]; i += 2)
    {
        for(j = blocks[i]; j < blocks[i + 1]; j++)
        {
            cucul_putchar(cv, x, y, j);

            if(++x == WIDTH)
            {
                x = 0;
                y++;
            }
        }
    }

    cucul_free_font(f);

    buffer = cucul_export_memory(cv, "tga", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    /* Free everything */
    cucul_free_canvas(cv);

    return 0;
}

