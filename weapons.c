
#include <stdlib.h>

#include "common.h"

static void draw_nuke( int x, int y, int frame );
static void draw_circle( int x, int y, int r );

void init_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        wp->x[i] = -1;
        wp->y[i] = -1;
        wp->v[i] = 0;
        wp->n[i] = 0;
        wp->type[i] = 0;
    }
}

void draw_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        if( wp->x[i] >= 0 )
        {
            switch( wp->type[i] )
            {
                case 2:
                    draw_nuke( wp->x[i], wp->y[i], wp->n[i] );
                    break;
                case 1:
                default:
                    GFX_COLOR( WHITE );
                    GFX_GOTO( wp->x[i], wp->y[i] );
                    GFX_WRITE( '|' );
                    GFX_COLOR( CYAN );
                    GFX_GOTO( wp->x[i], wp->y[i] + 1 );
                    GFX_WRITE( '|' );
                    break;
            }
        }
    }
}

void update_weapons( game *g, weapons *wp )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        if( wp->y[i] < 0 )
        {
            wp->x[i] = -1;
            wp->y[i] = -1;
        }
        else
        {
            switch( wp->type[i] )
            {
            case 2:
                wp->n[i]--;
                if( wp->n[i]-- < 0 )
                {
                    wp->y[i] = -1;
                }
                break;
            case 1:
            default:
                wp->y[i] += wp->v[i];
                break;
            }

            /* Check collisions */
        }
    }
}

void add_weapon( game *g, weapons *wp, int x, int y, int type )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        if( wp->y[i] < 0 )
        {
            wp->x[i] = x;
            wp->y[i] = y;
            wp->type[i] = type;
            wp->v[i] = -2;
            wp->n[i] = 30;
            break;
        }
    }
}

static void draw_nuke( int x, int y, int frame )
{
    int r = (34 - frame) * (34 - frame) / 10;

    /* Lots of duplicate pixels, but we don't care */
    GFX_COLOR( BLUE );
    draw_circle( x, y, r++ );
    GFX_COLOR( CYAN );
    draw_circle( x, y, r++ );
    GFX_COLOR( WHITE );
    draw_circle( x, y, r++ );
    draw_circle( x, y, r++ );
}

static void draw_circle( int x, int y, int r )
{
    int test, dx, dy;

    /* Optimized Bresenham. Kick ass. */
    for( test = 0, dx = 0, dy = r ; dx <= dy ; dx++ )
    {
        GFX_WRITETO( x + dx, y + dy / 2, '#' );
        GFX_WRITETO( x - dx, y + dy / 2, '#' );
        GFX_WRITETO( x + dx, y - dy / 2, '#' );
        GFX_WRITETO( x - dx, y - dy / 2, '#' );

        GFX_WRITETO( x + dy, y + dx / 2, '#' );
        GFX_WRITETO( x - dy, y + dx / 2, '#' );
        GFX_WRITETO( x + dy, y - dx / 2, '#' );
        GFX_WRITETO( x - dy, y - dx / 2, '#' );

        test += test > 0 ? dx - dy-- : dx;
    }
}

