
#include <stdlib.h>

#include "common.h"

static void draw_small_explosion( int x, int y, int frame );
static void draw_medium_explosion( int x, int y, int frame );

void init_explosions( game *g, explosions *ex )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
        ex->type[i] = EXPLOSION_NONE;
    }
}

void add_explosion( game *g, explosions *ex, int x, int y, int vx, int vy, int type )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
        if( ex->type[i] == EXPLOSION_NONE )
        {
            ex->type[i] = type;
            ex->x[i] = x;
            ex->y[i] = y;
            ex->vx[i] = vx;
            ex->vy[i] = vy;
            switch( type )
            {
                case EXPLOSION_MEDIUM:
                    ex->n[i] = 11;
                    break;
                case EXPLOSION_SMALL:
                    ex->n[i] = 7;
                    break;
            }
            break;
        }
    }
}

void draw_explosions( game *g, explosions *ex )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
#if 0
        gfx_color( GREEN );
        gfx_goto( ex->x[i] + 3, ex->y[i] );
        switch( GET_RAND(0,3) )
        {
        case 0:
            gfx_putchar( 'p' );
            gfx_putchar( 'i' );
            gfx_putchar( 'f' );
            break;
        case 1:
            gfx_putchar( 'p' );
            gfx_putchar( 'a' );
            gfx_putchar( 'f' );
            break;
        case 2:
            gfx_putchar( 'p' );
            gfx_putchar( 'o' );
            gfx_putchar( 'u' );
            gfx_putchar( 'f' );
            break;
        }
        gfx_putchar( '!' );
#endif

        switch( ex->type[i] )
        {
            case EXPLOSION_MEDIUM:
                draw_medium_explosion( ex->x[i], ex->y[i], ex->n[i] );
                break;
            case EXPLOSION_SMALL:
                draw_small_explosion( ex->x[i], ex->y[i], ex->n[i] );
                break;
            case EXPLOSION_NONE:
                break;
        }
    }
}

void update_explosions( game *g, explosions *ex )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
        switch( ex->type[i] )
        {
            case EXPLOSION_MEDIUM:
            case EXPLOSION_SMALL:
                ex->x[i] += ex->vx[i];
                ex->y[i] += ex->vy[i];
                ex->n[i]--;
                if( ex->n[i] < 0 )
                {
                    ex->type[i] = EXPLOSION_NONE;
                }
                break;
            case EXPLOSION_NONE:
                break;
        }
    }
}

static void draw_small_explosion( int x, int y, int frame )
{
    switch( frame )
    {
    case 6:
        gfx_color( YELLOW );
        gfx_goto( x, y );
        gfx_putchar( '+' );
        break;
    case 5:
        gfx_color( YELLOW );
        gfx_goto( x, y );
        gfx_putchar( 'o' );
        break;
    case 4:
        gfx_color( YELLOW );
        gfx_goto( x, y-1 );
        gfx_putchar( '_' );
        gfx_goto( x-1, y );
        gfx_putstr( ")_(" );
        break;
    case 3:
        gfx_color( YELLOW );
        gfx_goto( x-1, y-1 );
        gfx_putstr( "._," );
        gfx_goto( x-1, y );
        gfx_putstr( ")_(" );
        gfx_goto( x-1, y+1 );
        gfx_putstr( "\' `" );
        break;
    case 2:
        gfx_color( YELLOW );
        gfx_goto( x-1, y-1 );
        gfx_putstr( ".v," );
        gfx_goto( x-1, y );
        gfx_putstr( "> <" );
        gfx_goto( x-1, y+1 );
        gfx_putstr( "\'^`" );
        break;
    case 1:
        gfx_color( WHITE );
        gfx_goto( x-1, y-1 );
        gfx_putstr( ". ," );
        gfx_goto( x-1, y );
        gfx_putstr( "   " );
        gfx_goto( x-1, y+1 );
        gfx_putstr( "\' `" );
        break;
    }
}

static void draw_medium_explosion( int x, int y, int frame )
{
    gfx_color( YELLOW );

    switch( frame )
    {
    case 10:
        gfx_goto( x, y );
        gfx_putchar( '+' );
        break;
    case 9:
        gfx_goto( x, y );
        gfx_putchar( 'o' );
        break;
    case 8:
        gfx_goto( x, y-1 );
        gfx_putchar( '_' );
        gfx_goto( x-1, y );
        gfx_putstr( ")_(" );
        break;
    case 7:
        gfx_goto( x-1, y-1 );
        gfx_putstr( "._," );
        gfx_goto( x-1, y );
        gfx_putstr( ")_(" );
        gfx_goto( x-1, y+1 );
        gfx_putstr( "\' `" );
        break;
    case 6:
        gfx_goto( x-1, y-1 );
        gfx_putstr( ".v," );
        gfx_goto( x-1, y );
        gfx_putstr( "> <" );
        gfx_goto( x-1, y+1 );
        gfx_putstr( "\'^`" );
        break;
    case 5:
        gfx_color( RED );
    case 4:
        gfx_goto( x-2, y-1 );
        gfx_putstr( "_\\~/_" );
        gfx_goto( x-2, y );
        gfx_putstr( ">   <" );
        gfx_goto( x-2, y+1 );
        gfx_putstr( "~/_\\~" );
        break;
    case 3:
        gfx_color( RED );
    case 2:
        gfx_goto( x-2, y-1 );
        gfx_putstr( "_\\ /_" );
        gfx_goto( x-2, y );
        gfx_putstr( "_   _" );
        gfx_goto( x-2, y+1 );
        gfx_putstr( " / \\ " );
        break;
    case 1:
        gfx_color( WHITE );
        gfx_goto( x-2, y-1 );
        gfx_putstr( ". \' ," );
        gfx_goto( x-2, y );
        gfx_putstr( "     " );
        gfx_goto( x-2, y+1 );
        gfx_putstr( "\' . `" );
        break;
    }
}

