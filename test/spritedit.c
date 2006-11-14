/*
 *  spritedit     sprite editor for libcaca
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
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
#   include <stdio.h>
#endif
#include "cucul.h"
#include "caca.h"

#define MYDELAY 100

int main(int argc, char **argv)
{
#if 0
    cucul_canvas_t *cv;
    caca_display_t *dp;

    int quit = 0;
    cucul_sprite_t *sprite;
    int frame = 0;
    unsigned char play = 0;
    unsigned int delay = 0;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        return 1;
    }

    cv = cucul_create_canvas(0, 0);
    if(!cv)
        return 1;
    dp = caca_create_display(cv);
    if(!dp)
        return 1;

    sprite = cucul_load_sprite(argv[1]);

    if(!sprite)
    {
        caca_free_display(dp);
        cucul_free_canvas(cv);
        fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        return 1;
    }

    /* Go ! */
    while(!quit)
    {
        caca_event_t ev;
        int xa, ya, xb, yb;
        char buf[BUFSIZ];

        while(caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, 0))
        {
            switch(ev.data.key.ch)
            {
            case 0:
                break;
            case 'q':
                quit = 1;
                break;
            case '-':
                if(frame > 0)
                    frame--;
                break;
            case '+':
                if(frame < cucul_get_sprite_frames(sprite) - 1)
                    frame++;
                break;
	    case 'p':
		play=!play;

            }
        }

	if(play) {
	    if(!delay) {
		if(frame < cucul_get_sprite_frames(sprite) - 1) {
		    frame++;
		}
		else {
		    frame = 0;
		}
	    }
	    delay++;
	    if(delay>=MYDELAY) {
		delay = 0;
	    }
	}


        cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
        cucul_clear_canvas(cv);

        cucul_draw_thin_box(cv, 0, 0, cucul_get_canvas_width(cv) - 1,
                            cucul_get_canvas_height(cv) - 1);

        cucul_put_str(cv, 3, 0, "[ Sprite editor for libcaca ]");

        sprintf(buf, "sprite `%s'", argv[1]);
        cucul_put_str(cv, 3, 2, buf);
        sprintf(buf, "frame %i/%i", frame, cucul_get_sprite_frames(sprite) - 1);
        cucul_put_str(cv, 3, 3, buf);

        /* Crosshair */
        cucul_draw_thin_line(cv, 57, 2, 57, 18);
        cucul_draw_thin_line(cv, 37, 10, 77, 10);
        cucul_put_char(cv, 57, 10, '+');

        /* Boxed sprite */
        xa = -1 - cucul_get_sprite_dx(sprite, frame);
        ya = -1 - cucul_get_sprite_dy(sprite, frame);
        xb = xa + 1 + cucul_get_sprite_width(sprite, frame);
        yb = ya + 1 + cucul_get_sprite_height(sprite, frame);
        cucul_set_color_ansi(cv, CUCUL_BLACK, CUCUL_BLACK);
        cucul_fill_box(cv, 57 + xa, 10 + ya, 57 + xb, 10 + yb, " ");
        cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
        cucul_draw_thin_box(cv, 57 + xa, 10 + ya, 57 + xb, 10 + yb);
        cucul_draw_sprite(cv, 57, 10, sprite, frame);

        /* Free sprite */
        cucul_draw_sprite(cv, 20, 10, sprite, frame);

        caca_refresh_display(dp);
    }

    /* Clean up */
    caca_free_display(dp);
    cucul_free_canvas(cv);
#endif

    return 0;
}

