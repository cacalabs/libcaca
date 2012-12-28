/*
 *  frames        canvas frame switching features
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
#endif

#include "caca.h"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
    caca_display_t *dp;

    int n, frame;

    /* Create a canvas with 200 frames */
    cv = caca_create_canvas(0, 0);
    if(cv == NULL)
    {
        printf("Can't create canvas\n");
        return -1;
    }

    for(frame = 1; frame < 200; frame++)
        caca_create_frame(cv, frame);

    fprintf(stderr, "canvas created, size is %ix%i\n",
            caca_get_canvas_width(cv), caca_get_canvas_height(cv));

    /* Resize it to 150 x 80 (around 19MB) */
    caca_set_canvas_size(cv, 150, 80);

    fprintf(stderr, "canvas expanded, size is %ix%i\n",
            caca_get_canvas_width(cv), caca_get_canvas_height(cv));

    /* Fill the first 16 frames with a different colour */
    for(frame = 0; frame < 16; frame++)
    {
        caca_set_frame(cv, frame);
        caca_set_color_ansi(cv, CACA_WHITE, frame);
        caca_fill_box(cv, 0, 0, 40, 15, ':');
        caca_set_color_ansi(cv, CACA_WHITE, CACA_BLUE);
        caca_put_str(cv, frame * 5 / 2, frame, "カカ");
        caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);
    }

    /* Resize it to a more decent size */
    caca_set_canvas_size(cv, 41, 16);

    fprintf(stderr, "canvas shrinked, size is %ix%i\n",
            caca_get_canvas_width(cv), caca_get_canvas_height(cv));

    dp = caca_create_display(cv);
    caca_set_display_time(dp, 50000);

    fprintf(stderr, "display attached, size is %ix%i\n",
            caca_get_canvas_width(cv), caca_get_canvas_height(cv));

    n = 0;
    while(!caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, 0))
    {
        caca_set_frame(cv, n % 16);
        caca_refresh_display(dp);
        n++;
    }

    caca_free_display(dp);

    /* It is possible, though not necessary, to free all additional frames
     * separately. */
    caca_free_canvas(cv);

    return 0;
}

