
#include <stdlib.h>

#include "common.h"

void init_starfield( game *g, starfield *s )
{
    int i;

    for( i = 0; i < STARS; i++ )
    {
        s->x[i] = rand() % g->w;
        s->y[i] = rand() % g->h;
        s->z[i] = 1 + rand() % 3;
        s->ch[i] = (rand() % 2) ? '.' : '\'';
        s->c[i] = 6 + rand() % 2;
    }
}

void draw_starfield( game *g, starfield *s )
{
    int i;

    for( i = 0; i < STARS; i++ )
    {
        if( s->x[i] >= 0 )
        {
            GFX_COLOR( s->c[i] );
            GFX_GOTO( s->x[i], s->y[i] );
            GFX_WRITE( s->ch[i] );
        }
    }
}

void update_starfield( game *g, starfield *s )
{
    int i;

    for( i = 0; i < STARS; i++ )
    {
        if( s->x[i] < 0 )
        {
            s->x[i] = rand() % g->w;
            s->y[i] = 0;
            s->z[i] = 1 + rand() % 2;
            s->ch[i] = (rand() % 2) ? '.' : '\'';
            s->c[i] = 6 + rand() % 2;
        }
        else if( s->y[i] < g->h-1 )
        {
            s->y[i] += s->z[i];
        }
        else
        {
            s->x[i] = -1;
        }
    }
}

