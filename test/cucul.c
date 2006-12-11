/*
 *  all           full libcaca API test
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It commes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "caca.h"

#define ITER 128

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    unsigned int i, j, w, h;

    fprintf(stderr, "testing cucul_create_canvas()\n");
    for(i = 0; i < ITER; i++)
    {
        w = cucul_rand(1, 1000);
        h = cucul_rand(1, 1000);
        cv = cucul_create_canvas(w, h);
        cucul_put_char(cv, w - 1, h - 1, 'x');
        if(cucul_get_char(cv, w - 1, h - 1) != 'x')
            fprintf(stderr, "  failed (%ux%u)\n", w, h);
        cucul_free_canvas(cv);
    }

    fprintf(stderr, "testing cucul_set_frame_name()\n");
    cv = cucul_create_canvas(1, 1);
    for(i = 0; i < ITER; i++)
    {
        cucul_create_frame(cv, 0);
        for(j = 0; j < ITER; j++)
        {
            char buf[BUFSIZ];
            w = cucul_rand(1, 1000);
            memset(buf, 'x', w);
            buf[w] = '\0';
            cucul_set_frame_name(cv, buf);
        }
    }
    cucul_free_canvas(cv);

    fprintf(stderr, "all tests passed\n");
    return 0;
}

