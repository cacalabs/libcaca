/*
 *   spritedit     sprite editor using libee
 *   Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"

#include <stdio.h>

#include "ee.h"

int main(int argc, char **argv)
{
    int quit = 0;
    struct ee_sprite *sprite;
    int frame = 0;

    if(argc < 2)
    {
        fprintf(stderr, "%s: missing argument (filename).\n", argv[0]);
        return 1;
    }

    if(ee_init())
        return 1;

    sprite = ee_load_sprite(argv[1]);

    if(!sprite)
    {
        ee_end();
        fprintf(stderr, "%s: could not open `%s'.\n", argv[0], argv[1]);
        return 1;
    }

    /* Go ! */
    while(!quit)
    {
        int xa, ya, xb, yb;
        char buf[BUFSIZ];

        switch(ee_get_key())
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
            if(frame < ee_get_sprite_frames(sprite) - 1)
                frame++;
            break;
        }

        ee_clear();

        ee_set_color(EE_WHITE);
        ee_draw_thin_box(0, 0, ee_get_width() - 1, ee_get_height() - 1);

        ee_putstr(3, 0, "[ Sprite editor for libee ]");

        sprintf(buf, "sprite `%s'", argv[1]);
        ee_putstr(3, 2, buf);
        sprintf(buf, "frame %i/%i", frame, ee_get_sprite_frames(sprite) - 1);
        ee_putstr(3, 3, buf);

        /* Crosshair */
        ee_draw_thin_line(57, 2, 57, 18);
        ee_draw_thin_line(37, 10, 77, 10);
        ee_putchar(57, 10, '+');

        /* Boxed sprite */
        xa = -1 - ee_get_sprite_dx(sprite, frame);
        ya = -1 - ee_get_sprite_dy(sprite, frame);
        xb = xa + 1 + ee_get_sprite_width(sprite, frame);
        yb = ya + 1 + ee_get_sprite_height(sprite, frame);
        ee_set_color(EE_BLACK);
        ee_fill_box(57 + xa, 10 + ya, 57 + xb, 10 + yb, ' ');
        ee_set_color(EE_WHITE);
        ee_draw_thin_box(57 + xa, 10 + ya, 57 + xb, 10 + yb);
        ee_draw_sprite(57, 10, sprite, frame);

        /* Free sprite */
        ee_draw_sprite(20, 10, sprite, frame);

        ee_refresh();
    }

    /* Clean up */
    ee_end();

    return 0;
}

