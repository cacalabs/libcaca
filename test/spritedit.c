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

#include <stdio.h>

#include "caca.h"

int main(int argc, char **argv)
{
    int quit = 0;
    struct caca_sprite *sprite;
    int frame = 0;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        return 1;
    }

    if(caca_init())
        return 1;

    sprite = caca_load_sprite(argv[1]);

    if(!sprite)
    {
        caca_end();
        fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        return 1;
    }

    /* Go ! */
    while(!quit)
    {
        int xa, ya, xb, yb;
        char buf[BUFSIZ];
        int event;

        while((event = caca_get_event(CACA_EVENT_KEY_PRESS)))
        {
            switch(event & 0x00ffffff)
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
                if(frame < caca_get_sprite_frames(sprite) - 1)
                    frame++;
                break;
            }
        }

        caca_clear();

        caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);
        caca_draw_thin_box(0, 0, caca_get_width() - 1, caca_get_height() - 1);

        caca_putstr(3, 0, "[ Sprite editor for libcaca ]");

        sprintf(buf, "sprite `%s'", argv[1]);
        caca_putstr(3, 2, buf);
        sprintf(buf, "frame %i/%i", frame, caca_get_sprite_frames(sprite) - 1);
        caca_putstr(3, 3, buf);

        /* Crosshair */
        caca_draw_thin_line(57, 2, 57, 18);
        caca_draw_thin_line(37, 10, 77, 10);
        caca_putchar(57, 10, '+');

        /* Boxed sprite */
        xa = -1 - caca_get_sprite_dx(sprite, frame);
        ya = -1 - caca_get_sprite_dy(sprite, frame);
        xb = xa + 1 + caca_get_sprite_width(sprite, frame);
        yb = ya + 1 + caca_get_sprite_height(sprite, frame);
        caca_set_color(CACA_COLOR_BLACK, CACA_COLOR_BLACK);
        caca_fill_box(57 + xa, 10 + ya, 57 + xb, 10 + yb, ' ');
        caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);
        caca_draw_thin_box(57 + xa, 10 + ya, 57 + xb, 10 + yb);
        caca_draw_sprite(57, 10, sprite, frame);

        /* Free sprite */
        caca_draw_sprite(20, 10, sprite, frame);

        caca_refresh();
    }

    /* Clean up */
    caca_end();

    return 0;
}

