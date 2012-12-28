/*
 *  all           full libcaca API test
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
#   include <string.h>
#endif

#include "caca.h"

#define ITER 128

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
    unsigned int i, j, w, h;

    fprintf(stderr, "testing caca_create_canvas()\n");
    for(i = 0; i < ITER; i++)
    {
        w = caca_rand(1, 1000);
        h = caca_rand(1, 1000);
        cv = caca_create_canvas(w, h);
        caca_put_char(cv, w - 1, h - 1, 'x');
        if(caca_get_char(cv, w - 1, h - 1) != 'x')
            fprintf(stderr, "  failed (%ux%u)\n", w, h);
        caca_free_canvas(cv);
    }

    fprintf(stderr, "testing caca_set_frame_name()\n");
    cv = caca_create_canvas(1, 1);
    if(cv == NULL)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    for(i = 0; i < ITER; i++)
    {
        caca_create_frame(cv, 0);
        for(j = 0; j < ITER; j++)
        {
            char buf[BUFSIZ];
            w = caca_rand(1, BUFSIZ - 1);
            memset(buf, 'x', w);
            buf[w] = '\0';
            caca_set_frame_name(cv, buf);
        }
    }
    caca_free_canvas(cv);

    fprintf(stderr, "all tests passed\n");
    return 0;
}

