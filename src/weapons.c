
#include <stdlib.h>
#include <math.h>

#include "common.h"

static void draw_nuke( int x, int y, int frame );
static void draw_circle( int x, int y, int r, char c );

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
            case WEAPON_NUKE:
                draw_nuke( wp->x[i] >> 4, wp->y[i] >> 4, wp->n[i] );
                break;
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
                wp->vx[i] = (7 * wp->vx[i]
                              + (dx * 48) / sqrt(dx*dx+dy*dy) ) / 8;
                wp->vy[i] = (7 * wp->vy[i]
                              + (dy * 24) / sqrt(dx*dx+dy*dy) ) / 8;

                break;
            case WEAPON_NUKE:
                wp->n[i]--;
                if( wp->n[i] < 0 )
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;
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
            switch( type )
            {
                case WEAPON_LASER:
                    break;
                case WEAPON_SEEKER:
                    wp->x2[i] = x;
                    wp->y2[i] = y;
                    wp->x3[i] = x;
                    wp->y3[i] = y;
                    wp->n[i] = 10;
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

