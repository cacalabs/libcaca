/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: weapons.c,v 1.10 2002/12/22 23:01:35 sam Exp $
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

static void draw_bomb( int x, int y, int vx, int vy );
static void draw_nuke( int x, int y, int frame );
static void draw_beam( int x, int y, int frame );
static void draw_circle( int x, int y, int r, char c );
static void draw_fragbomb( int x, int y, int frame );

void init_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        wp->type[i] = WEAPON_NONE;
    }
}

void draw_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        switch( wp->type[i] )
        {
            case WEAPON_LASER:
                gfx_color( WHITE );
                gfx_goto( wp->x[i] >> 4, wp->y[i] >> 4 );
                gfx_putchar( '|' );
                gfx_color( CYAN );
                gfx_goto( wp->x[i] >> 4, (wp->y[i] >> 4) + 1 );
                gfx_putchar( '|' );
                break;
            case WEAPON_SEEKER:
                gfx_color( CYAN );
                gfx_goto( wp->x3[i] >> 4, wp->y3[i] >> 4 );
                gfx_putchar( '.' );
                gfx_goto( wp->x2[i] >> 4, wp->y2[i] >> 4 );
                gfx_putchar( 'o' );
                gfx_color( WHITE );
                gfx_goto( wp->x[i] >> 4, wp->y[i] >> 4 );
                gfx_putchar( '@' );
                break;
            case WEAPON_BOMB:
                gfx_color( GRAY );
                gfx_goto( (wp->x[i] - wp->vx[i]) >> 4, (wp->y[i] - wp->vy[i]) >> 4 );
                gfx_putchar( '.' );
                gfx_goto( (wp->x3[i] - wp->vx[i]) >> 4, (wp->y3[i] - wp->vy[i]) >> 4 );
                gfx_putchar( '.' );
                gfx_goto( (wp->x2[i] - wp->vx[i]) >> 4, (wp->y2[i] - wp->vy[i]) >> 4 );
                gfx_putchar( '.' );
                gfx_goto( wp->x3[i] >> 4, wp->y3[i] >> 4 );
                gfx_putchar( '.' );
                gfx_goto( wp->x2[i] >> 4, wp->y2[i] >> 4 );
                gfx_putchar( '.' );
                draw_bomb( wp->x[i] >> 4, wp->y[i] >> 4, wp->vx[i], wp->vy[i] );
                break;
            case WEAPON_FRAGBOMB:
                draw_fragbomb( wp->x[i] >> 4, wp->y[i] >> 4, wp->n[i] );
                break;
            case WEAPON_BEAM:
                draw_beam( wp->x[i] >> 4, wp->y[i] >> 4, wp->n[i] );
                break;
            case WEAPON_NUKE:
                draw_nuke( wp->x[i] >> 4, wp->y[i] >> 4, wp->n[i] );
                break;
            case WEAPON_LIGHTNING:
            case WEAPON_NONE:
                break;
        }
    }
}

