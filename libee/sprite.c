/*
 *   libee         ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ee.h"

struct ee_frame
{
    int w, h;
    int dx, dy;
    char *chars;
    int *color;
};

struct ee_sprite
{
    int f;
    int nf;
    struct ee_frame *frames;
};

struct ee_sprite *ee_load_sprite(const char *file)
{
    char buf[BUFSIZ];
    struct ee_sprite *sprite;
    FILE *fd;

    fd = fopen(file, "r");
    if(fd == NULL)
        return NULL;

    sprite = malloc(sizeof(struct ee_sprite));
    sprite->f = 0;
    sprite->nf = 0;
    sprite->frames = NULL;

    while(!feof(fd))
    {
        int x, y;
        int w = 0, h = 0, dx = 0, dy = 0;
        struct ee_frame *frame;

        /* Get width and height */
        if(!fgets(buf, BUFSIZ, fd))
            break;

        sscanf(buf, "%i %i %i %i", &w, &h, &dx, &dy);
        if(w <= 0 || h <= 0 || w > BUFSIZ / 2)
            break;

        if(sprite->nf++)
            sprite->frames = realloc(sprite->frames,
                                     sprite->nf * sizeof(struct ee_frame));
        else
            sprite->frames = malloc(sprite->nf * sizeof(struct ee_frame));
        frame = &sprite->frames[sprite->nf - 1];

        frame->w = w;
        frame->h = h;
        frame->dx = dx;
        frame->dy = dy;
        frame->chars = malloc(w * h * sizeof(char));
        frame->color = malloc(w * h * sizeof(int));

        for(y = 0; y < h; y++)
        {
            if(!fgets(buf, BUFSIZ, fd))
                goto failed;

            for(x = 0; x < w && buf[x] && buf[x] != '\r' && buf[x] != '\n'; x++)
                frame->chars[w * y + x] = buf[x];

            for(; x < w; x++)
                frame->chars[w * y + x] = ' ';
        }

        for(y = 0; y < h; y++)
        {
            if(!fgets(buf, BUFSIZ, fd))
                goto failed;

            for(x = 0; x < w && buf[x] && buf[x] != '\r' && buf[x] != '\n'; x++)
                frame->color[w * y + x] = buf[x] - 'a';

            for(; x < w; x++)
                frame->color[w * y + x] = ' ' - 'a';
        }

        continue;

    failed:
        free(sprite->frames[sprite->nf - 1].chars);
        free(sprite->frames[sprite->nf - 1].color);
        sprite->nf--;
        break;
    }

    fclose(fd);

    if(sprite->nf == 0)
    {
        free(sprite);
        return NULL;
    }

    return sprite;
}

void ee_set_sprite_frame(struct ee_sprite *sprite, int f)
{
    if(sprite == NULL)
        return;

    if(f < 0 || f >= sprite->nf)
        return;

    sprite->f = f;
}

int ee_get_sprite_frame(struct ee_sprite *sprite)
{
    if(sprite == NULL)
        return -1;

    return sprite->f;
}

void ee_draw_sprite(int x, int y, struct ee_sprite *sprite)
{
    int i, j;
    struct ee_frame *frame;

    if(sprite == NULL)
        return;

    frame = &sprite->frames[sprite->f];

    for(j = 0; j < frame->h; j++)
    {
        for(i = 0; i < frame->w; i++)
        {
            int col = frame->color[frame->w * j + i];
            if(col >= 0)
            {
                ee_goto(x + i - frame->dx, y + j - frame->dy);
                ee_color(col);
                ee_putchar(frame->chars[frame->w * j + i]);
            }
        }
    }
}

void ee_free_sprite(struct ee_sprite *sprite)
{
    int i;

    if(sprite == NULL)
        return;

    for(i = sprite->nf; i--;)
    {
        struct ee_frame *frame = &sprite->frames[i];
        free(frame->chars);
        free(frame->color);
    }

    free(sprite->frames);
    free(sprite);
}

