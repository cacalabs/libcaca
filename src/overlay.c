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

void draw_status( game *g )
{
    static char dots30[] = "------------------------------";
    static char dashes30[] = "==============================";

    /* Draw life jauge */
    ee_color( EE_GRAY );
    ee_goto( 4, 1 );
    ee_putstr( dots30 );

    if( g->p->life > MAX_LIFE * 7 / 10 )
    {
        ee_color( EE_GREEN );
    }
    else if( g->p->life > MAX_LIFE * 3 / 10 )
    {
        ee_color( EE_YELLOW );
    }
    else
    {
        ee_color( EE_RED );
    }

    ee_goto( 4, 1 );
    ee_putstr( dashes30 + ( MAX_LIFE - g->p->life ) * 30 / MAX_LIFE );

    ee_color( EE_WHITE );
    ee_goto( 1, 1 );
    ee_putstr( "L |" );
    ee_goto( 34, 1 );
    ee_putstr( "|" );

    /* Draw weapon jauge */
    ee_color( EE_GRAY );
    ee_goto( 42, 1 );
    ee_putstr( dots30 + 10 );

    if( g->p->special > MAX_SPECIAL * 9 / 10 )
    {
        ee_color( EE_WHITE );
    }
    else if( g->p->special > MAX_SPECIAL * 3 / 10 )
    {
        ee_color( EE_CYAN );
    }
    else
    {
        ee_color( EE_BLUE );
    }

    ee_goto( 42, 1 );
    ee_putstr( dashes30 + 10 + ( MAX_SPECIAL - g->p->special ) * 20 / MAX_SPECIAL );

    ee_color( EE_WHITE );
    ee_goto( 39, 1 );
    ee_putstr( "S |" );
    ee_goto( 62, 1 );
    ee_putstr( "|" );
}

