/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: common.h,v 1.14 2002/12/23 15:06:13 sam Exp $
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
#ifdef USE_SLANG
#   include <slang.h>
#   define gfx_color(x) SLsmg_set_color(x)
#   define gfx_goto(x,y) SLsmg_gotorc(y,x)
#   define gfx_putchar(x) SLsmg_write_char(x)
#   define gfx_putstr(x) SLsmg_write_string(x)
#elif USE_NCURSES
#   include <curses.h>
#   define gfx_color(x) attrset(COLOR_PAIR(x))
#   define gfx_goto(x,y) move(y,x)
#   define gfx_putchar(x) addch(x)
#   define gfx_putstr(x) addstr(x)
#else
#   define gfx_color(x) do{}while(0)
#   define gfx_goto(x,y) do{}while(0)
#   define gfx_putchar(x) do{}while(0)
#   define gfx_putstr(x) do{}while(0)
#endif

#define gfx_putcharTO(x,y,c) do{ gfx_goto(x,y); gfx_putchar(c); }while(0)

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

/*
 * From aliens.c
 */
void init_aliens( game *g, aliens *al );
void draw_aliens( game *g, aliens *al );
void update_aliens( game *g, aliens *al );
void add_alien( game *g, aliens *al, int x, int y, int type );

/*
 * From bonus.c
 */
void init_bonus( game *g, bonus *bo );
void draw_bonus( game *g, bonus *bo );
void update_bonus( game *g, bonus *bo );
void add_bonus( game *g, bonus *bo, int x, int y, int type );

/*
 * From ceo.c
 */
void ceo_alert( void );

/*
 * From collide.c
 */
void collide_weapons_tunnel( game *g, weapons *wp, tunnel *t, explosions *ex );
void collide_weapons_aliens( game *g, weapons *wp, aliens *al, explosions *ex );
void collide_player_tunnel( game *g, player *p, tunnel *t, explosions *ex );

/*
 * From explosions.c
 */
void init_explosions( game *g, explosions *ex );
void add_explosion( game *g, explosions *ex, int x, int y, int vx, int vy, int type );
void draw_explosions( game *g, explosions *ex );
void update_explosions( game *g, explosions *ex );

/*
 * From graphics.c
 */
int init_graphics( void );
void init_game( game *g );
char get_key( void );
void clear_graphics( void );
void refresh_graphics( void );
void end_graphics( void );

/*
 * From math.c
 */
int r00t( int a );

/*
 * From overlay.c
 */
void draw_overlay( game *g );

/*
 * From player.c
 */
player * create_player( game *g );
void free_player( player *p );
void draw_player( game *g, player *p );
void update_player( game *g, player *p );

/*
 * From starfield.c
 */
starfield * create_starfield( game *g );
void draw_starfield( game *g, starfield *s );
void update_starfield( game *g, starfield *s );
void free_starfield( game *g, starfield *s );

/*
 * From tunnel.c
 */
tunnel * create_tunnel( game *g, int w, int h );
void free_tunnel( tunnel *t );
void draw_tunnel( game *g, tunnel *t );
void update_tunnel( game *g, tunnel *t );

/*
 * From weapons.c
 */
void init_weapons( game *g, weapons *wp );
void draw_weapons( game *g, weapons *wp );
void update_weapons( game *g, weapons *wp );
void add_weapon( game *g, weapons *wp, int x, int y, int vx, int vy, int type );

