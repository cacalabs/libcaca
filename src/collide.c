
#include <stdlib.h>

#include "common.h"

void collide_weapons_tunnel( game *g, weapons *wp, tunnel *t, explosions *ex )
{
    int i;

    for( i = 0; i < WEAPONS; i++ )
    {
        if( wp->y[i] >= 0 )
        {
            if( wp->x[i] <= t->left[wp->y[i]+1]
                 || wp->x[i] >= t->right[wp->y[i]+1] )
            {
                add_explosion( g, ex, wp->x[i], wp->y[i]+1, 0, 1, 0 );

                if( wp->x[i] <= t->left[wp->y[i]+1] )
                {
                    t->left[wp->y[i]]--;
                    t->left[wp->y[i]+1]-=2;
                    t->left[wp->y[i]+2]--;
                }
                else
                {
                    t->right[wp->y[i]]++;
                    t->right[wp->y[i]+1]+=2;
                    t->right[wp->y[i]+2]++;
                }

                wp->y[i] = -1;
            }
            else if( wp->x[i] <= t->left[wp->y[i]]
                      || wp->x[i] >= t->right[wp->y[i]] )
            {
                add_explosion( g, ex, wp->x[i], wp->y[i], 0, 1, 0 );

                if( wp->x[i] <= t->left[wp->y[i]] )
                {
                    t->left[wp->y[i]-1]--;
                    t->left[wp->y[i]]-=2;
                    t->left[wp->y[i]+1]--;
                }
                else
                {
                    t->right[wp->y[i]-1]++;
                    t->right[wp->y[i]]+=2;
                    t->right[wp->y[i]+1]++;
                }

                wp->y[i] = -1;
            }
        }
    }
}

void collide_weapons_aliens( game *g, weapons *wp, aliens *al, explosions *ex )
{
    int i, j;

    for( i = 0; i < WEAPONS; i++ )
    {
        if( wp->y[i] >= 0 )
        {
            int ok = 0;
            int r;

            switch( wp->type[i] )
            {
            case 2:
                /* Big nuke */
                r = (34 - wp->n[i]) * (34 - wp->n[i]) / 10;

                for( j = 0; j < ALIENS; j++ )
                {
                    if( al->x[j] < 0 )
                    {
                        continue;
                    }

                    if( (al->x[j] - wp->x[i]) * (al->x[j] - wp->x[i])
                          + 4 * (al->y[j] - wp->y[i]) * (al->y[j] - wp->y[i])
                        <= r * r )
                    {
                        /* Kill alien, not nuke */
                        add_explosion( g, ex, al->x[j], al->y[j], 0, 0, 1 );
                        al->x[j] = -1;
                        al->y[j] = -1;
                    }
                }
                break;

            case 1:
            default:
                for( j = 0; j < ALIENS; j++ )
                {
                    if( wp->x[i] >= al->x[j]
                         && wp->x[i] <= al->x[j] + 4
                         && wp->y[i] >= al->y[j]
                         && wp->y[i] <= al->y[j] + 2 )
                    {
                        al->life[j]--;
                        if( al->life[j] == 0 )
                        {
                            al->x[j] = -1;
                            al->y[j] = -1;
                            add_explosion( g, ex, wp->x[i], wp->y[i], 0, 0, 1 );
                        }
                        ok = 1;
                    }
                    else if( wp->x[i] >= al->x[j]
                              && wp->x[i] <= al->x[j] + 4
                              && wp->y[i]+1 >= al->y[j]
                              && wp->y[i]+1 <= al->y[j] + 2 )
                    {
                        al->life[j]--;
                        if( al->life[j] == 0 )
                        {
                            al->x[j] = -1;
                            al->y[j] = -1;
                            add_explosion( g, ex, wp->x[i], wp->y[i]+1, 0, 0, 1 );
                        }
                        ok = 1;
                    }
                }

                if( ok )
                {
                    wp->y[i] = -1;
                }
                break;
            }
        }
    }
}

void collide_player_tunnel( game *g, player *p, tunnel *t, explosions *ex )
{
    if( p->x <= t->left[p->y] )
    {
        p->x += 3;
        //add_explosion( g, ex, x+1, y-2, 0, 1, 0 );
    }
    else if( p->x + 5 >= t->right[p->y] )
    {
        p->x -= 3;
        //add_explosion( g, ex, x+4, y-2, 0, 1, 0 );
    }
}

