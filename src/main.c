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

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <time.h>

#include "common.h"

static void start_game (game *);

int main (int argc, char **argv)
{
    game *g = malloc(sizeof(game));

    srand(time(NULL));

    if(ee_init())
    {
        return 1;
    }

    /* Initialize our program */
    g->w = ee_get_width();
    g->h = ee_get_height();

    /* Go ! */
    start_game(g);

    /* Clean up */
    ee_end();

    return 0;
}

static void start_game (game *g)
{
    int quit = 0;
    int poz = 0;
    int skip = 0;
    int purcompteur = 0;

    box *pausebox = NULL;

    g->sf = create_starfield(g);
    g->wp = malloc(sizeof(weapons));
    g->ex = malloc(sizeof(explosions));
    g->bo = malloc(sizeof(bonus));
    g->t = create_tunnel(g, g->w, g->h);
    g->p = create_player(g);
    g->al = malloc(sizeof(aliens));

    init_bonus(g, g->bo);
    init_weapons(g, g->wp);
    init_explosions(g, g->ex);
    init_aliens(g, g->al);

    /* Temporary stuff */
    g->t->w = 25;

    while(!quit)
    {
        char key;

        while((key = ee_get_key()))
        {
            switch(key)
            {
            case 'q':
                quit = 1;
                break;
            case 'p':
                poz = !poz;
                if(poz)
                {
                    pausebox = create_box(g, g->w / 2, g->h / 2,
                                              g->w - 16, 8);
                }
                else
                {
                    free_box(pausebox);
                }
                break;
            case '\t':
                ceo_alert(g);
                poz = 1;
                break;
            case 's':
                skip = 1;
                break;
            default:
                if(g->p->dead)
                {
                    break;
                }

                switch(key)
                {
                case 'h':
                    g->p->vx = -2;
                    break;
                case 'j':
                    if(g->p->y < g->h - 3) g->p->y += 1;
                    break;
                case 'k':
                    if(g->p->y > 2) g->p->y -= 1;
                    break;
                case 'l':
                    g->p->vx = 2;
                    break;
                case 'n':
                    if(g->p->special >= COST_NUKE)
                    {
                        g->p->special -= COST_NUKE;
                        add_weapon(g, g->wp, g->p->x << 4, g->p->y << 4, 0, 0, WEAPON_NUKE);
                    }
                    break;
                case 'f':
                    if(g->p->special >= COST_FRAGBOMB)
                    {
                        g->p->special -= COST_FRAGBOMB;
                        add_weapon(g, g->wp, g->p->x << 4, g->p->y << 4, 0, -16, WEAPON_FRAGBOMB);
                    }
                    break;
                case 'b':
                    if(g->p->special >= COST_BEAM)
                    {
                        g->p->special -= COST_BEAM;
                        add_weapon(g, g->wp, g->p->x << 4, g->p->y << 4, 0, 0, WEAPON_BEAM);
                    }
                    break;
                case ' ':
                    if(g->p->weapon == 0)
                    {
                        g->p->weapon = 4;
                        add_weapon(g, g->wp, (g->p->x - 2) << 4, g->p->y << 4, 0, -32, WEAPON_LASER);
                        add_weapon(g, g->wp, (g->p->x + 3) << 4, g->p->y << 4, 0, -32, WEAPON_LASER);
                        /* Extra schtuph */
                        add_weapon(g, g->wp, (g->p->x - 2) << 4, g->p->y << 4, -24, -16, WEAPON_SEEKER);
                        add_weapon(g, g->wp, (g->p->x + 3) << 4, g->p->y << 4, 24, -16, WEAPON_SEEKER);
                        /* More schtuph */
                        add_weapon(g, g->wp, (g->p->x - 1) << 4, (g->p->y - 1) << 4, 0, -32, WEAPON_LASER);
                        add_weapon(g, g->wp, (g->p->x + 2) << 4, (g->p->y - 1) << 4, 0, -32, WEAPON_LASER);
                        /* Even more schtuph */
                        add_weapon(g, g->wp, g->p->x << 4, (g->p->y - 1) << 4, 0, -32, WEAPON_LASER);
                        add_weapon(g, g->wp, (g->p->x + 1) << 4, (g->p->y - 1) << 4, 0, -32, WEAPON_LASER);
                        /* Extra schtuph */
                        add_weapon(g, g->wp, (g->p->x - 2) << 4, g->p->y << 4, -32, 0, WEAPON_SEEKER);
                        add_weapon(g, g->wp, (g->p->x + 3) << 4, g->p->y << 4, 32, 0, WEAPON_SEEKER);
                        /* MORE SCHTUPH! */
                        add_weapon(g, g->wp, g->p->x << 4, g->p->y << 4, 0, -16, WEAPON_BOMB);
                    }
                    break;
                }
            }
        }

        if(!poz || skip)
        {
            skip = 0;

            /* XXX: to be removed */
            if(ee_rand(0, 9) == 0)
            {
                int list[3] = { ALIEN_FOO, ALIEN_BAR, ALIEN_BAZ };

                add_alien(g, g->al, 0, rand() % g->h / 2, list[ee_rand(0,2)]);
            }

            /* Update game rules */
            if(g->t->right[1] - g->t->left[1] == g->t->w)
            {
                g->t->w = 85 - g->t->w;
            }

            /* Scroll and update positions */
            collide_player_tunnel(g, g->p, g->t, g->ex);
            update_player(g, g->p);
            collide_player_tunnel(g, g->p, g->t, g->ex);

            update_starfield(g, g->sf);
            update_bonus(g, g->bo);
            update_aliens(g, g->al);

            collide_weapons_tunnel(g, g->wp, g->t, g->ex);
            collide_weapons_aliens(g, g->wp, g->al, g->ex);
            update_weapons(g, g->wp);
            collide_weapons_tunnel(g, g->wp, g->t, g->ex);
            collide_weapons_aliens(g, g->wp, g->al, g->ex);

            update_explosions(g, g->ex);
            update_tunnel(g, g->t);
        }

        /* Clear screen */
        ee_clear();

        /* Print starfield, tunnel, aliens, player and explosions */
        draw_starfield(g, g->sf);
        draw_aliens(g, g->al);
        draw_tunnel(g, g->t);
        draw_bonus(g, g->bo);
        draw_explosions(g, g->ex);
        draw_weapons(g, g->wp);
        draw_player(g, g->p);
        draw_status(g);

        /* Print pause box if needed */
        if(poz)
        {
            pausebox->frame++;
            draw_box(g, pausebox);
        }

        /* Refresh */
        ee_refresh();

        purcompteur++;
    }

    if(pausebox)
    {
        free_box(pausebox);
    }

    free_starfield(g, g->sf);
    free_tunnel(g->t);
    free_player(g->p);
}

