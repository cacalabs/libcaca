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

/* Init tunnel */
player * create_player( game *g )
{
    player *p = malloc(sizeof(player));

    p->x = g->w / 2;
    p->y = g->h - 2;
    p->dir = 0;
    p->weapon = 0;
    p->nuke = 0;

    return p;
}

void free_player( player *p )
{
    free( p );
}

void draw_player( game *g, player *p )
{
    gfx_goto( p->x + 2, p->y - 2 );
    gfx_color( GREEN );
    gfx_putstr( "/\\" );
    gfx_goto( p->x + 1, p->y - 1 );
    gfx_putchar( '(' );
    gfx_color( YELLOW );
    gfx_putstr( "()" );
    gfx_color( GREEN );
    gfx_putchar( ')' );
    gfx_goto( p->x, p->y );
    gfx_color( GREEN );
    gfx_putstr( "I<__>I" );
}

void update_player( game *g, player *p )
{
    if( p->weapon )
    {
        p->weapon--;
    }

    if( p->nuke )
    {
        p->nuke--;
    }

    if( p->dir < 0 )
    {
        if( p->dir == -3 && p->x > -2 ) p->x -= 1;
        else if( p->x > -1 ) p->x -= 1;

        p->dir++;
    }
    else if( p->dir > 0 )
    {
        if( p->dir == 3 && p->x < g->w - 8 ) p->x += 1;
        else if( p->x < g->w - 7 ) p->x += 1;
        p->dir--;
    }
}

