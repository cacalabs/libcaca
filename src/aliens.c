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

#include <stdlib.h>

#include "common.h"

static void draw_alien_foo(game *, int, int, int);
static void draw_alien_bar(game *, int, int, int);
static void draw_alien_baz(game *, int, int, int);

void init_aliens(game *g, aliens *al)
{
    int i;

    for(i = 0; i < ALIENS; i++)
    {
        al->type[i] = ALIEN_NONE;
    }
}

void draw_aliens(game *g, aliens *al)
{
    int i;

    for(i = 0; i < ALIENS; i++)
    {
        switch(al->type[i])
        {
            case ALIEN_FOO:
                draw_alien_foo(g, al->x[i], al->y[i], al->img[i] % 8);
                break;
            case ALIEN_BAR:
                draw_alien_bar(g, al->x[i], al->y[i], al->img[i] % 2);
                break;
            case ALIEN_BAZ:
                draw_alien_baz(g, al->x[i], al->y[i], al->img[i] % 6);
                break;
            case ALIEN_NONE:
                break;
        }
    }
}

void update_aliens(game *g, aliens *al)
{
    int i;

    for(i = 0; i < ALIENS; i++)
    {
        /* If alien died, make it explode */
        if(al->type[i] != ALIEN_NONE && al->life[i] < 0)
        {
            add_explosion(g, g->ex, al->x[i], al->y[i], 0, 0, EXPLOSION_MEDIUM);
            al->type[i] = ALIEN_NONE;
            add_bonus(g, g->bo, al->x[i], al->y[i], GET_RAND(0,5) ? BONUS_GREEN : BONUS_LIFE);
        }

        /* Update coordinates */
        switch(al->type[i])
        {
            case ALIEN_FOO:
            case ALIEN_BAR:
            case ALIEN_BAZ:
                al->x[i] = ((al->x[i] + 5) % (g->w + 3)) - 3;
                al->y[i] = al->y[i] + (rand() % 8) / 7 - (rand() % 8) / 7;
                al->img[i] = al->img[i] + 1;

                /* Check bounds */
                if(al->y[i] < 0 ) al->y[i] = 0;
                if(al->y[i] > g->w - 1 ) al->y[i] = g->w - 1;
                break;
            case ALIEN_NONE:
                break;
        }
    }
}

void add_alien(game *g, aliens *al, int x, int y, int type)
{
    int i;

    for(i = 0; i < ALIENS; i++)
    {
        if(al->type[i] == ALIEN_NONE)
        {
            al->type[i] = type;
            al->x[i] = x;
            al->y[i] = y;
            al->img[i] = 0;

            switch(al->type[i])
            {
                case ALIEN_FOO:
                    al->life[i] = 3;
                    break;
                case ALIEN_BAR:
                    al->life[i] = 3;
                    break;
                case ALIEN_BAZ:
                    al->life[i] = 3;
                    break;
                case ALIEN_NONE:
                    break;
            }

            break;
        }
    }
}

static void draw_alien_bar(game *g, int x, int y, int frame)
{
    switch(frame)
    {
    case 0:
        ee_color(EE_MAGENTA);
        ee_goto(x, y);
        ee_putstr(",---.");
        ee_goto(x, y+1);
        ee_putchar('\\');
        ee_color(EE_WHITE);
        ee_putstr("o O");
        ee_color(EE_MAGENTA);
        ee_putchar('/');
        ee_goto(x, y+2);
        ee_putstr("^^^^^");
        break;
    case 1:
        ee_color(EE_MAGENTA);
        ee_goto(x, y);
        ee_putstr(",---.");
        ee_goto(x, y+1);
        ee_putchar('\\');
        ee_color(EE_WHITE);
        ee_putstr("O o");
        ee_color(EE_MAGENTA);
        ee_putchar('/');
        ee_goto(x, y+2);
        ee_putstr("^^^^^");
        break;
    }
}

static void draw_alien_baz(game *g, int x, int y, int frame)
{
    ee_color(EE_GREEN);
    ee_goto(x, y-1);
    ee_putstr("__");

    ee_goto(x-1, y);
    ee_putchar('/');
    ee_goto(x+2, y);
    ee_putchar('\\');

    switch(frame)
    {
    case 3:
        ee_goto(x-2, y+1);
        ee_putstr("//'`\\\\");
        break;
    case 4:
    case 2:
        ee_goto(x-2, y+1);
        ee_putstr("/(~~)\\");
        break;
    case 5:
    case 1:
        ee_goto(x-2, y+1);
        ee_putstr("((^^))");
        break;
    case 0:
        ee_goto(x-1, y+1);
        ee_putstr("\\\\//");
        break;
    }

    ee_color(EE_WHITE);
    ee_goto(x, y);
    ee_putstr("oo");
}

static void draw_alien_foo(game *g, int x, int y, int frame)
{
    ee_color(EE_YELLOW);

    switch(frame)
    {
    case 0:
        ee_goto(x, y);
        ee_putchar('.');
        ee_goto(x+6, y);
        ee_putchar(',');
        ee_goto(x+1, y+1);
        ee_putstr("\\ X /");
        break;
    case 7:
    case 1:
        ee_goto(x-1, y);
        ee_putchar('.');
        ee_goto(x+7, y);
        ee_putchar(',');
        ee_goto(x, y+1);
        ee_putstr("`- X -'");
        break;
    case 6:
    case 2:
        ee_goto(x-1, y+1);
        ee_putstr("`-- X --'");
        break;
    case 5:
    case 3:
        ee_goto(x, y+1);
        ee_putstr(",- X -.");
        ee_goto(x-1, y+2);
        ee_putchar('\'');
        ee_goto(x+7, y+2);
        ee_putchar('`');
        break;
    case 4:
        ee_goto(x+1, y+1);
        ee_putstr(", X .");
        ee_goto(x, y+2);
        ee_putchar('/');
        ee_goto(x+6, y+2);
        ee_putchar('\\');
        break;
    }

    ee_goto(x+2, y+2);
    ee_putstr("`V'");

    ee_color(EE_WHITE);
    ee_goto(x+2, y+1);
    ee_putchar('o');
    ee_goto(x+4, y+1);
    ee_putchar('o');
}