void update_weapons( game *g, weapons *wp )
{
    int i, j, dist, xmin, ymin, dx, dy, xnew, ynew;

    for( i = 0; i < WEAPONS; i++ )
    {
        switch( wp->type[i] )
        {
            case WEAPON_LASER:
                wp->x[i] += wp->vx[i];
                wp->y[i] += wp->vy[i];
                if( wp->y[i] < 0 )
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_BOMB:
            case WEAPON_SEEKER:
                /* Update tail */
                wp->x3[i] = wp->x2[i];
                wp->y3[i] = wp->y2[i];

                wp->x2[i] = wp->x[i];
                wp->y2[i] = wp->y[i];

                wp->x[i] += wp->vx[i];
                wp->y[i] += wp->vy[i];

                if( wp->y[i] < 0 )
                {
                    wp->type[i] = WEAPON_NONE;
                    break;
                }

                if( wp->n[i] < 0 )
                {
                    /* Stop updating direction */
                    break;
                }

                wp->n[i]--;

                /* Estimate our position in 2 frames */
                xnew = wp->x[i] + 4 * wp->vx[i];
                ynew = wp->y[i] + 4 * wp->vy[i];

                xmin = xnew;
                ymin = - (g->h << 4);
                dist = (xnew - xmin) * (xnew - xmin)
                        + 4 * (ynew - ymin) * (ynew - ymin);

                /* Find the nearest alien */
                for( j = 0; j < ALIENS; j++ )
                {
                    if( g->al->type[j] != ALIEN_NONE )
                    {
                        int alx = g->al->x[j] << 4;
                        int aly = g->al->y[j] << 4;
                        int new = (xnew - alx) * (xnew - alx)
                                   + 4 * (ynew - aly) * (ynew - aly);
                        if( new <= dist )
                        {
                            dist = new;
                            xmin = alx;
                            ymin = aly;
                        }
                    }
                }

                /* Find our new direction */
                dx = xmin - wp->x[i];
                dy = ymin - wp->y[i];

                /* Normalize and update speed */
                if( dx | dy )
                {
                    int norm = r00t( dx * dx + dy * dy );

                    wp->vx[i] = (7 * wp->vx[i] + (dx * 48) / norm ) / 8;
                    wp->vy[i] = (7 * wp->vy[i] + (dy * 24) / norm ) / 8;
                }
                break;

            case WEAPON_FRAGBOMB:
                /* If n was set to -1, the fragbomb just exploded */
                if( wp->n[i] == -1 )
                {
                    int coords[] =
                    {
                        24,  0,   -24,  0,    0,  24,     0, -24,
                        24,  8,   -24,  8,   24,  -8,   -24,  -8,
                        16, 16,   -16, 16,   16, -16,   -16, -16
                    };

                    for( j = 0; j < 12; j++ )
                    {
                        add_weapon( g, g->wp, wp->x[i] + coords[2*j], wp->y[i] + coords[2*j+1], coords[2*j], coords[2*j+1], WEAPON_SEEKER );
                    }

                    wp->type[i] = WEAPON_NONE;
                }

                wp->x[i] += wp->vx[i];
                wp->y[i] += wp->vy[i];
                wp->n[i]++;
                if( wp->y[i] < 0 )
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;

            case WEAPON_BEAM:
                wp->x[i] = (g->p->x + 2) << 4;
                wp->y[i] = g->p->y << 4;
                wp->n[i]--;
                if( wp->n[i] < 0 )
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_NUKE:
                wp->n[i]--;
                if( wp->n[i] < 0 )
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_LIGHTNING:
            case WEAPON_NONE:
                break;
        }
    }
}

void add_weapon( game *g, weapons *wp, int x, int y, int vx, int vy, int type )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        if( wp->type[i] == WEAPON_NONE )
        {
            wp->x[i] = x;
            wp->y[i] = y;
            wp->vx[i] = vx;
            wp->vy[i] = vy;
            wp->type[i] = type;
            wp->n[i] = 0;
            switch( type )
            {
                case WEAPON_LASER:
                    break;
                case WEAPON_FRAGBOMB:
                    break;
                case WEAPON_SEEKER:
                case WEAPON_BOMB:
                    wp->x2[i] = x;
                    wp->y2[i] = y;
                    wp->x3[i] = x;
                    wp->y3[i] = y;
                    wp->n[i] = 20;
                    break;
                case WEAPON_BEAM:
                    wp->n[i] = 25;
                    break;
                case WEAPON_NUKE:
                    wp->n[i] = 25;
                    break;
                case WEAPON_NONE:
                    break;
            }
            break;
        }
    }
}

