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

#include <stdlib.h>

#include "common.h"

void init_bonus( game *g, bonus *bo )
{
    int i;

    for( i = 0; i < BONUS; i++ )
    {
        bo->type[i] = BONUS_NONE;
    }
}

void draw_bonus( game *g, bonus *bo )
{
    int i;

    for( i = 0; i < BONUS; i++ )
    {
        switch( bo->type[i] )
        {
            case BONUS_GREEN:
                ee_color( (bo->n[i]/2 % 3) ? EE_GREEN : EE_WHITE );
                ee_goto( bo->x[i]+1, bo->y[i]-1 );
                ee_putchar( '_' );
                ee_goto( bo->x[i], bo->y[i] );
                ee_putstr( "/ \\" );
                ee_goto( bo->x[i], bo->y[i]+1 );
                ee_putstr( "\\_/" );
                ee_color( EE_WHITE );
                ee_goto( bo->x[i]+1, bo->y[i] );
                ee_putchar( 'g' );
                break;
            case BONUS_LIFE:
                ee_color( (bo->n[i] % 3) ? EE_RED : EE_WHITE );
                ee_goto( bo->x[i]+1, bo->y[i]-1 );
                ee_putchar( '_' );
                ee_goto( bo->x[i]+3, bo->y[i]-1 );
                ee_putchar( '_' );
                ee_goto( bo->x[i], bo->y[i] );
                ee_putstr( "( ' )" );
                ee_goto( bo->x[i]+1, bo->y[i]+1 );
                ee_putstr( "`v'" );
                ee_color( EE_WHITE );
                ee_goto( bo->x[i]+3, bo->y[i] );
                ee_putchar( '^' );
                break;
            case BONUS_NONE:
                break;
        }
    }
}

void update_bonus( game *g, bonus *bo )
{
    int i;

    for( i = 0; i < BONUS; i++ )
    {
        switch( bo->type[i] )
        {
            case BONUS_GREEN:
                bo->n[i]++;
                bo->y[i]++;
                if( bo->y[i] > g->h )
                {
                    bo->type[i] = BONUS_NONE;
                }
                break;
            case BONUS_LIFE:
                bo->n[i]++;
                bo->y[i]++;
                if( bo->y[i] > g->h )
                {
                    bo->type[i] = BONUS_NONE;
                }
                break;
            case BONUS_NONE:
                break;
        }
    }
}

void add_bonus( game *g, bonus *bo, int x, int y, int type )
{
    int i;

    for( i = 0; i < BONUS; i++ )
    {
        if( bo->type[i] == BONUS_NONE )
        {
            bo->type[i] = type;
            bo->x[i] = x;
            bo->y[i] = y;
            bo->n[i] = 0;
            break;
        }
    }
}

