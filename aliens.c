
#include <stdlib.h>

#include "common.h"

static void draw_alien( game *g, int x, int y, int type );

void init_aliens( game *g, aliens *al )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        al->x[i] = -1;
        al->y[i] = -1;
        al->img[i] = 0;
        al->life[i] = 0;
    }
}

void draw_aliens( game *g, aliens *al )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        if( al->y[i] >= 0 )
        {
            draw_alien( g, al->x[i], al->y[i], al->img[i] % 2 );
        }
    }
}

void update_aliens( game *g, aliens *al )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        if( al->y[i] < 0 )
        {
            //al->x[i] = g->w;
        }
        else
        {
            al->x[i] = ((al->x[i] + 5) % (g->w + 3)) - 3;
            al->y[i] = al->y[i] + (rand() % 8) / 7 - (rand() % 8) / 7;
            al->img[i] = al->img[i] + rand() % 4;

            /* Check collisions */
            if( al->y[i] < 0 ) al->y[i] = 0;
            if( al->y[i] > g->w - 1 ) al->y[i] = g->w - 1;
        }
    }
}

void add_alien( game *g, aliens *al, int x, int y )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        if( al->y[i] < 0 )
        {
            al->x[i] = x;
            al->y[i] = y;
            al->img[i] = 0;
            al->life[i] = 2;
            break;
        }
    }
}

static void draw_alien( game *g, int x, int y, int type )
{
    switch( type )
    {
    case 0:
        GFX_COLOR( MAGENTA );
        GFX_GOTO( x, y );
        GFX_WRITE( ',' );
        GFX_WRITE( '-' );
        GFX_WRITE( '-' );
        GFX_WRITE( '-' );
        GFX_WRITE( '.' );
        GFX_GOTO( x, y+1 );
        GFX_WRITE( '\\' );
        GFX_COLOR( WHITE );
        GFX_WRITE( 'o' );
        GFX_WRITE( ' ' );
        GFX_WRITE( 'O' );
        GFX_COLOR( MAGENTA );
        GFX_WRITE( '/' );
        GFX_GOTO( x, y+2 );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        break;
    case 1:
        GFX_COLOR( MAGENTA );
        GFX_GOTO( x, y );
        GFX_WRITE( ',' );
        GFX_WRITE( '-' );
        GFX_WRITE( '-' );
        GFX_WRITE( '-' );
        GFX_WRITE( '.' );
        GFX_GOTO( x, y+1 );
        GFX_WRITE( '\\' );
        GFX_COLOR( WHITE );
        GFX_WRITE( 'O' );
        GFX_WRITE( ' ' );
        GFX_WRITE( 'o' );
        GFX_COLOR( MAGENTA );
        GFX_WRITE( '/' );
        GFX_GOTO( x, y+2 );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        GFX_WRITE( '^' );
        break;
    }
}

#if 0
void draw_rock( int x, int y, int type )
{
    switch( type )
    {
    case 0:
        GFX_COLOR( RED );
        GFX_GOTO( x, y );
        GFX_WRITE( '/' );
        GFX_WRITE( '\\' );
        GFX_WRITE( '_' );
        GFX_WRITE( '/' );
        GFX_WRITE( '\\' );
        GFX_GOTO( x, y+1 );
        GFX_WRITE( '>' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '/' );
        GFX_GOTO( x, y+2 );
        GFX_WRITE( '\\' );
        GFX_WRITE( '/' );
        GFX_WRITE( '\\' );
        GFX_WRITE( '_' );
        GFX_WRITE( '>' );
        break;
    case 1:
        GFX_COLOR( RED );
        GFX_GOTO( x, y );
        GFX_WRITE( '_' );
        GFX_WRITE( '/' );
        GFX_WRITE( '\\' );
        GFX_WRITE( '/' );
        GFX_WRITE( '>' );
        GFX_GOTO( x, y+1 );
        GFX_WRITE( '\\' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( ' ' );
        GFX_WRITE( '\\' );
        GFX_GOTO( x, y+2 );
        GFX_WRITE( '<' );
        GFX_WRITE( '_' );
        GFX_WRITE( '/' );
        GFX_WRITE( '\\' );
        GFX_WRITE( '/' );
        break;
    }
}
#endif
