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

#ifndef __EE_H__
#define __EE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Colors
 */
enum ee_colors
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

extern char *ee_color_names[16];

/*
 * Types
 */
struct ee_sprite;

/*
 * Prototypes
 */
int ee_init(void);
void ee_set_delay(int);
int ee_get_width(void);
int ee_get_height(void);
void ee_refresh(void);
void ee_end(void);

char ee_get_key(void);

void ee_set_color(int);
int ee_get_color(void);
void ee_putchar(int, int, char);
void ee_putstr(int, int, char *);
void ee_clear(void);

void ee_draw_line(int, int, int, int, char);
void ee_draw_polyline(int[], int[], int, char);
void ee_draw_thin_line(int, int, int, int);
void ee_draw_thin_polyline(int[], int[], int);

void ee_draw_circle(int, int, int, char);
void ee_draw_ellipse(int, int, int, int, char);
void ee_draw_thin_ellipse(int, int, int, int);
void ee_fill_ellipse(int, int, int, int, char);

void ee_draw_box(int, int, int, int, char);
void ee_draw_thin_box(int, int, int, int);
void ee_fill_box(int, int, int, int, char);

void ee_draw_triangle(int, int, int, int, int, int, char);
void ee_draw_thin_triangle(int, int, int, int, int, int);
void ee_fill_triangle(int, int, int, int, int, int, char);

int ee_rand(int, int);
int ee_sqrt(int);

struct ee_sprite * ee_load_sprite(const char *);
int ee_get_sprite_frames(struct ee_sprite *);
int ee_get_sprite_width(struct ee_sprite *, int);
int ee_get_sprite_height(struct ee_sprite *, int);
int ee_get_sprite_dx(struct ee_sprite *, int);
int ee_get_sprite_dy(struct ee_sprite *, int);
void ee_draw_sprite(int, int, struct ee_sprite *, int);
void ee_free_sprite(struct ee_sprite *);

#ifdef __cplusplus
}
#endif

#endif /* __EE_H__ */
