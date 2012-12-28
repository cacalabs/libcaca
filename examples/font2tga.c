/*
 *  font2tga      libcaca font test program
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
#endif

#include "caca.h"

int main(int argc, char *argv[])
{
    uint32_t const *blocks;
    caca_font_t *f;
    char const * const * fonts;
    caca_canvas_t *cv;
    void *buffer;
    size_t len;
    unsigned int i, j, x, y, cells, width;

    fonts = caca_get_font_list();
    f = caca_load_font(fonts[0], 0);
    blocks = caca_get_font_blocks(f);

    for(i = 0, cells = 0; blocks[i + 1]; i += 2)
    {
        cells += blocks[i + 1] - blocks[i];
        for(j = blocks[i]; j < blocks[i + 1]; j++)
            if(caca_utf32_is_fullwidth(j))
                cells++;
    }

    for(width = 64; width * width < cells; width *= 2)
        ;

    /* Create a canvas */
    cv = caca_create_canvas(width, (cells + width - 1) / (width - 1));
    caca_set_color_ansi(cv, CACA_RED, CACA_RED);
    caca_clear_canvas(cv);
    caca_set_color_ansi(cv, CACA_BLACK, CACA_WHITE);

    /* Put all glyphs on the canvas */
    x = y = 0;

    for(i = 0; blocks[i + 1]; i += 2)
    {
        for(j = blocks[i]; j < blocks[i + 1]; j++)
        {
            caca_put_char(cv, x, y, j);

            if(caca_utf32_is_fullwidth(j))
                ++x;

            if(++x >= width - 1)
            {
                x = 0;
                y++;
            }
        }
    }

    caca_free_font(f);

    buffer = caca_export_canvas_to_memory(cv, "tga", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    /* Free everything */
    caca_free_canvas(cv);

    return 0;
}

