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

#include "cucul.h"
#include "caca.h"

int main(int argc, char **argv)
{
    cucul_t *qq;
    caca_t *kk;

    int quit = 0;
    struct cucul_sprite *sprite;
    int frame = 0;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        return 1;
    }

    qq = cucul_create(0, 0);
    if(!qq)
        return 1;
    kk = caca_attach(qq);
    if(!kk)
        return 1;

    sprite = cucul_load_sprite(qq, argv[1]);

    if(!sprite)
    {
        caca_detach(kk);
        cucul_free(qq);
        fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        return 1;
    }

    /* Go ! */
    while(!quit)
    {
        int xa, ya, xb, yb;
        char buf[BUFSIZ];
        int event;

        while((event = caca_get_event(kk, CACA_EVENT_KEY_PRESS)))
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
                if(frame < cucul_get_sprite_frames(qq, sprite) - 1)
                    frame++;
                break;
            }
        }

        cucul_clear(qq);

        cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
        cucul_draw_thin_box(qq, 0, 0, cucul_get_width(qq) - 1, cucul_get_height(qq) - 1);

        cucul_putstr(qq, 3, 0, "[ Sprite editor for libcaca ]");

        sprintf(buf, "sprite `%s'", argv[1]);
        cucul_putstr(qq, 3, 2, buf);
        sprintf(buf, "frame %i/%i", frame, cucul_get_sprite_frames(qq, sprite) - 1);
        cucul_putstr(qq, 3, 3, buf);

        /* Crosshair */
        cucul_draw_thin_line(qq, 57, 2, 57, 18);
        cucul_draw_thin_line(qq, 37, 10, 77, 10);
        cucul_putchar(qq, 57, 10, '+');

        /* Boxed sprite */
        xa = -1 - cucul_get_sprite_dx(qq, sprite, frame);
        ya = -1 - cucul_get_sprite_dy(qq, sprite, frame);
        xb = xa + 1 + cucul_get_sprite_width(qq, sprite, frame);
        yb = ya + 1 + cucul_get_sprite_height(qq, sprite, frame);
        cucul_set_color(qq, CUCUL_COLOR_BLACK, CUCUL_COLOR_BLACK);
        cucul_fill_box(qq, 57 + xa, 10 + ya, 57 + xb, 10 + yb, " ");
        cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
        cucul_draw_thin_box(qq, 57 + xa, 10 + ya, 57 + xb, 10 + yb);
        cucul_draw_sprite(qq, 57, 10, sprite, frame);

        /* Free sprite */
        cucul_draw_sprite(qq, 20, 10, sprite, frame);

        caca_display(kk);
    }

    /* Clean up */
    caca_detach(kk);
    cucul_free(qq);

    return 0;
}

