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

void collide_weapons_tunnel(game *g, weapons *wp, tunnel *t, explosions *ex)
{
    int i, j, x, y;

    for(i = 0; i < WEAPONS; i++)
    {
        x = wp->x[i] >> 4;
        y = wp->y[i] >> 4;

        switch(wp->type[i])
        {
        case WEAPON_LIGHTNING:
        case WEAPON_NONE:
            break;
        case WEAPON_SEEKER:
        case WEAPON_BOMB:
        case WEAPON_FRAGBOMB:
            if(y < 0 || y >= g->h)
            {
                break;
            }

            if(x <= t->left[y]
                 || x >= t->right[y])
            {
                int damage = wp->type[i] == WEAPON_SEEKER ? 1 : 2;

                add_explosion(g, ex, x, y, 0, 1, wp->type[i] == WEAPON_SEEKER ? EXPLOSION_SMALL : EXPLOSION_MEDIUM);

                if(x <= t->left[y])
                {
                    if(y-2 >= 0) t->left[y-2] -= damage - 1;
                    if(y-1 >= 0) t->left[y-1] -= damage;
                    t->left[y] -= damage + 1;
                    if(y+1 < g->h) t->left[y+1] -= damage;
                    if(y+2 < g->h) t->left[y+2] -= damage - 1;
                }
                else
                {
                    if(y-2 >= 0) t->right[y-2] += damage - 1;
                    if(y-1 >= 0) t->right[y-1] += damage;
                    t->right[y] += damage + 1;
                    if(y+1 < g->h) t->right[y+1] += damage;
                    if(y+2 < g->h) t->right[y+2] += damage - 1;
                }

                if(wp->type[i] == WEAPON_FRAGBOMB)
                {
                    wp->n[i] = -1;
                }
                else
                {
                    wp->type[i] = WEAPON_NONE;
                }
            }
            break;
        case WEAPON_LASER:
            for(j = GET_MIN(0, wp->vy[i] >> 4);
                 j < GET_MAX(0, wp->vy[i] >> 4);
                 j++)
            {
                if(y+j >= g->h || y+j < 0)
                {
                    continue;
                }

                if(x <= t->left[y+j] || x >= t->right[y+j])
                {
                    add_explosion(g, ex, x, y+j, 0, 1, EXPLOSION_SMALL);
                    wp->type[i] = WEAPON_NONE;

                    if(x <= t->left[y+j])
                    {
                        if(y+j-1 >= 0) t->left[y+j-1]--;
                        t->left[y+j] -= 2;
                        if(y+j+1 < g->h) t->left[y+j+1]--;
                    }
                    else
                    {
                        if(y+j-1 >= 0) t->right[y+j-1]++;
                        t->right[y+j] += 2;
                        if(y+j+1 < g->h) t->right[y+j+1]++;
                    }
                    break;
                }
            }
            break;
        case WEAPON_BEAM:
            if(wp->n[i] > 19)
            {
                break;
            }

            j = (29 - wp->n[i]) * (29 - wp->n[i]) / 8;
            j = GET_MIN(y, j);

            for(; j > 0; j--)
            {
                if(x - 2 <= t->left[y-j])
                {
                    add_explosion(g, ex, GET_MIN(t->left[y-j], x+3), y-j, 0, 1, EXPLOSION_SMALL);
                    t->left[y-j] -= ee_rand(0,2);
                }
                else if(x + 3 >= t->right[y-j])
                {
                    add_explosion(g, ex, GET_MAX(t->right[y-j], x-2), y-j, 0, 1, EXPLOSION_SMALL);
                    t->right[y-j] += ee_rand(0,2);
                }
            }
            break;

        case WEAPON_NUKE:
            /* The nuke does not break the tunnel */
            break;
        }
    }
}

