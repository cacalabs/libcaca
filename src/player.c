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

struct ee_sprite *ship_sprite;

/* Init tunnel */
player * create_player(game *g)
{
    player *p = malloc(sizeof(player));

    p->x = g->w / 2;
    p->y = g->h - 3;
    p->vx = 0;
    p->vy = 0;
    p->weapon = 0;
    p->special = MAX_SPECIAL;
    p->life = MAX_LIFE;
    p->dead = 0;

    ship_sprite = ee_load_sprite("data/ship.txt");

    return p;
}

void free_player(player *p)
{
    free(p);
}

void draw_player(game *g, player *p)
{
    if(p->dead)
        return;

    ee_draw_sprite(p->x, p->y, ship_sprite, 0);
}

void update_player(game *g, player *p)
{
    if(p->dead)
        return;

    if(p->life <= 0)
    {
        add_explosion(g, g->ex, p->x, p->y, 0, 0, EXPLOSION_SMALL);
        p->dead = 1;
        return;
    }

    /* Update weapon stats */
    if(p->weapon)
        p->weapon--;

    if(p->special < MAX_SPECIAL)
        p->special++;

    /* Update life */
    if(p->life < MAX_LIFE)
        p->life++;

    /* Update coords */
    p->x += p->vx;

    if(p->vx < 0)
        p->vx++;
    else if(p->vx > 0)
        p->vx--;

    if(p->x < 1)
        p->x = 1;
    else if(p->x > g->w - 7)
        p->x = g->w - 7;
}

