/*
 *   libcaca       ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id$
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307  USA
 */

#ifndef __CACA_H__
#define __CACA_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Colors
 */
enum caca_color
{
    CACA_COLOR_BLACK = 0,
    CACA_COLOR_BLUE = 1,
    CACA_COLOR_GREEN = 2,
    CACA_COLOR_CYAN = 3,
    CACA_COLOR_RED = 4,
    CACA_COLOR_MAGENTA = 5,
    CACA_COLOR_BROWN = 6,
    CACA_COLOR_LIGHTGRAY = 7,
    CACA_COLOR_DARKGRAY = 8,
    CACA_COLOR_LIGHTBLUE = 9,
    CACA_COLOR_LIGHTGREEN = 10,
    CACA_COLOR_LIGHTCYAN = 11,
    CACA_COLOR_LIGHTRED = 12,
    CACA_COLOR_LIGHTMAGENTA = 13,
    CACA_COLOR_YELLOW = 14,
    CACA_COLOR_WHITE = 15
};

/*
 * Dithering modes
 */
enum caca_dithering
{
    CACA_DITHER_NONE = 0,
    CACA_DITHER_RANDOM = 1
};

/*
 * Types
 */
struct caca_sprite;

/*
 * Prototypes
 */
int caca_init(void);
void caca_set_delay(unsigned int);
void caca_set_dithering(enum caca_dithering);
unsigned int caca_get_rendertime(void);
unsigned int caca_get_width(void);
unsigned int caca_get_height(void);
const char *caca_get_color_name(unsigned int);
void caca_refresh(void);
void caca_end(void);

char caca_get_key(void);

void caca_set_color(enum caca_color);
enum caca_color caca_get_color(void);
void caca_putchar(int, int, char);
void caca_putstr(int, int, const char *);
void caca_printf(int, int, const char *, ...);
void caca_clear(void);

void caca_draw_line(int, int, int, int, char);
void caca_draw_polyline(const int[], const int[], int, char);
void caca_draw_thin_line(int, int, int, int);
void caca_draw_thin_polyline(const int[], const int[], int);

void caca_draw_circle(int, int, int, char);
void caca_draw_ellipse(int, int, int, int, char);
void caca_draw_thin_ellipse(int, int, int, int);
void caca_fill_ellipse(int, int, int, int, char);

void caca_draw_box(int, int, int, int, char);
void caca_draw_thin_box(int, int, int, int);
void caca_fill_box(int, int, int, int, char);

void caca_draw_triangle(int, int, int, int, int, int, char);
void caca_draw_thin_triangle(int, int, int, int, int, int);
void caca_fill_triangle(int, int, int, int, int, int, char);

int caca_rand(int, int);
unsigned int caca_sqrt(unsigned int);

struct caca_sprite * caca_load_sprite(const char *);
int caca_get_sprite_frames(struct caca_sprite *);
int caca_get_sprite_width(struct caca_sprite *, int);
int caca_get_sprite_height(struct caca_sprite *, int);
int caca_get_sprite_dx(struct caca_sprite *, int);
int caca_get_sprite_dy(struct caca_sprite *, int);
void caca_draw_sprite(int, int, struct caca_sprite *, int);
void caca_free_sprite(struct caca_sprite *);

void caca_blit(int, int, int, int, void *, int, int);

#ifdef __cplusplus
}
#endif

#endif /* __CACA_H__ */
