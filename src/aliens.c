
#include <stdlib.h>

#include "common.h"

static void draw_alien_poolp( game *g, int x, int y, int frame );
static void draw_alien_bool( game *g, int x, int y, int frame );
static void draw_alien_brah( game *g, int x, int y, int frame );

void init_aliens( game *g, aliens *al )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        al->type[i] = ALIEN_NONE;
    }
}

void draw_aliens( game *g, aliens *al )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        switch( al->type[i] )
        {
            case ALIEN_BRAH:
                draw_alien_brah( g, al->x[i], al->y[i], al->img[i] % 8 );
                break;
            case ALIEN_POOLP:
                draw_alien_poolp( g, al->x[i], al->y[i], al->img[i] % 2 );
                break;
            case ALIEN_BOOL:
                draw_alien_bool( g, al->x[i], al->y[i], al->img[i] % 6 );
                break;
            case ALIEN_NONE:
                break;
        }
    }
}

void update_aliens( game *g, aliens *al )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        switch( al->type[i] )
        {
            case ALIEN_POOLP:
            case ALIEN_BOOL:
            case ALIEN_BRAH:
                al->x[i] = ((al->x[i] + 5) % (g->w + 3)) - 3;
                al->y[i] = al->y[i] + (rand() % 8) / 7 - (rand() % 8) / 7;
                al->img[i] = al->img[i] + 1;

                /* Check bounds */
                if( al->y[i] < 0 ) al->y[i] = 0;
                if( al->y[i] > g->w - 1 ) al->y[i] = g->w - 1;
                break;
            case ALIEN_NONE:
                break;
        }
    }
}

void add_alien( game *g, aliens *al, int x, int y, int type )
{
    int i;

    for( i = 0; i < ALIENS; i++ )
    {
        if( al->type[i] == ALIEN_NONE )
        {
            al->type[i] = type;
            al->x[i] = x;
            al->y[i] = y;
            al->img[i] = 0;

            switch( al->type[i] )
            {
                case ALIEN_POOLP:
                    al->life[i] = 3;
                    break;
                case ALIEN_BOOL:
                    al->life[i] = 3;
                    break;
                case ALIEN_BRAH:
                    al->life[i] = 3;
                    break;
                case ALIEN_NONE:
                    break;
            }

            break;
        }
    }
}

static void draw_alien_poolp( game *g, int x, int y, int frame )
{
    switch( frame )
    {
    case 0:
        gfx_color( MAGENTA );
        gfx_goto( x, y );
        gfx_putstr( ",---." );
        gfx_goto( x, y+1 );
        gfx_putchar( '\\' );
        gfx_color( WHITE );
        gfx_putstr( "o O" );
        gfx_color( MAGENTA );
        gfx_putchar( '/' );
        gfx_goto( x, y+2 );
        gfx_putstr( "^^^^^" );
        break;
    case 1:
        gfx_color( MAGENTA );
        gfx_goto( x, y );
        gfx_putstr( ",---." );
        gfx_goto( x, y+1 );
        gfx_putchar( '\\' );
        gfx_color( WHITE );
        gfx_putstr( "O o" );
        gfx_color( MAGENTA );
        gfx_putchar( '/' );
        gfx_goto( x, y+2 );
        gfx_putstr( "^^^^^" );
        break;
    }
}

static void draw_alien_bool( game *g, int x, int y, int frame )
{
    gfx_color( GREEN );
    gfx_goto( x, y-1 );
    gfx_putstr( "__" );

    gfx_goto( x-1, y );
    gfx_putchar( '/' );
    gfx_goto( x+2, y );
    gfx_putchar( '\\' );

    switch( frame )
    {
    case 3:
        gfx_goto( x-2, y+1 );
        gfx_putstr( "//'`\\\\" );
        break;
    case 4:
    case 2:
        gfx_goto( x-2, y+1 );
        gfx_putstr( "/(~~)\\" );
        break;
    case 5:
    case 1:
        gfx_goto( x-2, y+1 );
        gfx_putstr( "((^^))" );
        break;
    case 0:
        gfx_goto( x-1, y+1 );
        gfx_putstr( "\\\\//" );
        break;
    }

    gfx_color( WHITE );
    gfx_goto( x, y );
    gfx_putstr( "oo" );
}

static void draw_alien_brah( game *g, int x, int y, int frame )
{
    gfx_color( YELLOW );

    switch( frame )
    {
    case 0:
        gfx_goto( x, y );
        gfx_putchar( '.' );
        gfx_goto( x+6, y );
        gfx_putchar( ',' );
        gfx_goto( x+1, y+1 );
        gfx_putstr( "\\ X /" );
        break;
    case 7:
    case 1:
        gfx_goto( x-1, y );
        gfx_putchar( '.' );
        gfx_goto( x+7, y );
        gfx_putchar( ',' );
        gfx_goto( x, y+1 );
        gfx_putstr( "`- X -'" );
        break;
    case 6:
    case 2:
        gfx_goto( x-1, y+1 );
        gfx_putstr( "`-- X --'" );
        break;
    case 5:
    case 3:
        gfx_goto( x, y+1 );
        gfx_putstr( ",- X -." );
        gfx_goto( x-1, y+2 );
        gfx_putchar( '\'' );
        gfx_goto( x+7, y+2 );
        gfx_putchar( '`' );
        break;
    case 4:
        gfx_goto( x+1, y+1 );
        gfx_putstr( ", X ." );
        gfx_goto( x, y+2 );
        gfx_putchar( '/' );
        gfx_goto( x+6, y+2 );
        gfx_putchar( '\\' );
        break;
    }

    gfx_goto( x+2, y+2 );
    gfx_putstr( "`V'" );

    gfx_color( WHITE );
    gfx_goto( x+2, y+1 );
    gfx_putchar( 'o' );
    gfx_goto( x+4, y+1 );
    gfx_putchar( 'o' );
}


