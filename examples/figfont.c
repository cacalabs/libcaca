/*
 *  figfont       libcucul FIGfont test program
 *  Copyright (c) 2007 Sam Hocevar <sam@zoy.org>
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
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#endif

#include "cucul.h"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    void *buffer;
    unsigned long int len;
    uint8_t color = 0;

    if(argc < 3)
    {
        fprintf(stderr, "Too few arguments\n");
        return -1;
    }

    cv = cucul_create_canvas(0, 0);
    if(cucul_canvas_set_figfont(cv, argv[1]))
    {
        fprintf(stderr, "Could not open font\n");
        return -1;
    }

    while(argv[2][0])
    {
        cucul_set_color_ansi(cv, 1 + ((color += 4) % 15), CUCUL_TRANSPARENT);
        cucul_put_figchar(cv, argv[2]++[0]);
    }

    buffer = cucul_export_memory(cv, "utf8", &len);
    fwrite(buffer, len, 1, stdout);
    free(buffer);

    cucul_free_canvas(cv);

    return 0;
}

