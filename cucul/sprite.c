/*
 *  libcucul      Canvas for ultrafast compositing of Unicode letters
 *  Copyright (c) 2002-2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  This file contains a small framework for sprite loading and blitting.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#endif

#include "cucul.h"
#include "cucul_internals.h"

#if !defined(_DOXYGEN_SKIP_ME)
struct cucul_frame
{
    int w, h;
    int dx, dy;
    char *chars;
    int *color;
};

struct cucul_sprite
{
    int nf;
    struct cucul_frame *frames;
};
#endif

/** \brief Allocate a sprite loaded from a file.
 *
 *  \param file The filename.
 *  \return The sprite, or NULL if an error occured.
 */
struct cucul_sprite *cucul_load_sprite(char const *file)
{
    char buf[BUFSIZ];
    struct cucul_sprite *sprite;
    FILE *fd;

    fd = fopen(file, "r");
    if(fd == NULL)
        return NULL;

    sprite = malloc(sizeof(struct cucul_sprite));
    if(sprite == NULL)
        goto sprite_alloc_failed;

    sprite->nf = 0;
    sprite->frames = NULL;

    while(!feof(fd))
    {
        int x, y;
        int w = 0, h = 0, dx = 0, dy = 0;
        struct cucul_frame *frame;

        /* Get width and height */
        if(!fgets(buf, BUFSIZ, fd))
            break;

        sscanf(buf, "%i %i %i %i", &w, &h, &dx, &dy);
        if(w <= 0 || h <= 0 || w > BUFSIZ / 2)
            break;

        if(sprite->nf)
        {
            void *tmp = realloc(sprite->frames,
                                (sprite->nf + 1) * sizeof(struct cucul_frame));
            if(tmp == NULL)
                goto frame_failed;
            sprite->frames = tmp;
            sprite->nf++;
        }
        else
        {
            sprite->frames = malloc((sprite->nf + 1) * sizeof(struct cucul_frame));
            if(sprite->frames == NULL)
                goto sprite_failed;
            sprite->nf++;
        }

        frame = &sprite->frames[sprite->nf - 1];

        frame->w = w;
        frame->h = h;
        frame->dx = dx;
        frame->dy = dy;
        frame->chars = malloc(w * h * sizeof(char));
        if(frame->chars == NULL)
        {
            sprite->nf--;
            goto frame_failed;
        }
        frame->color = malloc(w * h * sizeof(int));
        if(frame->color == NULL)
        {
            free(frame->chars);
            sprite->nf--;
            goto frame_failed;
        }

        for(y = 0; y < h; y++)
        {
            if(!fgets(buf, BUFSIZ, fd))
                goto frame_failed;

            for(x = 0; x < w && buf[x] && buf[x] != '\r' && buf[x] != '\n'; x++)
                frame->chars[w * y + x] = buf[x];

            for(; x < w; x++)
                frame->chars[w * y + x] = ' ';
        }

        for(y = 0; y < h; y++)
        {
            if(!fgets(buf, BUFSIZ, fd))
                goto frame_failed;

            for(x = 0; x < w && buf[x] && buf[x] != '\r' && buf[x] != '\n'; x++)
                frame->color[w * y + x] = buf[x] - 'a';

            for(; x < w; x++)
                frame->color[w * y + x] = ' ' - 'a';
        }

        continue;
    }

    if(sprite->nf == 0)
        goto sprite_failed;

    fclose(fd);
    return sprite;

frame_failed:
    while(sprite->nf)
    {
        free(sprite->frames[sprite->nf - 1].color);
        free(sprite->frames[sprite->nf - 1].chars);
        sprite->nf--;
    }
sprite_failed:
    free(sprite);
sprite_alloc_failed:
    fclose(fd);
    return NULL;
}

/** \brief Return the number of frames in a sprite.
 *
 *  \param sprite The sprite.
 *  \return The number of frames.
 */
int cucul_get_sprite_frames(struct cucul_sprite const *sprite)
{
    if(sprite == NULL)
        return 0;

    return sprite->nf;
}

/** \brief Return the width of a sprite.
 *
 *  \param sprite The sprite.
 *  \param f The frame index.
 *  \return The width of the given frame of the sprite.
 */
int cucul_get_sprite_width(struct cucul_sprite const *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].w;
}

/** \brief Return the height of a sprite.
 *
 *  \param sprite The sprite.
 *  \param f The frame index.
 *  \return The height of the given frame of the sprite.
 */
int cucul_get_sprite_height(struct cucul_sprite const *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].h;
}

/** \brief Return the X coordinate of a sprite's handle.
 *
 *  \param sprite The sprite.
 *  \param f The frame index.
 *  \return The X coordinate of the given frame's handle.
 */
int cucul_get_sprite_dx(struct cucul_sprite const *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].dx;
}

/** \brief Return the Y coordinate of a sprite's handle.
 *
 *  \param sprite The sprite.
 *  \param f The frame index.
 *  \return The Y coordinate of the given frame's handle.
 */
int cucul_get_sprite_dy(struct cucul_sprite const *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].dy;
}

/** \brief Draw a sprite's specific frame at the given coordinates. If the
 *         frame does not exist, nothing is displayed.
 *
 *  \param qq A libcucul canvas
 *  \param x The X coordinate.
 *  \param y The Y coordinate.
 *  \param sprite The sprite.
 *  \param f The frame index.
 *  \return void
 */
void cucul_draw_sprite(cucul_t *qq, int x, int y, struct cucul_sprite const *sprite, int f)
{
    int i, j;
    unsigned int oldfg, oldbg;
    struct cucul_frame *frame;

    if(sprite == NULL)
        return;

    if(f < 0 || f >= sprite->nf)
        return;

    frame = &sprite->frames[f];

    oldfg = qq->fgcolor;
    oldbg = qq->bgcolor;

    for(j = 0; j < frame->h; j++)
    {
        for(i = 0; i < frame->w; i++)
        {
            int col = frame->color[frame->w * j + i];
            if(col >= 0)
            {
                cucul_set_color(qq, col, CUCUL_COLOR_BLACK);
                cucul_putchar(qq, x + i - frame->dx, y + j - frame->dy,
                              frame->chars[frame->w * j + i]);
            }
        }
    }

    cucul_set_color(qq, oldfg, oldbg);
}

/** \brief Free the memory associated with a sprite.
 *
 *  \param sprite The sprite to be freed.
 *  \return void
 */
void cucul_free_sprite(struct cucul_sprite *sprite)
{
    int i;

    if(sprite == NULL)
        return;

    for(i = sprite->nf; i--;)
    {
        struct cucul_frame *frame = &sprite->frames[i];
        free(frame->chars);
        free(frame->color);
    }

    free(sprite->frames);
    free(sprite);
}

