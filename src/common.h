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

/*
 * Compile-time limits
 */
#define STARS 50
#define WEAPONS 200
#define BONUS 30
#define ALIENS 30
#define EXPLOSIONS 200

/*
 * Game defines
 */
#define MAX_LIFE 1000
#define MAX_SPECIAL 200

#define COST_NUKE     (100*MAX_SPECIAL/100)
#define COST_BEAM      (75*MAX_SPECIAL/100)
#define COST_FRAGBOMB  (50*MAX_SPECIAL/100)

/*
 * Graphics primitives
 */
#include "ee.h"

/*
 * Useful macros
 */
#define GET_RAND(p,q) ((p)+(int)((1.0*((q)-(p)))*rand()/(RAND_MAX+1.0)))
#define GET_MAX(a,b) ((a)>(b)?(a):(b))
#define GET_MIN(a,b) ((a)<(b)?(a):(b))

/*
 * Game structures
 */
typedef struct
{
    int w, h, *left, *right;

} tunnel;

typedef struct
{
    int x, y, z, c;
    char ch;

} starfield;

typedef struct
{
    enum { EXPLOSION_NONE, EXPLOSION_SMALL, EXPLOSION_MEDIUM } type[EXPLOSIONS];
    int x[EXPLOSIONS];
    int y[EXPLOSIONS];
    int vx[EXPLOSIONS];
    int vy[EXPLOSIONS];
    int n[EXPLOSIONS];

} explosions;

typedef struct
{
    enum { WEAPON_NONE, WEAPON_LASER, WEAPON_SEEKER, WEAPON_NUKE, WEAPON_BEAM, WEAPON_LIGHTNING, WEAPON_BOMB, WEAPON_FRAGBOMB } type[WEAPONS];
    int x[WEAPONS];
    int y[WEAPONS];
    int x2[WEAPONS];
    int y2[WEAPONS];
    int x3[WEAPONS];
    int y3[WEAPONS];
    int vx[WEAPONS];
    int vy[WEAPONS];
    int n[WEAPONS];

} weapons;

typedef struct
{
    enum { BONUS_NONE, BONUS_LIFE, BONUS_GREEN } type[BONUS];
    int x[BONUS];
    int y[BONUS];
    int n[BONUS];

} bonus;

typedef struct
{
    int x, y;
    int vx, vy;
    int weapon, special;
    int life, dead;

} player;

typedef struct
{
    enum { ALIEN_NONE, ALIEN_FOO, ALIEN_BAR, ALIEN_BAZ } type[ALIENS];
    int x[ALIENS];
    int y[ALIENS];
    int life[ALIENS];
    int img[ALIENS];

} aliens;

typedef struct
{
    int w, h;
    int x, y;
    int frame;

} box;

typedef struct
{
    int w, h;

    starfield *sf;
    weapons *wp;
    explosions *ex;
    tunnel *t;
    player *p;
    aliens *al;
    bonus *bo;

} game;

/*
 * From aliens.c
 */
void init_aliens(game *, aliens *);
void draw_aliens(game *, aliens *);
void update_aliens(game *, aliens *);
void add_alien(game *, aliens *, int, int, int);

/*
 * From bonus.c
 */
void init_bonus(game *, bonus *);
void draw_bonus(game *, bonus *);
void update_bonus(game *, bonus *);
void add_bonus(game *, bonus *, int, int, int);

/*
 * From box.c
 */
box * create_box(game *, int, int, int, int);
void draw_box(game *, box *);
void free_box(box *);

/*
 * From ceo.c
 */
void ceo_alert(game *);

/*
 * From collide.c
 */
void collide_weapons_tunnel(game *, weapons *, tunnel *, explosions *);
void collide_weapons_aliens(game *, weapons *, aliens *, explosions *);
void collide_player_tunnel(game *, player *, tunnel *, explosions *);

/*
 * From explosions.c
 */
void init_explosions(game *, explosions *);
void add_explosion(game *, explosions *, int, int, int, int, int);
void draw_explosions(game *, explosions *);
void update_explosions(game *, explosions *);

/*
 * From math.c
 */
int r00t(int);

/*
 * From overlay.c
 */
void draw_status(game *);

/*
 * From player.c
 */
player * create_player(game *);
void free_player(player *);
void draw_player(game *, player *);
void update_player(game *, player *);

/*
 * From starfield.c
 */
starfield * create_starfield(game *);
void draw_starfield(game *, starfield *);
void update_starfield(game *, starfield *);
void free_starfield(game *, starfield *);

/*
 * From tunnel.c
 */
tunnel * create_tunnel(game *, int, int);
void free_tunnel(tunnel *);
void draw_tunnel(game *, tunnel *);
void update_tunnel(game *, tunnel *);

/*
 * From weapons.c
 */
void init_weapons(game *, weapons *);
void draw_weapons(game *, weapons *);
void update_weapons(game *, weapons *);
void add_weapon(game *, weapons *, int, int, int, int, int);

