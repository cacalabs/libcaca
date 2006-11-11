/*
 *  frames        canvas frame switching features
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
#endif
#include "cucul.h"
#include "caca.h"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    caca_display_t *dp;

    int n, frame;

    /* Create a canvas with 200 frames */
    cv = cucul_create_canvas(0, 0);
    for(frame = 1; frame < 200; frame++)
        cucul_create_canvas_frame(cv, frame);

    fprintf(stderr, "canvas created, size is %ix%i\n",
            cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));

    /* Resize it to 150 x 80 (around 19MB) */
    cucul_set_canvas_size(cv, 150, 80);

    fprintf(stderr, "canvas expanded, size is %ix%i\n",
            cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));

    /* Fill the first 16 frames with a different colour */
    for(frame = 0; frame < 16; frame++)
    {
        cucul_set_canvas_frame(cv, frame);
        cucul_set_color_ansi(cv, CUCUL_WHITE, frame);
        cucul_fill_box(cv, 0, 0, 40, 15, ':');
        cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLUE);
        cucul_putstr(cv, frame * 5 / 2, frame, "カカ");
        cucul_set_color_ansi(cv, CUCUL_DEFAULT, CUCUL_TRANSPARENT);
    }

    /* Resize it to a more decent size */
    cucul_set_canvas_size(cv, 41, 16);

    fprintf(stderr, "canvas shrinked, size is %ix%i\n",
            cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));

    dp = caca_create_display(cv);
    caca_set_display_time(dp, 50000);

    fprintf(stderr, "display attached, size is %ix%i\n",
            cucul_get_canvas_width(cv), cucul_get_canvas_height(cv));

    n = 0;
    while(!caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, 0))
    {
        cucul_set_canvas_frame(cv, n % 16);
        caca_refresh_display(dp);
        n++;
    }

    caca_free_display(dp);

    /* It is possible, though not necessary, to free all additional frames
     * separately. */
    cucul_free_canvas(cv);

    return 0;
}

