/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: common.h,v 1.9 2002/12/22 22:36:42 sam Exp $
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

#define STARS 50
#define WEAPONS 100
#define BONUS 30
#define ALIENS 30
#define EXPLOSIONS 100

#ifdef USE_SLANG
#   include <slang.h>
#   define gfx_color(x) SLsmg_set_color(x)
#   define gfx_goto(x,y) SLsmg_gotorc(y,x)
#   define gfx_putchar(x) SLsmg_write_char(x)
#   define gfx_putstr(x) SLsmg_write_string(x)
#else
#   include <curses.h>
#   define gfx_color(x) attrset(COLOR_PAIR(x))
#   define gfx_goto(x,y) move(y,x)
#   define gfx_putchar(x) addch(x)
#   define gfx_putstr(x) addstr(x)
#endif

#define gfx_putcharTO(x,y,c) do{ gfx_goto(x,y); gfx_putchar(c); }while(0)

#define GET_RAND(p,q) ((p)+(int)((1.0*((q)-(p)))*rand()/(RAND_MAX+1.0)))

typedef struct
{
    int w, h, *left, *right;

} tunnel;

typedef struct
{
    int x[STARS];
    int y[STARS];
    int z[STARS];
    char ch[STARS];
    int c[STARS];

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
    int dir;
    int weapon, nuke;

} player;

typedef struct
{
    enum { ALIEN_NONE, ALIEN_POOLP, ALIEN_BOOL, ALIEN_BRAH } type[ALIENS];
    int x[ALIENS];
    int y[ALIENS];
    int life[ALIENS];
    int img[ALIENS];

} aliens;

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

#define BLACK 1
#define GREEN 2
#define YELLOW 3
#define WHITE 4
#define RED 5
#define GRAY 6
#define LIGHTGRAY 7
#define BLUE 8
#define CYAN 9
#define MAGENTA 10

void collide_weapons_tunnel( game *g, weapons *wp, tunnel *t, explosions *ex );
void collide_weapons_aliens( game *g, weapons *wp, aliens *al, explosions *ex );
void collide_player_tunnel( game *g, player *p, tunnel *t, explosions *ex );

void init_aliens( game *g, aliens *al );
void draw_aliens( game *g, aliens *al );
void update_aliens( game *g, aliens *al );
void add_alien( game *g, aliens *al, int x, int y, int type );

int init_graphics( void );
void init_game( game *g );
char get_key( void );
void clear_graphics( void );
void refresh_graphics( void );
void end_graphics( void );

player * create_player( game *g );
void free_player( player *p );
void draw_player( game *g, player *p );
void update_player( game *g, player *p );

void init_weapons( game *g, weapons *wp );
void draw_weapons( game *g, weapons *wp );
void update_weapons( game *g, weapons *wp );
void add_weapon( game *g, weapons *wp, int x, int y, int vx, int vy, int type );

void init_bonus( game *g, bonus *bo );
void draw_bonus( game *g, bonus *bo );
void update_bonus( game *g, bonus *bo );
void add_bonus( game *g, bonus *bo, int x, int y, int type );

void init_starfield( game *g, starfield *s );
void draw_starfield( game *g, starfield *s );
void update_starfield( game *g, starfield *s );

tunnel * create_tunnel( game *g, int w, int h );
void free_tunnel( tunnel *t );
void draw_tunnel( game *g, tunnel *t );
void update_tunnel( game *g, tunnel *t );

void init_explosions( game *g, explosions *ex );
void add_explosion( game *g, explosions *ex, int x, int y, int vx, int vy, int type );
void draw_explosions( game *g, explosions *ex );
void update_explosions( game *g, explosions *ex );

void ceo_alert( void );

int r00t( int a );

