
#include <stdlib.h>

#include "common.h"

/* Init tunnel */
player * create_player( game *g )
{
    player *p = malloc(sizeof(player));

    p->x = g->w / 2;
    p->y = g->h - 3;
    p->dir = 0;
    p->weapon = 0;
    p->nuke = 0;

    return p;
}

void free_player( player *p )
{
    free( p );
}

void draw_player( game *g, player *p )
{
    GFX_GOTO( p->x + 2, p->y - 2 );
    GFX_COLOR( GREEN );
    GFX_WRITE( '/' );
    GFX_WRITE( '\\' );
    GFX_GOTO( p->x + 1, p->y - 1 );
    GFX_WRITE( '(' );
    GFX_COLOR( YELLOW );
    GFX_WRITE( '(' );
    GFX_WRITE( ')' );
    GFX_COLOR( GREEN );
    GFX_WRITE( ')' );
    GFX_GOTO( p->x, p->y );
    GFX_COLOR( GREEN );
    GFX_WRITE( 'I' );
    GFX_WRITE( '<' );
    GFX_WRITE( '_' );
    GFX_WRITE( '_' );
    GFX_WRITE( '>' );
    GFX_WRITE( 'I' );
}

void update_player( game *g, player *p )
{
    if( p->weapon )
    {
        p->weapon--;
    }

    if( p->nuke )
    {
        p->nuke--;
    }

    if( p->dir < 0 )
    {
        if( p->dir == -3 && p->x > -2 ) p->x -= 1;
        else if( p->x > -1 ) p->x -= 1;

        p->dir++;
    }
    else if( p->dir > 0 )
    {
        if( p->dir == 3 && p->x < g->w - 8 ) p->x += 1;
        else if( p->x < g->w - 7 ) p->x += 1;
        p->dir--;
    }
}