static void draw_bomb( int x, int y, int vx, int vy )
{
    vy *= 2;
    gfx_color( CYAN );

    if( vx > vy )
    {
        if( vx > -vy ) /* right quarter */
        {
            if( vy > vx/4 )
            {
                /* -1pi/6 */
                gfx_goto( x-4, y-1 );
                gfx_putstr( "/`-." );
                gfx_goto( x-4, y );
                gfx_putstr( "`-._\\" );
                gfx_color( WHITE );
                gfx_goto( x-1, y );
                gfx_putstr( "_\\" );
                gfx_goto( x, y+1 );
                gfx_putchar( '`' );
            }
            else if( vy < -vx/4 )
            {
                /* 1pi/6 */
                gfx_goto( x-4, y );
                gfx_putstr( ",-' " );
                gfx_goto( x-4, y+1 );
                gfx_putstr( "\\,-'" );
                gfx_color( WHITE );
                gfx_goto( x-1, y-1 );
                gfx_putstr( "_," );
                gfx_goto( x, y );
                gfx_putchar( '/' );
            }
            else
            {
                /* 0pi/6 */
                gfx_goto( x-4, y-1 );
                gfx_putstr( "____" );
                gfx_goto( x-5, y );
                gfx_putstr( "|____" );
                gfx_color( WHITE );
                gfx_goto( x, y );
                gfx_putchar( '>' );
            }
        }
        else /* top quarter */
        {
            if( vx > -vy/4 )
            {
                /* 2pi/6 */
                gfx_goto( x-2, y );
                gfx_putstr( "/ " );
                gfx_goto( x-3, y+1 );
                gfx_putstr( "/ /" );
                gfx_goto( x-3, y+2 );
                gfx_putstr( "`'" );
                gfx_color( WHITE );
                gfx_goto( x-1, y-1 );
                gfx_putstr( "_," );
                gfx_goto( x, y );
                gfx_putchar( '|' );
            }
            else if( vx < vy/4 )
            {
                /* 4pi/6 */
                gfx_goto( x+1, y );
                gfx_putstr( " \\" );
                gfx_goto( x+1, y+1 );
                gfx_putstr( "\\ \\" );
                gfx_goto( x+2, y+2 );
                gfx_putstr( "`'" );
                gfx_color( WHITE );
                gfx_goto( x, y-1 );
                gfx_putstr( "._" );
                gfx_goto( x, y );
                gfx_putchar( '|' );
            }
            else
            {
                /* 3pi/6 */
                gfx_goto( x-1, y+1 );
                gfx_putstr( "| |" );
                gfx_goto( x-1, y+2 );
                gfx_putstr( "|_|" );
                gfx_color( WHITE );
                gfx_goto( x-1, y );
                gfx_putstr( ",^." );
            }
        }
    }
    else
    {
        if( vx > -vy ) /* bottom quarter */
        {
            if( vx > vy/4 )
            {
                /* -2pi/6 */
                gfx_goto( x-2, y-2 );
                gfx_putstr( ",." );
                gfx_goto( x-2, y-1 );
                gfx_putstr( "\\ \\" );
                gfx_goto( x-1, y );
                gfx_putchar( '\\' );
                gfx_color( WHITE );
                gfx_goto( x, y );
                gfx_putstr( "_|" );
            }
            else if( vx < -vy/4 )
            {
                /* -4pi/6 */
                gfx_goto( x+1, y-2 );
                gfx_putstr( ",." );
                gfx_goto( x, y-1 );
                gfx_putstr( "/ /" );
                gfx_goto( x+1, y );
                gfx_putchar( '/' );
                gfx_color( WHITE );
                gfx_goto( x-1, y );
                gfx_putstr( "|_/" );
            }
            else
            {
                /* -3pi/6 */
                gfx_goto( x, y-3 );
                gfx_putchar( '_' );
                gfx_goto( x-1, y-2 );
                gfx_putstr( "| |" );
                gfx_goto( x-1, y-1 );
                gfx_putstr( "| |" );
                gfx_color( WHITE );
                gfx_goto( x-1, y );
                gfx_putstr( "`v'" );
            }
        }
        else /* left quarter */
        {
            if( vy > -vx/4 )
            {
                /* -5pi/6 */
                gfx_goto( x+1, y-1 );
                gfx_putstr( ",-'\\" );
                gfx_goto( x+2, y );
                gfx_putstr( ",-'" );
                gfx_goto( x, y+1 );
                gfx_putchar( '\'' );
                gfx_color( WHITE );
                gfx_goto( x, y );
                gfx_putstr( "/_" );
            }
            else if( vy < vx/4 )
            {
                /* 5pi/6 */
                gfx_goto( x+1, y );
                gfx_putstr( " `-." );
                gfx_goto( x+1, y+1 );
                gfx_putstr( "`-./" );
                gfx_color( WHITE );
                gfx_goto( x, y-1 );
                gfx_putstr( "._" );
                gfx_goto( x, y );
                gfx_putchar( '\\' );
            }
            else
            {
                /* 6pi/6 */
                gfx_goto( x+1, y-1 );
                gfx_putstr( "____" );
                gfx_goto( x+1, y );
                gfx_putstr( "____|" );
                gfx_color( WHITE );
                gfx_goto( x, y );
                gfx_putchar( '<' );
            }
        }
    }
}

