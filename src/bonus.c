
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
                gfx_color( (bo->n[i]/2 % 3) ? GREEN : WHITE );
                gfx_goto( bo->x[i]+1, bo->y[i]-1 );
                gfx_putchar( '_' );
                gfx_goto( bo->x[i], bo->y[i] );
                gfx_putstr( "/ \\" );
                gfx_goto( bo->x[i], bo->y[i]+1 );
                gfx_putstr( "\\_/" );
                gfx_color( WHITE );
                gfx_goto( bo->x[i]+1, bo->y[i] );
                gfx_putchar( 'g' );
                break;
            case BONUS_LIFE:
                gfx_color( (bo->n[i] % 3) ? RED : WHITE );
                gfx_goto( bo->x[i]+1, bo->y[i]-1 );
                gfx_putchar( '_' );
                gfx_goto( bo->x[i]+3, bo->y[i]-1 );
                gfx_putchar( '_' );
                gfx_goto( bo->x[i], bo->y[i] );
                gfx_putstr( "( ' )" );
                gfx_goto( bo->x[i]+1, bo->y[i]+1 );
                gfx_putstr( "`v'" );
                gfx_color( WHITE );
                gfx_goto( bo->x[i]+3, bo->y[i] );
                gfx_putchar( '^' );
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

