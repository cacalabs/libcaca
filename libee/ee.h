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

/*
 * Graphics primitives
 */
#ifdef USE_SLANG
#   include <slang.h>
#   define ee_color(x) SLsmg_set_color(x)
#   define ee_goto(x,y) SLsmg_gotorc(y,x)
#   define ee_putchar(x) SLsmg_write_char(x)
#   define ee_putstr(x) SLsmg_write_string(x)
#elif USE_NCURSES
#   define box box_divert
#   include <curses.h>
#   undef box
#   define ee_color(x) attrset(COLOR_PAIR(x))
#   define ee_goto(x,y) move(y,x)
#   define ee_putchar(x) addch(x)
#   define ee_putstr(x) addstr(x)
#else
#   define ee_color(x) (void)(x)
#   define ee_goto(x,y) do{ (void)(x); (void)(y); } while(0)
#   define ee_putchar(x) (void)(x)
#   define ee_putstr(x) (void)(x)
#endif

#define ee_putcharTO(x,y,c) do{ ee_goto(x,y); ee_putchar(c); }while(0)

/*
 * Colours
 */
#define EE_BLACK 1
#define EE_GREEN 2
#define EE_YELLOW 3
#define EE_WHITE 4
#define EE_RED 5
#define EE_GRAY 6
#define EE_LIGHTGRAY 7
#define EE_BLUE 8
#define EE_CYAN 9
#define EE_MAGENTA 10

/*
 * Prototypes
 */
int ee_init(void);
int ee_get_width(void);
int ee_get_height(void);
void ee_clear(void);
void ee_refresh(void);
void ee_end(void);

char ee_get_key(void);

