/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
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

#include <stdlib.h>

#include "common.h"

static void draw_bomb(int x, int y, int vx, int vy);
static void draw_nuke(int x, int y, int frame);
static void draw_beam(int x, int y, int frame);
static void draw_fragbomb(int x, int y, int frame);

struct ee_sprite *bomb_sprite;
struct ee_sprite *fragbomb_sprite;

void init_weapons(game *g, weapons *wp)
{
    int i;

    for(i = 0; i < WEAPONS; i++)
    {
        wp->type[i] = WEAPON_NONE;
    }

    bomb_sprite = ee_load_sprite("data/weapon_bomb");
    fragbomb_sprite = ee_load_sprite("data/weapon_fragbomb");
}

void draw_weapons(game *g, weapons *wp)
{
    int i;

    for(i = 0; i < WEAPONS; i++)
    {
        switch(wp->type[i])
        {
            case WEAPON_LASER:
                ee_color(EE_WHITE);
                ee_putchar(wp->x[i] >> 4, wp->y[i] >> 4, '|');
                ee_color(EE_CYAN);
                ee_putchar(wp->x[i] >> 4, (wp->y[i] >> 4) + 1, '|');
                break;
            case WEAPON_SEEKER:
                ee_color(EE_CYAN);
                ee_putchar(wp->x3[i] >> 4, wp->y3[i] >> 4, '.');
                ee_putchar(wp->x2[i] >> 4, wp->y2[i] >> 4, 'o');
                ee_color(EE_WHITE);
                ee_putchar(wp->x[i] >> 4, wp->y[i] >> 4, '@');
                break;
            case WEAPON_BOMB:
                ee_color(EE_GRAY);
                ee_putchar((wp->x[i] - wp->vx[i]) >> 4, (wp->y[i] - wp->vy[i]) >> 4, '.');
                ee_putchar((wp->x3[i] - wp->vx[i]) >> 4, (wp->y3[i] - wp->vy[i]) >> 4, '.');
                ee_putchar((wp->x2[i] - wp->vx[i]) >> 4, (wp->y2[i] - wp->vy[i]) >> 4, '.');
                ee_putchar(wp->x3[i] >> 4, wp->y3[i] >> 4, '.');
                ee_putchar(wp->x2[i] >> 4, wp->y2[i] >> 4, '.');
                draw_bomb(wp->x[i] >> 4, wp->y[i] >> 4, wp->vx[i], wp->vy[i]);
                break;
            case WEAPON_FRAGBOMB:
                draw_fragbomb(wp->x[i] >> 4, wp->y[i] >> 4, wp->n[i]);
                break;
            case WEAPON_BEAM:
                draw_beam(wp->x[i] >> 4, wp->y[i] >> 4, wp->n[i]);
                break;
            case WEAPON_NUKE:
                draw_nuke(wp->x[i] >> 4, wp->y[i] >> 4, wp->n[i]);
                break;
            case WEAPON_LIGHTNING:
            case WEAPON_NONE:
                break;
        }
    }
}

