/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
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

#include <stdlib.h>

#include "common.h"

static void draw_small_explosion(int x, int y, int frame);
static void draw_medium_explosion(int x, int y, int frame);

struct ee_sprite *medium_sprite;
struct ee_sprite *small_sprite;

void init_explosions(game *g, explosions *ex)
{
    int i;

    for(i = 0; i < EXPLOSIONS; i++)
    {
        ex->type[i] = EXPLOSION_NONE;
    }

    medium_sprite = ee_load_sprite("data/explosion_medium");
    small_sprite = ee_load_sprite("data/explosion_small");
}

void add_explosion(game *g, explosions *ex, int x, int y, int vx, int vy, int type)
{
    int i;

    for(i = 0; i < EXPLOSIONS; i++)
    {
        if(ex->type[i] == EXPLOSION_NONE)
        {
            ex->type[i] = type;
            ex->x[i] = x;
            ex->y[i] = y;
            ex->vx[i] = vx;
            ex->vy[i] = vy;
            switch(type)
            {
                case EXPLOSION_MEDIUM:
                    ex->n[i] = 11;
                    break;
                case EXPLOSION_SMALL:
                    ex->n[i] = 7;
                    break;
            }
            break;
        }
    }
}

void draw_explosions(game *g, explosions *ex)
{
    int i;

    for(i = 0; i < EXPLOSIONS; i++)
    {
#if 0
        ee_color(GREEN);
        ee_goto(ex->x[i] + 3, ex->y[i]);
        switch(ee_rand(0,2))
        {
        case 0:
            ee_putchar('p');
            ee_putchar('i');
            ee_putchar('f');
            break;
        case 1:
            ee_putchar('p');
            ee_putchar('a');
            ee_putchar('f');
            break;
        case 2:
            ee_putchar('p');
            ee_putchar('o');
            ee_putchar('u');
            ee_putchar('f');
            break;
        }
        ee_putchar('!');
#endif

        switch(ex->type[i])
        {
            case EXPLOSION_MEDIUM:
                draw_medium_explosion(ex->x[i], ex->y[i], ex->n[i]);
                break;
            case EXPLOSION_SMALL:
                draw_small_explosion(ex->x[i], ex->y[i], ex->n[i]);
                break;
            case EXPLOSION_NONE:
                break;
        }
    }
}

void update_explosions(game *g, explosions *ex)
{
    int i;

    for(i = 0; i < EXPLOSIONS; i++)
    {
        switch(ex->type[i])
        {
            case EXPLOSION_MEDIUM:
            case EXPLOSION_SMALL:
                ex->x[i] += ex->vx[i];
                ex->y[i] += ex->vy[i];
                ex->n[i]--;
                if(ex->n[i] < 0)
                {
                    ex->type[i] = EXPLOSION_NONE;
                }
                break;
            case EXPLOSION_NONE:
                break;
        }
    }
}

static void draw_small_explosion(int x, int y, int frame)
{
    ee_set_sprite_frame(small_sprite, 6 - frame);
    ee_draw_sprite(x, y, small_sprite);
}

static void draw_medium_explosion(int x, int y, int frame)
{
    ee_set_sprite_frame(medium_sprite, 10 - frame);
    ee_draw_sprite(x, y, medium_sprite);
}

