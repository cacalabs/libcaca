
#include <stdlib.h>

#include "common.h"

static void draw_small_explosion( int x, int y, int frame );
static void draw_big_explosion( int x, int y, int frame );

void init_explosions( game *g, explosions *ex )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
        ex->n[i] = 0;
        ex->x[i] = -1;
        ex->y[i] = -1;
        ex->vx[i] = -1;
        ex->vy[i] = -1;
        ex->type[i] = 0;
    }
}

void add_explosion( game *g, explosions *ex, int x, int y, int vx, int vy, int type )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
        if( ex->n[i] <= 0 )
        {
            ex->x[i] = x;
            ex->y[i] = y;
            ex->vx[i] = vx;
            ex->vy[i] = vy;
            switch( type )
            {
                case 1: ex->n[i] = 13; break;
                case 2: ex->n[i] = 30; break;
                case 0: default: ex->n[i] = 7; break;
            }
            ex->type[i] = type;
            break;
        }
    }
}

void draw_explosions( game *g, explosions *ex )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
        if( ex->n[i] <= 0 )
        {
            continue;
        }

#if 0
        GFX_COLOR( GREEN );
        GFX_GOTO( ex->x[i] + 3, ex->y[i] );
        switch( GET_RAND(0,3) )
        {
        case 0:
            GFX_WRITE( 'p' );
            GFX_WRITE( 'i' );
            GFX_WRITE( 'f' );
            break;
        case 1:
            GFX_WRITE( 'p' );
            GFX_WRITE( 'a' );
            GFX_WRITE( 'f' );
            break;
        case 2:
            GFX_WRITE( 'p' );
            GFX_WRITE( 'o' );
            GFX_WRITE( 'u' );
            GFX_WRITE( 'f' );
            break;
        }
        GFX_WRITE( '!' );
#endif

        switch( ex->type[i] )
        {
            case 1:
                draw_big_explosion( ex->x[i], ex->y[i], ex->n[i] );
                break;
            case 0:
            default:
                draw_small_explosion( ex->x[i], ex->y[i], ex->n[i] );
                break;
        }

    }
}

void update_explosions( game *g, explosions *ex )
{
    int i;

    for( i = 0; i < EXPLOSIONS; i++ )
    {
        if( ex->n[i] > 0 )
        {
            ex->x[i] += ex->vx[i];
            ex->y[i] += ex->vy[i];
            ex->n[i]--;
        }
    }
}

static void draw_small_explosion( int x, int y, int frame )
{
    switch( frame )
    {
    default:
    case 6:
        GFX_COLOR( YELLOW );
        GFX_GOTO( x, y );
        GFX_WRITE( '+' );
        break;
    case 5:
        GFX_COLOR( YELLOW );
        GFX_GOTO( x, y );
        GFX_WRITE( 'o' );
        break;
    case 4:
        GFX_COLOR( YELLOW );
        GFX_GOTO( x, y-1 );
        GFX_WRITE( '_' );
        GFX_GOTO( x-1, y );
        GFX_WRITE( ')' );
        GFX_WRITE( '_' );
        GFX_WRITE( '(' );
        break;
    case 3:
        GFX_COLOR( YELLOW );
        GFX_GOTO( x-1, y-1 );
        GFX_WRITE( '.' );
        GFX_WRITE( '_' );
        GFX_WRITE( ',' );
        GFX_GOTO( x-1, y );
        GFX_WRITE( ')' );
        GFX_WRITE( '_' );
        GFX_WRITE( '(' );
        GFX_GOTO( x-1, y+1 );
        GFX_WRITE( '\'' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '`' );
        break;
    case 2:
        GFX_COLOR( YELLOW );
        GFX_GOTO( x-1, y-1 );
        GFX_WRITE( '.' );
        GFX_WRITE( 'v' );
        GFX_WRITE( ',' );
        GFX_GOTO( x-1, y );
        GFX_WRITE( '>' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '<' );
        GFX_GOTO( x-1, y+1 );
        GFX_WRITE( '\'' );
        GFX_WRITE( '^' );
        GFX_WRITE( '`' );
        break;
    case 1:
        GFX_COLOR( WHITE );
        GFX_GOTO( x-1, y-1 );
        GFX_WRITE( '.' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ',' );
        GFX_GOTO( x-1, y );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_GOTO( x-1, y+1 );
        GFX_WRITE( '\'' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '`' );
        break;
    }
}

static void draw_big_explosion( int x, int y, int frame )
{
    GFX_COLOR( YELLOW );

    switch( frame )
    {
    default:
    case 12:
        GFX_GOTO( x, y );
        GFX_WRITE( '+' );
        break;
    case 11:
        GFX_GOTO( x, y );
        GFX_WRITE( 'o' );
        break;
    case 10:
        GFX_GOTO( x, y-1 );
        GFX_WRITE( '_' );
        GFX_GOTO( x-1, y );
        GFX_WRITE( ')' );
        GFX_WRITE( '_' );
        GFX_WRITE( '(' );
        break;
    case 9:
        GFX_GOTO( x-1, y-1 );
        GFX_WRITE( '.' );
        GFX_WRITE( '_' );
        GFX_WRITE( ',' );
        GFX_GOTO( x-1, y );
        GFX_WRITE( ')' );
        GFX_WRITE( '_' );
        GFX_WRITE( '(' );
        GFX_GOTO( x-1, y+1 );
        GFX_WRITE( '\'' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '`' );
        break;
    case 8:
        GFX_GOTO( x-1, y-1 );
        GFX_WRITE( '.' );
        GFX_WRITE( 'v' );
        GFX_WRITE( ',' );
        GFX_GOTO( x-1, y );
        GFX_WRITE( '>' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '<' );
        GFX_GOTO( x-1, y+1 );
        GFX_WRITE( '\'' );
        GFX_WRITE( '^' );
        GFX_WRITE( '`' );
        break;
    case 6:
        GFX_COLOR( RED );
    case 7:
    case 5:
        GFX_GOTO( x-2, y-1 );
        GFX_WRITE( '_' );
        GFX_WRITE( '\\' );
        GFX_WRITE( '~' );
        GFX_WRITE( '/' );
        GFX_WRITE( '_' );
        GFX_GOTO( x-2, y );
        GFX_WRITE( '>' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '<' );
        GFX_GOTO( x-2, y+1 );
        GFX_WRITE( '~' );
        GFX_WRITE( '/' );
        GFX_WRITE( '_' );
        GFX_WRITE( '\\' );
        GFX_WRITE( '~' );
        break;
    case 3:
        GFX_COLOR( RED );
    case 4:
    case 2:
        GFX_GOTO( x-2, y-1 );
        GFX_WRITE( '_' );
        GFX_WRITE( '\\' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '/' );
        GFX_WRITE( '_' );
        GFX_GOTO( x-2, y );
        GFX_WRITE( '_' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '_' );
        GFX_GOTO( x-2, y+1 );
        GFX_WRITE( ' ' );
        GFX_WRITE( '/' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '\\' );
        GFX_WRITE( ' ' );
        break;
    case 1:
        GFX_COLOR( WHITE );
        GFX_GOTO( x-2, y-1 );
        GFX_WRITE( '.' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '\'' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ',' );
        GFX_GOTO( x-2, y );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_GOTO( x-2, y+1 );
        GFX_WRITE( '\'' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '.' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '`' );
        break;
    }
}

