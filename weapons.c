
#include <stdlib.h>

#include "common.h"

void init_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < SHOTS; i++ )
    {
        wp->x[i] = -1;
        wp->y[i] = -1;
        wp->v[i] = 0;
    }
}

void draw_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < SHOTS; i++ )
    {
        if( wp->x[i] >= 0 )
        {
            GFX_COLOR( WHITE );
            GFX_GOTO( wp->x[i], wp->y[i] );
            GFX_WRITE( '|' );
            GFX_COLOR( CYAN );
            GFX_GOTO( wp->x[i], wp->y[i] + 1 );
            GFX_WRITE( '|' );
        }
    }
}

void update_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < SHOTS; i++ )
    {
        if( wp->y[i] < 0 )
        {
            wp->x[i] = -1;
            wp->y[i] = -1;
        }
        else
        {
            wp->y[i] += wp->v[i];

            /* Check collisions */
        }
    }
}

void add_weapon( game *g, weapons *wp, int x, int y )
{
    int i;

    for( i = 0; i < SHOTS; i++ )
    {
        if( wp->y[i] < 0 )
        {
            wp->x[i] = x;
            wp->y[i] = y;
            wp->v[i] = -2;
            break;
        }
    }
}

