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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include "cucul.h"
#include "caca.h"

int main(void)
{
    caca_event_t ev;
    cucul_canvas_t *cv;
    caca_display_t *dp;

    int n, frame;

    /* Create a canvas with 200 frames */
    cv = cucul_create_canvas(0, 0);
    for(frame = 1; frame < 200; frame++)
        cucul_create_canvas_frame(cv, frame);

    /* Resize it to 150 x 80 (around 19MB) */
    cucul_set_canvas_size(cv, 150, 80);

    /* Resize it to a more decent size */
    cucul_set_canvas_size(cv, 41, 16);

    dp = caca_create_display(cv);
    caca_set_delay(dp, 50000);

    /* Fill the first 16 frames with a different colour */
    for(frame = 0; frame < 16; frame++)
    {
        cucul_set_canvas_frame(cv, frame);
        cucul_set_color(cv, CUCUL_COLOR_WHITE, frame);
        cucul_clear_canvas(cv);
        cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_putstr(cv, frame * 5 / 2, frame, "CACA");
    }

    n = 0;
    while(!caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0))
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

