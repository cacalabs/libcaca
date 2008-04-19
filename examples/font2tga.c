/*
 *  font2tga      libcucul font test program
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#endif

#include "cucul.h"

int main(int argc, char *argv[])
{
    uint32_t const *blocks;
    cucul_font_t *f;
    char const * const * fonts;
    cucul_canvas_t *cv;
    void *buffer;
    size_t len;
    unsigned int i, j, x, y, cells, width;

    fonts = cucul_get_font_list();
    f = cucul_load_font(fonts[0], 0);
    blocks = cucul_get_font_blocks(f);

    for(i = 0, cells = 0; blocks[i + 1]; i += 2)
    {
        cells += blocks[i + 1] - blocks[i];
        for(j = blocks[i]; j < blocks[i + 1]; j++)
            if(cucul_utf32_is_fullwidth(j))
                cells++;
    }

    for(width = 64; width * width < cells; width *= 2)
        ;

    /* Create a canvas */
    cv = cucul_create_canvas(width, (cells + width - 1) / (width - 1));
    cucul_set_color_ansi(cv, CUCUL_RED, CUCUL_RED);
    cucul_clear_canvas(cv);
    cucul_set_color_ansi(cv, CUCUL_BLACK, CUCUL_WHITE);

    /* Put all glyphs on the canvas */
    x = y = 0;

    for(i = 0; blocks[i + 1]; i += 2)
    {
        for(j = blocks[i]; j < blocks[i + 1]; j++)
        {
            cucul_put_char(cv, x, y, j);

            if(cucul_utf32_is_fullwidth(j))
                ++x;

            if(++x >= width - 1)
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

