/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: starfield.c,v 1.4 2002/12/22 18:44:12 sam Exp $
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

void init_starfield( game *g, starfield *s )
{
    int i;

    for( i = 0; i < STARS; i++ )
    {
        s->x[i] = rand() % g->w;
        s->y[i] = rand() % g->h;
        s->z[i] = 1 + rand() % 3;
        s->ch[i] = (rand() % 2) ? '.' : '\'';
        s->c[i] = 6 + rand() % 2;
    }
}

void draw_starfield( game *g, starfield *s )
{
    int i;

    for( i = 0; i < STARS; i++ )
    {
        if( s->x[i] >= 0 )
        {
            gfx_color( s->c[i] );
            gfx_goto( s->x[i], s->y[i] );
            gfx_putchar( s->ch[i] );
        }
    }
}

void update_starfield( game *g, starfield *s )
{
    int i;

    for( i = 0; i < STARS; i++ )
    {
        if( s->x[i] < 0 )
        {
            s->x[i] = rand() % g->w;
            s->y[i] = 0;
            s->z[i] = 1 + rand() % 2;
            s->ch[i] = (rand() % 2) ? '.' : '\'';
            s->c[i] = 6 + rand() % 2;
        }
        else if( s->y[i] < g->h-1 )
        {
            s->y[i] += s->z[i];
        }
        else
        {
            s->x[i] = -1;
        }
    }
}