void update_weapons(game *g, weapons *wp)
{
    int i, j, dist, xmin, ymin, dx, dy, xnew, ynew;

    for(i = 0; i < WEAPONS; i++)
    {
        switch(wp->type[i])
        {
            case WEAPON_LASER:
                wp->x[i] += wp->vx[i];
                wp->y[i] += wp->vy[i];
                if(wp->y[i] < 0)
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_BOMB:
            case WEAPON_SEEKER:
                /* Update tail */
                wp->x3[i] = wp->x2[i];
                wp->y3[i] = wp->y2[i];

                wp->x2[i] = wp->x[i];
                wp->y2[i] = wp->y[i];

                wp->x[i] += wp->vx[i];
                wp->y[i] += wp->vy[i];

                if(wp->y[i] < 0)
                {
                    wp->type[i] = WEAPON_NONE;
                    break;
                }

                if(wp->n[i] < 0)
                {
                    /* Stop updating direction */
                    break;
                }

                wp->n[i]--;

                /* Estimate our position next frames */
                xnew = wp->x[i] + wp->vx[i];
                ynew = wp->y[i] + wp->vy[i];

                xmin = xnew;
                ymin = - (g->h << 4);
                dist = (xnew - xmin) * (xnew - xmin)
                        + 4 * (ynew - ymin) * (ynew - ymin);

                /* Find the nearest alien */
                for(j = 0; j < ALIENS; j++)
                {
                    if(g->al->type[j] != ALIEN_NONE)
                    {
                        int alx = g->al->x[j] << 4;
                        int aly = g->al->y[j] << 4;
                        int new = (xnew - alx) * (xnew - alx)
                                   + 4 * (ynew - aly) * (ynew - aly);
                        if(new <= dist)
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

                /* Normalize direction */
                if(dx | dy)
                {
                    int norm = ee_sqrt(dx * dx + 4 * dy * dy);
                    dx = dx * 32 / norm;
                    dy = dy * 32 / norm;
                }

                /* Find our new speed */
                dx = (dx + 3 * wp->vx[i]) / 4;
                dy = (dy + 3 * wp->vy[i]) / 4;

                /* Normalize speed */
                if(dx | dy)
                {
                    int norm = ee_sqrt(dx * dx + 4 * dy * dy);
                    wp->vx[i] = dx * 32 / norm;
                    wp->vy[i] = dy * 32 / norm;
                }

                break;

            case WEAPON_FRAGBOMB:
                /* If n was set to -1, the fragbomb just exploded */
                if(wp->n[i] == -1)
                {
                    int coords[] =
                    {
                        32,  0,   -32,  0,    0,  16,     0, -16,
                        28,  8,   -28,  8,   28,  -8,   -28,  -8,
                        24, 12,   -24, 12,   24, -12,   -24, -12,
                        16, 14,   -16, 14,   16, -14,   -16, -14
                    };

                    for(j = 0 ; j < sizeof(coords) / sizeof(int) ; j += 2)
                    {
                        add_weapon(g, g->wp, wp->x[i] + coords[j], wp->y[i] + coords[j+1] / 2, coords[j], coords[j+1], WEAPON_SEEKER);
                        add_weapon(g, g->wp, wp->x[i] + coords[j] / 2, wp->y[i] + coords[j+1], coords[j], coords[j+1], WEAPON_SEEKER);
                    }

                    wp->type[i] = WEAPON_NONE;
                }

                wp->x[i] += wp->vx[i];
                wp->y[i] += wp->vy[i];
                wp->n[i]++;
                if(wp->y[i] < 0)
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;

            case WEAPON_BEAM:
                wp->x[i] = (g->p->x + 2) << 4;
                wp->y[i] = g->p->y << 4;
                wp->n[i]--;
                if(wp->n[i] < 0)
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_NUKE:
                wp->n[i]--;
                if(wp->n[i] < 0)
                {
                    wp->type[i] = WEAPON_NONE;
                }
                break;
            case WEAPON_LIGHTNING:
            case WEAPON_NONE:
                break;
        }
    }
}

void add_weapon(game *g, weapons *wp, int x, int y, int vx, int vy, int type)
{
    int i;

    for(i = 0; i < WEAPONS; i++)
    {
        if(wp->type[i] == WEAPON_NONE)
        {
            wp->x[i] = x;
            wp->y[i] = y;
            wp->vx[i] = vx;
            wp->vy[i] = vy;
            wp->type[i] = type;
            wp->n[i] = 0;
            switch(type)
            {
                case WEAPON_LASER:
                    break;
                case WEAPON_FRAGBOMB:
                    break;
                case WEAPON_SEEKER:
                case WEAPON_BOMB:
                    wp->x2[i] = x;
                    wp->y2[i] = y;
                    wp->x3[i] = x;
                    wp->y3[i] = y;
                    wp->n[i] = 20;
                    break;
                case WEAPON_BEAM:
                    wp->n[i] = 25;
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

static void draw_bomb(int x, int y, int vx, int vy)
{
    int frame;

    /* We have 1x2 pixels */
    vy *= 2;

    if(vx > vy)
    {
        if(vx > -vy) /* right quarter */
        {
            if(vy > vx/4)
                frame = 0; /* -pi/6 */
            else if(vy < -vx/4)
                frame = 1; /* pi/6 */
            else
                frame = 2; /* 0pi/6 */
        }
        else /* top quarter */
        {
            if(vx > -vy/4)
                frame = 3; /* 2pi/6 */
            else if(vx < vy/4)
                frame = 4; /* 4pi/6 */
            else
                frame = 5; /* 3pi/6 */
        }
    }
    else
    {
        if(vx > -vy) /* bottom quarter */
        {
            if(vx > vy/4)
                frame = 6; /* -2pi/6 */
            else if(vx < -vy/4)
                frame = 7; /* -4pi/6 */
            else
                frame = 8; /* -3pi/6 */
        }
        else /* left quarter */
        {
            if(vy > -vx/4)
                frame = 9; /* -5pi/6 */
            else if(vy < vx/4)
                frame = 10; /* 5pi/6 */
            else
                frame = 11; /* 6pi/6 */
        }
    }

    ee_set_sprite_frame(bomb_sprite, frame);
    ee_draw_sprite(x, y, bomb_sprite);
}

static void draw_fragbomb(int x, int y, int frame)
{
    /* Draw the head */
    ee_set_sprite_frame(fragbomb_sprite, frame & 1);
    ee_draw_sprite(x, y, fragbomb_sprite);

    /* Draw the tail */
    ee_set_sprite_frame(fragbomb_sprite, 2 + (frame % 4));
    ee_draw_sprite(x, y, fragbomb_sprite);
}

static void draw_beam(int x, int y, int frame)
{
    int r = (29 - frame) * (29 - frame) / 8;
    int i;

    switch(frame)
    {
        case 24:
            ee_color(EE_WHITE);
            ee_putstr(x, y-3, "__");
            ee_putchar(x-1, y-2, '\'');
            ee_putchar(x+2, y-2, '`');
            break;
        case 23:
            ee_color(EE_CYAN);
            ee_putstr(x, y-3, "__");
            ee_color(EE_WHITE);
            ee_putstr(x-2, y-2, "-'");
            ee_putstr(x+2, y-2, "`-");
            break;
        case 22:
            ee_color(EE_CYAN);
            ee_putstr(x, y-3, "__");
            ee_putchar(x-1, y-2, '\'');
            ee_putchar(x+2, y-2, '`');
            ee_color(EE_WHITE);
            ee_putstr(x-3, y-2, ",-");
            ee_putstr(x+3, y-2, "-.");
            break;
        case 21:
            ee_color(EE_CYAN);
            ee_putstr(x-1, y-3, "____");
            ee_putchar(x-2, y-2, '\'');
            ee_putchar(x+3, y-2, '`');
            ee_color(EE_WHITE);
            ee_putstr(x-4, y-2, ",-");
            ee_putstr(x+4, y-2, "-.");
            break;
        case 20:
            ee_color(EE_WHITE);
            ee_putstr(x, y-3, "%%");
            ee_putchar(x-4, y-2, ',');
            ee_putchar(x+5, y-2, '.');
            ee_color(EE_CYAN);
            ee_putchar(x-1, y-3, ':');
            ee_putchar(x+2, y-3, ':');
            ee_putstr(x-3, y-2, "-'");
            ee_putstr(x+3, y-2, "`-");
            break;
        case 19:
            ee_color(EE_WHITE);
            ee_putstr(x, y-4, "%%");
            ee_putstr(x, y-3, "##");
            ee_color(EE_CYAN);
            ee_putchar(x-1, y-4, ':');
            ee_putchar(x+2, y-4, ':');
            ee_putchar(x-1, y-3, '%');
            ee_putchar(x+2, y-3, '%');
            ee_putstr(x-4, y-2, ",-'");
            ee_putstr(x+3, y-2, "`-.");
            ee_color(EE_BLUE);
            ee_putchar(x-2, y-3, ':');
            ee_putchar(x+3, y-3, ':');
            break;
        case 18:
        default:
            r = (18 - frame) * (18 - frame);
            ee_color(EE_WHITE);
            ee_putstr(x-1, y-5-r, ":%%:");
            ee_putstr(x-1, y-4-r, "%##%");
            ee_color(EE_CYAN);
            ee_putchar(x-2, y-4-r, ':');
            ee_putchar(x+3, y-4-r, ':');
            ee_putchar(x-2, y-2, '\'');
            ee_putchar(x+3, y-2, '`');
            ee_color(EE_BLUE);
            ee_putchar(x-3, y-2, ':');
            ee_putchar(x+4, y-2, ':');
            for(i = 0; i <= r; i++)
            {
                ee_color(EE_WHITE);
                ee_putstr(x-1, y-3-i, ((i+frame) % 5) ? "####" : "%%%%");
                ee_color(EE_CYAN);
                ee_putchar(x-2, y-3-i, '%');
                ee_putchar(x+3, y-3-i, '%');
                ee_color(EE_BLUE);
                ee_putchar(x-3, y-3-i, ':');
                ee_putchar(x+4, y-3-i, ':');
            }
            break;
    }
}

static void draw_nuke(int x, int y, int frame)
{
    int r = (29 - frame) * (29 - frame) / 8;

    /* Lots of duplicate pixels, but we don't care */
    ee_color(EE_BLUE);
    ee_draw_circle(x, y, r++, ':');
    ee_color(EE_CYAN);
    ee_draw_circle(x, y, r++, '%');
    ee_color(EE_WHITE);
    ee_draw_circle(x, y, r++, '#');
    ee_draw_circle(x, y, r++, '#');
}

