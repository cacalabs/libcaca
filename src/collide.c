/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: tarass
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

#include <stdlib.h>

#include "common.h"

void collide_weapons_tunnel( game *g, weapons *wp, tunnel *t, explosions *ex )
{
    int i, x, y;

    for( i = 0; i < WEAPONS; i++ )
    {
        x = wp->x[i] >> 4;
        y = wp->y[i] >> 4;

        switch( wp->type[i] )
        {
            case WEAPON_NONE:
                break;
            case WEAPON_SEEKER:
            case WEAPON_BOMB:
                if( x <= t->left[y]
                     || x >= t->right[y] )
                {
                    int damage = wp->type[i] == WEAPON_SEEKER ? 1 : 2;

                    add_explosion( g, ex, x, y, 0, 1, wp->type[i] == WEAPON_SEEKER ? EXPLOSION_SMALL : EXPLOSION_MEDIUM );

                    if( x <= t->left[y] )
                    {
                        t->right[y-2] -= damage - 1;
                        t->left[y-1] -= damage;
                        t->left[y] -= damage + 1;
                        t->left[y+1] -= damage;
                        t->right[y+2] -= damage - 1;
                    }
                    else
                    {
                        t->right[y-2] += damage - 1;
                        t->right[y-1] += damage;
                        t->right[y] += damage + 1;
                        t->right[y+1] += damage;
                        t->right[y+2] += damage - 1;
                    }

                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_LASER:
                if( x <= t->left[y+1]
                     || x >= t->right[y+1] )
                {
                    add_explosion( g, ex, x, y+1, 0, 1, EXPLOSION_SMALL );

                    if( x <= t->left[y+1] )
                    {
                        t->left[y]--;
                        t->left[y+1]-=2;
                        t->left[y+2]--;
                    }
                    else
                    {
                        t->right[y]++;
                        t->right[y+1]+=2;
                        t->right[y+2]++;
                    }

                    wp->type[i] = WEAPON_NONE;
                }
                else if( x <= t->left[y]
                          || x >= t->right[y] )
                {
                    add_explosion( g, ex, x, y, 0, 1, EXPLOSION_SMALL );

                    if( x <= t->left[y] )
                    {
                        t->left[y-1]--;
                        t->left[y]-=2;
                        t->left[y+1]--;
                    }
                    else
                    {
                        t->right[y-1]++;
                        t->right[y]+=2;
                        t->right[y+1]++;
                    }

                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_NUKE:
            case WEAPON_BEAM:
                /* The nuke and the laser do not break the tunnel */
                break;
        }
    }
}

void collide_weapons_aliens( game *g, weapons *wp, aliens *al, explosions *ex )
{
    int i, j, x, y;

    for( i = 0; i < WEAPONS; i++ )
    {
        int ok = 0;
        int r;

        x = wp->x[i] >> 4;
        y = wp->y[i] >> 4;

        switch( wp->type[i] )
        {
            case WEAPON_NONE:
                break;
            case WEAPON_NUKE:
                /* Big nuke */
                r = (29 - wp->n[i]) * (29 - wp->n[i]) / 8;

                for( j = 0; j < ALIENS; j++ )
                {
                    if( al->type[j] == ALIEN_NONE )
                    {
                        continue;
                    }

                    if( wp->n[i] == 0 /* Nuke destroys _everything_ */ ||
                        (al->x[j] - x) * (al->x[j] - x)
                          + 4 * (al->y[j] - y) * (al->y[j] - y)
                        <= r * r )
                    {
                        /* Kill alien, not nuke */
                        al->type[j] = ALIEN_NONE;
                        add_explosion( g, ex, al->x[j], al->y[j], 0, 0, EXPLOSION_MEDIUM );
                        add_bonus( g, g->bo, al->x[j], al->y[j], GET_RAND(0,5) ? BONUS_GREEN : BONUS_LIFE );
                    }
                }
                break;
            case WEAPON_BEAM:
                r = (29 - wp->n[i]) * (29 - wp->n[i]) / 8;

                for( j = 0; j < ALIENS; j++ )
                {
                    if( al->type[j] == ALIEN_NONE )
                    {
                        continue;
                    }

                    if( x >= al->x[j] && x <= al->x[j] + 4
                         && y >= al->y[j] + 2 && y-5-r <= al->y[j] )
                    {
                        al->life[j] -= 4;
                        if( al->life[j] <= 0 )
                        {
                            al->type[j] = ALIEN_NONE;
                            add_explosion( g, ex, al->x[j], al->y[j], 0, 0, EXPLOSION_MEDIUM );
                            add_bonus( g, g->bo, al->x[j], al->y[j], GET_RAND(0,5) ? BONUS_GREEN : BONUS_LIFE );
                        }
                    }
                }
                break;

            case WEAPON_LASER:
                for( j = 0; j < ALIENS; j++ )
                {
                    if( al->type[j] == ALIEN_NONE )
                    {
                        continue;
                    }

                    if( x >= al->x[j] && x <= al->x[j] + 4
                         && y >= al->y[j] && y <= al->y[j] + 2 )
                    {
                        al->life[j]--;
                        if( al->life[j] <= 0 )
                        {
                            al->type[j] = ALIEN_NONE;
                            add_explosion( g, ex, al->x[j], al->y[j], 0, 0, EXPLOSION_MEDIUM );
                            add_bonus( g, g->bo, al->x[j], al->y[j], GET_RAND(0,5) ? BONUS_GREEN : BONUS_LIFE );
                        }
                        ok = 1;
                    }
                    else if( x >= al->x[j] && x <= al->x[j] + 4
                              && y+1 >= al->y[j] && y+1 <= al->y[j] + 2 )
                    {
                        al->life[j]--;
                        if( al->life[j] <= 0 )
                        {
                            al->type[j] = ALIEN_NONE;
                            add_explosion( g, ex, al->x[j], al->y[j]+1, 0, 0, EXPLOSION_MEDIUM );
                            add_bonus( g, g->bo, al->x[j], al->y[j]+1, GET_RAND(0,5) ? BONUS_GREEN : BONUS_LIFE );
                        }
                        ok = 1;
                    }
                }

                if( ok )
                {
                    add_explosion( g, ex, x, y+1, 0, 0, EXPLOSION_SMALL );
                    wp->type[i] = WEAPON_NONE;
                }
                break;

            case WEAPON_SEEKER:
            case WEAPON_BOMB:
                for( j = 0; j < ALIENS; j++ )
                {
                    if( al->type[j] == ALIEN_NONE )
                    {
                        continue;
                    }

                    if( x >= al->x[j] && x <= al->x[j] + 4
                         && y >= al->y[j] && y <= al->y[j] + 2 )
                    {
                        al->life[j] -= wp->type[i] == WEAPON_BOMB ? 5 : 1;
                        if( al->life[j] <= 0 )
                        {
                            al->type[j] = ALIEN_NONE;
                            add_explosion( g, ex, al->x[j], al->y[j], 0, 0, EXPLOSION_MEDIUM );
                            add_bonus( g, g->bo, al->x[j], al->y[j], GET_RAND(0,5) ? BONUS_GREEN : BONUS_LIFE );
                        }
                        ok = 1;
                    }
                }

                if( ok )
                {
                    add_explosion( g, ex, x, y+1, 0, 0, wp->type[i] == WEAPON_SEEKER ? EXPLOSION_SMALL : EXPLOSION_MEDIUM );
                    wp->type[i] = WEAPON_NONE;
                }
                break;
        }
    }
}

void collide_player_tunnel( game *g, player *p, tunnel *t, explosions *ex )
{
    if( p->x <= t->left[p->y] )
    {
        p->x += 2;
        add_explosion( g, ex, p->x+1, p->y-2, 0, 0, EXPLOSION_SMALL );
    }
    else if( p->x + 5 >= t->right[p->y] )
    {
        p->x -= 2;
        add_explosion( g, ex, p->x+4, p->y-2, 0, 0, EXPLOSION_SMALL );
    }
}

