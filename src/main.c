/*
 *   ttyvaders - a tty based shoot'em'up
 *   Copyright (C) 2002 Sam Hocevar <sam@zoy.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include "common.h"

static void start_game (game *);

int main (int argc, char **argv)
{
    game *g = malloc(sizeof(game));

//    srand(time(NULL));

    if( init_graphics() )
    {
        return 1;
    }

    /* Initialize our program */
    init_game(g);

    /* Go ! */
    start_game(g);

    /* Clean up */
    end_graphics();

    return 0;
}

static void start_game (game *g)
{
    int i;
    int quit = 0;
    int poz = 0;
    int skip = 0;

    starfield *sf = malloc(sizeof(starfield));
    weapons *wp = malloc(sizeof(weapons));
    explosions *ex = malloc(sizeof(explosions));
    tunnel *t = create_tunnel( g, g->w, g->h );
    player *p = create_player( g );
    aliens *al = malloc(sizeof(aliens));

    init_starfield( g, sf );
    init_weapons( g, wp );
    init_explosions( g, ex );
    init_aliens( g, al );

    /* Temporary stuff */
    for( i = 0; i < 5; i++ )
    {
        add_alien( g, al, rand() % g->w, rand() % g->h / 2 );
    }
    t->w = 25;

    while( !quit )
    {
        char key;

        while( ( key = get_key() ) )
        {
            switch( key )
            {
                case 'q':
                    quit = 1;
                    break;
                case 'p':
                    poz = !poz;
                    break;
                case 's':
                    skip = 1;
                    break;
                case 'h':
                    p->dir = -3;
                    break;
                case 'j':
                    if( p->y < g->h - 2 ) p->y += 1;
                    break;
                case 'k':
                    if( p->y > 1 ) p->y -= 1;
                    break;
                case 'l':
                    p->dir = 3;
                    break;
                case '\r':
                    if( p->nuke == 0 )
                    {
                        p->nuke = 40;
                        add_weapon( g, wp, p->x + 2, p->y, 2 );
                    }
                    break;
                case ' ':
                    if( p->weapon == 0 )
                    {
                        p->weapon = 4;
                        add_weapon( g, wp, p->x, p->y, 1 );
                        add_weapon( g, wp, p->x + 5, p->y, 1 );
                    }
                    break;
            }
        }

        usleep(40000);

        if( GET_RAND(0,10) == 0 )
        {
            add_alien( g, al, 0, rand() % g->h / 2 );
        }

        if( poz )
        {
            if( skip )
            {
                skip = 0;
            }
            else
            {
                continue;
            }
        }

        /* Scroll and update positions */
        collide_player_tunnel( g, p, t, ex );
        update_player( g, p );
        collide_player_tunnel( g, p, t, ex );

        update_starfield( g, sf );
        update_aliens( g, al );

        collide_weapons_tunnel( g, wp, t, ex );
        collide_weapons_aliens( g, wp, al, ex );
        update_weapons( g, wp );
        collide_weapons_tunnel( g, wp, t, ex );
        collide_weapons_aliens( g, wp, al, ex );

        update_explosions( g, ex );
        update_tunnel( g, t );

        /* Clear screen */
        clear_graphics();

        /* Print starfield, tunnel, aliens, player and explosions */
        draw_starfield( g, sf );
        draw_tunnel( g, t );
        draw_aliens( g, al );
        draw_player( g, p );
        draw_weapons( g, wp );
        draw_explosions( g, ex );

        /* Refresh */
        refresh_graphics();
    }

#if 0
    free_player( p );
    free_tunnel( t );
#endif
}