static void draw_fragbomb( int x, int y, int frame )
{
    gfx_color( WHITE );

    gfx_color( frame & 1 ? CYAN : WHITE );
    gfx_goto( x-2, y );
    gfx_putstr( "(    )" );
    gfx_goto( x-1, y+1 );
    gfx_putstr( "`--'" );

    gfx_color( frame & 1 ? WHITE : CYAN );
    gfx_goto( x-1, y-1 );
    gfx_putstr( ",--." );
    gfx_goto( x, y );
    gfx_putstr( "'," );

    switch( frame % 4 )
    {
    case 0:
        gfx_color( CYAN );
        gfx_goto( x, y + 2 );
        gfx_putchar( 'O' );
        gfx_goto( x + 2, y + 2 );
        gfx_putchar( 'o' );
        gfx_goto( x + 1, y + 3 );
        gfx_putchar( 'o' );
        gfx_color( GRAY );
        gfx_goto( x - 1, y + 3 );
        gfx_putchar( '°' );
        gfx_goto( x + 2, y + 4 );
        gfx_putchar( '°' );
        gfx_goto( x, y + 4 );
        gfx_putchar( '.' );
        gfx_goto( x + 1, y + 5 );
        gfx_putchar( '.' );
        break;
    case 1:
        gfx_color( CYAN );
        //gfx_goto( x, y + 1 );
        //gfx_putchar( 'O' );
        gfx_goto( x + 1, y + 2 );
        gfx_putchar( 'O' );
        gfx_goto( x, y + 3 );
        gfx_putchar( 'o' );
        gfx_color( GRAY );
        gfx_goto( x + 2, y + 3 );
        gfx_putchar( '°' );
        gfx_goto( x + 1, y + 4 );
        gfx_putchar( '°' );
        gfx_goto( x - 1, y + 4 );
        gfx_putchar( '.' );
        gfx_goto( x + 2, y + 5 );
        gfx_putchar( '.' );
        break;
    case 2:
        gfx_color( CYAN );
        //gfx_goto( x - 1, y + 1 );
        //gfx_putchar( 'O' );
        gfx_goto( x + 2, y + 2 );
        gfx_putchar( 'O' );
        gfx_goto( x, y + 2 );
        gfx_putchar( 'o' );
        gfx_goto( x + 1, y + 3 );
        gfx_putchar( 'o' );
        gfx_color( GRAY );
        gfx_goto( x, y + 4 );
        gfx_putchar( '°' );
        gfx_goto( x + 2, y + 4 );
        gfx_putchar( '.' );
        gfx_goto( x + 1, y + 5 );
        gfx_putchar( '.' );
        break;
    case 3:
        gfx_color( CYAN );
        //gfx_goto( x + 2, y + 1 );
        //gfx_putchar( 'O' );
        gfx_goto( x + 1, y + 2 );
        gfx_putchar( 'O' );
        gfx_goto( x - 1, y + 2 );
        gfx_putchar( 'o' );
        gfx_goto( x + 2, y + 3 );
        gfx_putchar( 'o' );
        gfx_color( GRAY );
        gfx_goto( x, y + 3 );
        gfx_putchar( '°' );
        gfx_goto( x + 1, y + 4 );
        gfx_putchar( '°' );
        gfx_goto( x, y + 5 );
        gfx_putchar( '.' );
        break;
    }
}

