/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: overlay.c,v 1.1 2002/12/23 15:06:13 sam Exp $
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

void draw_overlay( game *g )
{
    static char dots30[] = "------------------------------";
    static char dashes30[] = "==============================";

    /* Draw life jauge */
    gfx_color( GRAY );
    gfx_goto( 2, 1 );
    gfx_putstr( dots30 );

    if( g->p->life > MAX_LIFE * 7 / 10 )
    {
        gfx_color( GREEN );
    }
    else if( g->p->life > MAX_LIFE * 3 / 10 )
    {
        gfx_color( YELLOW );
    }
    else
    {
        gfx_color( RED );
    }

    gfx_goto( 2, 1 );
    gfx_putstr( dashes30 + ( MAX_LIFE - g->p->life ) * 30 / MAX_LIFE );

    gfx_color( WHITE );
    gfx_goto( 1, 1 );
    gfx_putstr( "|" );
    gfx_goto( 32, 1 );
    gfx_putstr( "| L" );

    /* Draw weapon jauge */
    gfx_color( GRAY );
    gfx_goto( 38, 1 );
    gfx_putstr( dots30 + 10 );

    if( g->p->special > MAX_SPECIAL * 9 / 10 )
    {
        gfx_color( WHITE );
    }
    else if( g->p->special > MAX_SPECIAL * 3 / 10 )
    {
        gfx_color( CYAN );
    }
    else
    {
        gfx_color( BLUE );
    }

    gfx_goto( 38, 1 );
    gfx_putstr( dashes30 + 10 + ( MAX_SPECIAL - g->p->special ) * 20 / MAX_SPECIAL );

    gfx_color( WHITE );
    gfx_goto( 37, 1 );
    gfx_putstr( "|" );
    gfx_goto( 58, 1 );
    gfx_putstr( "| S" );
}

