/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

/** \file sprite.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Sprite loading and blitting
 *
 *  This file contains a small framework for sprite loading and blitting.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caca.h"
#include "caca_internals.h"

struct caca_frame
{
    int w, h;
    int dx, dy;
    char *chars;
    int *color;
};

struct caca_sprite
{
    int nf;
    struct caca_frame *frames;
};

/**
 * \brief Allocate a sprite loaded from a file.
 *
 * \param file The filename.
 * \return The sprite, or NULL if an error occured.
 */
struct caca_sprite *caca_load_sprite(const char *file)
{
    char buf[BUFSIZ];
    struct caca_sprite *sprite;
    FILE *fd;

    fd = fopen(file, "r");
    if(fd == NULL)
        return NULL;

    sprite = malloc(sizeof(struct caca_sprite));
    if(sprite == NULL)
        goto sprite_alloc_failed;

    sprite->nf = 0;
    sprite->frames = NULL;

    while(!feof(fd))
    {
        int x, y;
        int w = 0, h = 0, dx = 0, dy = 0;
        struct caca_frame *frame;

        /* Get width and height */
        if(!fgets(buf, BUFSIZ, fd))
            break;

        sscanf(buf, "%i %i %i %i", &w, &h, &dx, &dy);
        if(w <= 0 || h <= 0 || w > BUFSIZ / 2)
            break;

        if(sprite->nf)
        {
            void *tmp = realloc(sprite->frames,
                                (sprite->nf + 1) * sizeof(struct caca_frame));
            if(tmp == NULL)
                goto frame_failed;
            sprite->frames = tmp;
            sprite->nf++;
        }
        else
        {
            sprite->frames = malloc((sprite->nf + 1) * sizeof(struct caca_frame));
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

/**
 * \brief Return the number of frames in a sprite.
 *
 * \param sprite The sprite.
 * \return The number of frames.
 */
int caca_get_sprite_frames(const struct caca_sprite *sprite)
{
    if(sprite == NULL)
        return 0;

    return sprite->nf;
}

/**
 * \brief Return the width of a sprite.
 *
 * \param sprite The sprite.
 * \param f The frame index.
 * \return The width of the given frame of the sprite.
 */
int caca_get_sprite_width(const struct caca_sprite *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].w;
}

/**
 * \brief Return the height of a sprite.
 *
 * \param sprite The sprite.
 * \param f The frame index.
 * \return The height of the given frame of the sprite.
 */
int caca_get_sprite_height(const struct caca_sprite *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].h;
}

/**
 * \brief Return the X coordinate of a sprite's handle.
 *
 * \param sprite The sprite.
 * \param f The frame index.
 * \return The X coordinate of the given frame's handle.
 */
int caca_get_sprite_dx(const struct caca_sprite *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].dx;
}

/**
 * \brief Return the Y coordinate of a sprite's handle.
 *
 * \param sprite The sprite.
 * \param f The frame index.
 * \return The Y coordinate of the given frame's handle.
 */
int caca_get_sprite_dy(const struct caca_sprite *sprite, int f)
{
    if(sprite == NULL)
        return 0;

    if(f < 0 || f >= sprite->nf)
        return 0;

    return sprite->frames[f].dy;
}

/**
 * \brief Draw a sprite's specific frame at the given coordinates. If the
 *        frame does not exist, nothing is displayed.
 *
 * \param x The X coordinate.
 * \param y The Y coordinate.
 * \param sprite The sprite.
 * \param f The frame index.
 * \return void
 */
void caca_draw_sprite(int x, int y, const struct caca_sprite *sprite, int f)
{
    int i, j;
    enum caca_color oldfg, oldbg;
    struct caca_frame *frame;

    if(sprite == NULL)
        return;

    if(f < 0 || f >= sprite->nf)
        return;

    frame = &sprite->frames[f];

    oldfg = caca_get_fg_color();
    oldbg = caca_get_bg_color();

    for(j = 0; j < frame->h; j++)
    {
        for(i = 0; i < frame->w; i++)
        {
            int col = frame->color[frame->w * j + i];
            if(col >= 0)
            {
                caca_set_color(col, CACA_COLOR_BLACK);
                caca_putchar(x + i - frame->dx, y + j - frame->dy,
                           frame->chars[frame->w * j + i]);
            }
        }
    }

    caca_set_color(oldfg, oldbg);
}

/**
 * \brief Free the memory associated with a sprite.
 *
 * \param sprite The sprite to be freed.
 * \return void
 */
void caca_free_sprite(struct caca_sprite *sprite)
{
    int i;

    if(sprite == NULL)
        return;

    for(i = sprite->nf; i--;)
    {
        struct caca_frame *frame = &sprite->frames[i];
        free(frame->chars);
        free(frame->color);
    }

    free(sprite->frames);
    free(sprite);
}

