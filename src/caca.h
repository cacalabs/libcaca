/*
 *   libcaca       ASCII-Art library
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

#ifndef __EE_H__
#define __EE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Colors
 */
enum caca_colors
{
    EE_BLACK = 0,
    EE_BLUE = 1,
    EE_GREEN = 2,
    EE_CYAN = 3,
    EE_RED = 4,
    EE_MAGENTA = 5,
    EE_BROWN = 6,
    EE_LIGHTGRAY = 7,
    EE_DARKGRAY = 8,
    EE_LIGHTBLUE = 9,
    EE_LIGHTGREEN = 10,
    EE_LIGHTCYAN = 11,
    EE_LIGHTRED = 12,
    EE_LIGHTMAGENTA = 13,
    EE_YELLOW = 14,
    EE_WHITE = 15
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
unsigned int caca_get_rendertime(void);
unsigned int caca_get_width(void);
unsigned int caca_get_height(void);
const char *caca_get_color_name(unsigned int);
void caca_refresh(void);
void caca_end(void);

char caca_get_key(void);

void caca_set_color(int);
int caca_get_color(void);
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

#endif /* __EE_H__ */
