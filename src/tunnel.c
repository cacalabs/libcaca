
#include <stdlib.h>

#include "common.h"

static void draw_wall( game *g, int *wall );

/* Init tunnel */
tunnel * create_tunnel( game *g, int w, int h )
{
    int i;
    tunnel *t = malloc(sizeof(tunnel));

    t->left = malloc(h*sizeof(int));
    t->right = malloc(h*sizeof(int));
    t->w = w;
    t->h = h;

    if( t->w >= g->w )
    {
        t->left[0] = -1;
        t->right[0] = g->w;
        for( i = 0; i < g->h; i++ )
        {
            t->left[i] = -1;
            t->right[i] = g->w;
        }
    }
    else
    {
        t->left[0] = (g->w - w) / 2;
        t->right[0] = (g->w + w) / 2;
        /* Yeah, sub-efficient, but less code to do :-) */
        for( i = 0; i < g->h; i++ )
        {
            update_tunnel( g, t );
        }
    }

    return t;
}

void free_tunnel( tunnel *t )
{
    free( t->left );
    free( t->right );
    free( t );
}

void draw_tunnel( game *g, tunnel *t )
{
    /* Print tunnel */
    draw_wall( g, t->left );
    draw_wall( g, t->right );
}

void update_tunnel( game *g, tunnel *t )
{
    static int const delta[] = { -2, -1, 1, 2 };
    int i,j,k;

    /* Slide tunnel one block vertically */
    for( i = t->h; i--; )
    {
        t->left[i+1] = t->left[i];
        t->right[i+1] = t->right[i];
    }

    /* Generate new values */
    i = delta[GET_RAND(0,4)];
    j = delta[GET_RAND(0,4)];

    /* Check in which direction we need to alter tunnel */
    if( t->right[1] - t->left[1] < t->w )
    {
        /* Not wide enough */
        if( i > j )
        {
            k = j; j = i; i = k;
        }
    }
    else if( t->right[1] - t->left[1] - 2 > t->w )
    {
        /* Too wide */
        if( i < j )
        {
            k = j; j = i; i = k;
        }
    }
    else
    {
        /* No need to mess with i and j: width is OK */
    }

    /* If width doesn't exceed game size, update coords */
    if( t->w <= g->w || t->right[1] - t->left[1] < t->w )
    {
        t->left[0] = t->left[1] + i;
        t->right[0] = t->right[1] + j;
    }
    else
    {
        t->left[0] = -1;
        t->right[0] = g->w;
    }

    if( t->w > g->w )
    {
        if( t->left[0] < 0 && t->right[0] < g->w - 2 )
        {
             t->left[0] = t->left[1] + 1;
        }

        if( t->left[0] > 1 && t->right[0] > g->w - 1 )
        {
             t->right[0] = t->right[1] - 1;
        }
    }
    else
    {
        if( t->left[0] < 0 )
        {
            t->left[0] = t->left[1] + 1;
        }

        if( t->right[0] > g->w - 1 )
        {
            t->right[0] = t->right[1] - 1;
        }
    }
}

static void draw_wall( game *g, int *wall )
{
    int i;

    gfx_color( RED );

    for( i = 0; i < g->h ; i++ )
    {
        char *str;

        if( wall[i] < 0 || wall[i] >= g->w )
        {
            continue;
        }

        if( wall[i] > wall[i+1] )
        {
            str = wall[i] > wall[i-1] ? ">##>" : "/##/";
        }
        else
        {
            str = wall[i] > wall[i-1] ? "\\##\\" : "<##<";
        }

        if( wall[i] == wall[i+1] + 2 )
        {
            gfx_goto( wall[i] - 1, i );
            gfx_putchar( '_' );
        }

        gfx_goto( wall[i], i );
        gfx_putstr( str );
        if( wall[i] == wall[i+1] - 2 ) gfx_putchar( '_' );
    }
}