static void draw_beam( int x, int y, int frame )
{
    int r = (29 - frame) * (29 - frame) / 8;
    int i;

    switch( frame )
    {
        case 24:
            gfx_color( WHITE );
            gfx_goto( x, y-3 );
            gfx_putstr( "__" );
            gfx_goto( x-1, y-2 );
            gfx_putchar( '\'' );
            gfx_goto( x+2, y-2 );
            gfx_putchar( '`' );
            break;
        case 23:
            gfx_color( CYAN );
            gfx_goto( x, y-3 );
            gfx_putstr( "__" );
            gfx_color( WHITE );
            gfx_goto( x-2, y-2 );
            gfx_putstr( "-'" );
            gfx_goto( x+2, y-2 );
            gfx_putstr( "`-" );
            break;
        case 22:
            gfx_color( CYAN );
            gfx_goto( x, y-3 );
            gfx_putstr( "__" );
            gfx_goto( x-1, y-2 );
            gfx_putchar( '\'' );
            gfx_goto( x+2, y-2 );
            gfx_putchar( '`' );
            gfx_color( WHITE );
            gfx_goto( x-3, y-2 );
            gfx_putstr( ",-" );
            gfx_goto( x+3, y-2 );
            gfx_putstr( "-." );
            break;
        case 21:
            gfx_color( CYAN );
            gfx_goto( x-1, y-3 );
            gfx_putstr( "____" );
            gfx_goto( x-2, y-2 );
            gfx_putchar( '\'' );
            gfx_goto( x+3, y-2 );
            gfx_putchar( '`' );
            gfx_color( WHITE );
            gfx_goto( x-4, y-2 );
            gfx_putstr( ",-" );
            gfx_goto( x+4, y-2 );
            gfx_putstr( "-." );
            break;
        case 20:
            gfx_color( WHITE );
            gfx_goto( x, y-3 );
            gfx_putstr( "%%" );
            gfx_goto( x-4, y-2 );
            gfx_putchar( ',' );
            gfx_goto( x+5, y-2 );
            gfx_putchar( '.' );
            gfx_color( CYAN );
            gfx_goto( x-1, y-3 );
            gfx_putchar( ':' );
            gfx_goto( x+2, y-3 );
            gfx_putchar( ':' );
            gfx_goto( x-3, y-2 );
            gfx_putstr( "-'" );
            gfx_goto( x+3, y-2 );
            gfx_putstr( "`-" );
            break;
        case 19:
            gfx_color( WHITE );
            gfx_goto( x, y-4 );
            gfx_putstr( "%%" );
            gfx_goto( x, y-3 );
            gfx_putstr( "##" );
            gfx_color( CYAN );
            gfx_goto( x-1, y-4 );
            gfx_putchar( ':' );
            gfx_goto( x+2, y-4 );
            gfx_putchar( ':' );
            gfx_goto( x-1, y-3 );
            gfx_putchar( '%' );
            gfx_goto( x+2, y-3 );
            gfx_putchar( '%' );
            gfx_goto( x-4, y-2 );
            gfx_putstr( ",-'" );
            gfx_goto( x+3, y-2 );
            gfx_putstr( "`-." );
            gfx_color( BLUE );
            gfx_goto( x-2, y-3 );
            gfx_putchar( ':' );
            gfx_goto( x+3, y-3 );
            gfx_putchar( ':' );
            break;
        case 18:
        default:
            r = (18 - frame) * (18 - frame);
            gfx_color( WHITE );
            gfx_goto( x-1, y-5-r );
            gfx_putstr( ":%%:" );
            gfx_goto( x-1, y-4-r );
            gfx_putstr( "%##%" );
            gfx_color( CYAN );
            gfx_goto( x-2, y-4-r );
            gfx_putchar( ':' );
            gfx_goto( x+3, y-4-r );
            gfx_putchar( ':' );
            gfx_goto( x-2, y-2 );
            gfx_putchar( '\'' );
            gfx_goto( x+3, y-2 );
            gfx_putchar( '`' );
            gfx_color( BLUE );
            gfx_goto( x-3, y-2 );
            gfx_putchar( ':' );
            gfx_goto( x+4, y-2 );
            gfx_putchar( ':' );
            for( i = 0; i <= r; i++ )
            {
                gfx_color( WHITE );
                gfx_goto( x-1, y-3-i );
                gfx_putstr( (i+frame) % 5 ? "####" : "%%%%" );
                gfx_color( CYAN );
                gfx_goto( x-2, y-3-i );
                gfx_putchar( '%' );
                gfx_goto( x+3, y-3-i );
                gfx_putchar( '%' );
                gfx_color( BLUE );
                gfx_goto( x-3, y-3-i );
                gfx_putchar( ':' );
                gfx_goto( x+4, y-3-i );
                gfx_putchar( ':' );
            }
            break;
    }
}

static void draw_nuke( int x, int y, int frame )
{
    int r = (29 - frame) * (29 - frame) / 8;

    /* Lots of duplicate pixels, but we don't care */
    gfx_color( BLUE );
    draw_circle( x, y, r++, ':' );
    gfx_color( CYAN );
    draw_circle( x, y, r++, '%' );
    gfx_color( WHITE );
    draw_circle( x, y, r++, '#' );
    draw_circle( x, y, r++, '#' );
}

static void draw_circle( int x, int y, int r, char c )
{
    int test, dx, dy;

    /* Optimized Bresenham. Kick ass. */
    for( test = 0, dx = 0, dy = r ; dx <= dy ; dx++ )
    {
        gfx_putcharTO( x + dx, y + dy / 2, c );
        gfx_putcharTO( x - dx, y + dy / 2, c );
        gfx_putcharTO( x + dx, y - dy / 2, c );
        gfx_putcharTO( x - dx, y - dy / 2, c );

        gfx_putcharTO( x + dy, y + dx / 2, c );
        gfx_putcharTO( x - dy, y + dx / 2, c );
        gfx_putcharTO( x + dy, y - dx / 2, c );
        gfx_putcharTO( x - dy, y - dx / 2, c );

        test += test > 0 ? dx - dy-- : dx;
    }
}

