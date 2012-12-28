/*
 *  figfont       libcaca FIGfont test program
 *  Copyright (c) 2007-2012 Sam Hocevar <sam@hocevar.net>
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
    caca_canvas_t *cv;
    void *buffer;
    size_t len;
    uint8_t color = 0;

    if(argc < 3)
    {
        fprintf(stderr, "Too few arguments\n");
        return -1;
    }

    cv = caca_create_canvas(0, 0);
    if(caca_canvas_set_figfont(cv, argv[1]))
    {
        fprintf(stderr, "Could not open font\n");
        return -1;
    }

    while(argv[2][0])
    {
        caca_set_color_ansi(cv, 1 + ((color += 4) % 15), CACA_TRANSPARENT);
        caca_put_figchar(cv, argv[2]++[0]);
    }

    buffer = caca_export_canvas_to_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    caca_free_canvas(cv);

    return 0;
}

