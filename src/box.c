/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002-2003 Sam Hocevar <sam@zoy.org>
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

box * create_box( game *g, int x, int y, int w, int h )
{
    box *b = malloc( sizeof( box ) );

    b->x = x;
    b->y = y;
    b->w = w;
    b->h = h;
    b->frame = 0;

    return b;
}

void draw_box( game *g, box *b )
{
    int i, j, frame;

    ee_color( EE_YELLOW );

    /* Draw the thin horizontal line */
    if( b->frame < 8 )
    {
        for( i = b->x - b->w * b->frame / 16 ;
             i < b->x + b->w * b->frame / 16 ;
             i++ )
        {
            ee_goto( i, b->y );
            ee_putchar( 'X' );
        }

        return;
    }

    /* Draw the frame */
    frame = b->frame < 12 ? b->frame : 12;

    for( i = b->x - b->w / 2 ;
         i < b->x + b->w / 2 ;
         i++ )
    {
        ee_goto( i, b->y - b->h * (frame - 8) / 8 );
        ee_putchar( 'X' );
        ee_goto( i, b->y + b->h * (frame - 8) / 8 );
        ee_putchar( 'X' );
    }

    for( j = b->y - b->h * (frame - 8) / 8 ;
         j < b->y + b->h * (frame - 8) / 8 ;
         j++ )
    {
        ee_goto( b->x - b->w / 2, j );
        ee_putchar( 'X' );
        ee_goto( b->x + b->w / 2 - 1, j );
        ee_putchar( 'X' );
    }

    ee_color( EE_BLACK );

    for( j = b->y - b->h * (frame - 8) / 8 + 1 ;
         j < b->y + b->h * (frame - 8) / 8 ;
         j++ )
    {
        for( i = b->x - b->w / 2 + 1 ;
             i < b->x + b->w / 2 - 1 ;
             i++ )
        {
            ee_goto( i, j );
            ee_putchar( 'X' );
        }
    }

    if( b->frame < 12 )
    {
        return;
    }

    /* Draw the text inside the frame */
    ee_color( EE_YELLOW );

    /* FIXME: use a font */
    ee_goto( b->x - b->w / 2 + 12, b->y - b->h / 2 + 2 );
    ee_putstr( "XXXX.  .XXXX  X   X  .XXXX  .XXXX  XXXX." );
    ee_goto( b->x - b->w / 2 + 12, b->y - b->h / 2 + 3 );
    ee_putstr( "X  `X  X'  X  X   X  X'     X'     X  `X" );
    ee_goto( b->x - b->w / 2 + 12, b->y - b->h / 2 + 4 );
    ee_putstr( "XXXX'  XXXXX  X   X  `XXX   XXXX   X   X" );
    ee_goto( b->x - b->w / 2 + 12, b->y - b->h / 2 + 5 );
    ee_putstr( "X'     X' `X  X. ,X     `X  X'     X  ,X" );
    ee_goto( b->x - b->w / 2 + 12, b->y - b->h / 2 + 6 );
    ee_putstr( "X      X   X  `XXXX  XXXX'  `XXXX  XXXX'" );
}

void free_box( box *b )
{
    free( b );
}

