/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002, 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

/** \file caca_internals.h
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief The \e libcaca private header.
 *
 *  This header contains the private types and functions used by \e libcaca.
 */

#ifndef __CACA_INTERNALS_H__
#define __CACA_INTERNALS_H__

/* Graphics driver */
enum caca_driver
{
#if defined(USE_CONIO)
    CACA_DRIVER_CONIO = 1,
#endif
#if defined(USE_NCURSES)
    CACA_DRIVER_NCURSES = 2,
#endif
#if defined(USE_SLANG)
    CACA_DRIVER_SLANG = 3,
#endif
#if defined(USE_X11)
    CACA_DRIVER_X11 = 4,
#endif
    CACA_DRIVER_NONE = 0
};

extern enum caca_driver _caca_driver;

/* Initialisation functions */
extern int _caca_init_graphics(void);
extern int _caca_end_graphics(void);

/* Cached screen size */
extern unsigned int _caca_width;
extern unsigned int _caca_height;

/* Internal libcaca features */
extern enum caca_feature _caca_background;
extern enum caca_feature _caca_dithering;
extern enum caca_feature _caca_antialiasing;

#if defined(USE_X11)
#include <X11/Xlib.h>
extern Display *x11_dpy;
extern Window x11_window;
#endif

#endif /* __CACA_INTERNALS_H__ */
