/*
 *   libcaca       ASCII-Art library
 *   Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
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

/**  \file caca_internals.h
 *   \version \$Id$
 *   \author Sam Hocevar <sam@zoy.org>
 *   \brief The \e libcaca private header.
 *
 *   This header contains the private types and functions used by \e libcaca.
 */

#ifndef __CACA_INTERNALS_H__
#define __CACA_INTERNALS_H__

extern int _caca_init_graphics(void);

#if defined(USE_NCURSES)
extern int _caca_attr[];
#endif

#if defined(USE_CONIO)
extern struct text_info _ti;
extern char *_caca_screen;
#endif

extern unsigned int _caca_width;
extern unsigned int _caca_height;

extern char *_caca_empty_line;
extern char *_caca_scratch_line;

#endif /* __CACA_INTERNALS_H__ */
