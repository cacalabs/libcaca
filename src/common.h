
#define STARS 50
#define WEAPONS 50
#define ROCKS 10
#define ALIENS 10
#define EXPLOSIONS 20

#ifdef USE_SLANG
#   include <slang.h>
#   define GFX_COLOR(x) SLsmg_set_color(x)
#   define GFX_GOTO(x,y) SLsmg_gotorc(y,x)
#   define GFX_WRITE(x) SLsmg_write_char(x)
#else
#   include <curses.h>
#   define GFX_COLOR(x) attrset(COLOR_PAIR(x))
#   define GFX_GOTO(x,y) move(y,x)
#   define GFX_WRITE(x) addch(x)
#endif

#define GFX_WRITETO(x,y,c) do{ GFX_GOTO(x,y); GFX_WRITE(c); }while(0)

#define GET_RAND(p,q) ((p)+(int)((1.0*((q)-(p)))*rand()/(RAND_MAX+1.0)))

typedef struct
{
    int w, h;

} game;

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
    int x[EXPLOSIONS];
    int y[EXPLOSIONS];
    int vx[EXPLOSIONS];
    int vy[EXPLOSIONS];
    int type[EXPLOSIONS];
    int n[EXPLOSIONS];

} explosions;

typedef struct
{
    int x[WEAPONS];
    int y[WEAPONS];
    int v[WEAPONS];
    int n[WEAPONS];
    int type[WEAPONS];

} weapons;

typedef struct
{
    int x, y;
    int dir;
    int weapon, nuke;

} player;

typedef struct
{
    int x[ALIENS];
    int y[ALIENS];
    int life[ALIENS];
    int img[ALIENS];

} aliens;

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
void add_alien( game *g, aliens *al, int x, int y );

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
void add_weapon( game *g, weapons *wp, int x, int y, int type );

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