void collide_weapons_aliens(game *g, weapons *wp, aliens *al, explosions *ex)
{
    int i, j, k, x, y;

    for(i = 0; i < WEAPONS; i++)
    {
        int ok = 0;
        int r;

        x = wp->x[i] >> 4;
        y = wp->y[i] >> 4;

        switch(wp->type[i])
        {
        case WEAPON_LIGHTNING:
        case WEAPON_NONE:
            break;
        case WEAPON_NUKE:
            /* Big nuke */
            r = (29 - wp->n[i]) * (29 - wp->n[i]) / 8;

            for(j = 0; j < ALIENS; j++)
            {
                if(al->type[j] == ALIEN_NONE || al->life[j] < 0)
                {
                    continue;
                }

                if(wp->n[i] == 0 /* Nuke destroys _everything_ */ ||
                    (al->x[j] - x) * (al->x[j] - x)
                      + 4 * (al->y[j] - y) * (al->y[j] - y)
                    <= r * r)
                {
                    /* Kill alien, not nuke */
                    al->life[j] -= 10;
                }
            }
            break;

        case WEAPON_BEAM:
            if(wp->n[i] > 19)
            {
                break;
            }

            r = (29 - wp->n[i]) * (29 - wp->n[i]) / 8;

            for(j = 0; j < ALIENS; j++)
            {
                if(al->type[j] == ALIEN_NONE || al->life[j] < 0)
                {
                    continue;
                }

                if(x >= al->x[j] && x <= al->x[j] + 4
                     && y >= al->y[j] + 2 && y-5-r <= al->y[j])
                {
                    al->life[j] -= 4;
                }
            }
            break;

        case WEAPON_LASER:
            for(j = 0; j < ALIENS; j++)
            {
                if(al->type[j] == ALIEN_NONE || al->life[j] < 0)
                {
                    continue;
                }

                for(k = GET_MIN(0, wp->vy[i] >> 4);
                     k < GET_MAX(0, wp->vy[i] >> 4);
                     k++)
                {
                    if(x >= al->x[j] && x <= al->x[j] + 4
                         && y+k >= al->y[j] && y+k <= al->y[j] + 2)
                    {
                        al->life[j]--;
                        ok = 1;
                        break;
                    }
                }
            }

            if(ok)
            {
                add_explosion(g, ex, x, y+1, 0, 0, EXPLOSION_SMALL);
                wp->type[i] = WEAPON_NONE;
            }
            break;

        case WEAPON_SEEKER:
        case WEAPON_BOMB:
        case WEAPON_FRAGBOMB:
            for(j = 0; j < ALIENS; j++)
            {
                if(al->type[j] == ALIEN_NONE || al->life[j] < 0)
                {
                    continue;
                }

                if(x >= al->x[j] && x <= al->x[j] + 4
                     && y >= al->y[j] && y <= al->y[j] + 2)
                {
                    al->life[j] -= wp->type[i] == WEAPON_SEEKER ? 1 : 5;
                    ok = 1;
                }
            }

            if(ok)
            {
                add_explosion(g, ex, x, y+1, 0, 0, wp->type[i] == WEAPON_SEEKER ? EXPLOSION_SMALL : EXPLOSION_MEDIUM);

                if(wp->type[i] == WEAPON_FRAGBOMB)
                {
                    wp->n[i] = -1;
                }
                else
                {
                    wp->type[i] = WEAPON_NONE;
                }
            }
            break;
        }
    }
}

void collide_player_tunnel(game *g, player *p, tunnel *t, explosions *ex)
{
    if(p->dead)
    {
        return;
    }

    if(p->x <= t->left[p->y])
    {
        p->x += 3;
        p->vx = 2;
        add_explosion(g, ex, p->x+1, p->y-1, 0, 0, EXPLOSION_SMALL);
        p->life -= 180;
    }
    else if(p->x + 5 >= t->right[p->y])
    {
        p->x -= 3;
        p->vx = -2;
        add_explosion(g, ex, p->x+4, p->y-1, 0, 0, EXPLOSION_SMALL);
        p->life -= 180;
    }
}

