/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: math.c,v 1.2 2002/12/22 23:01:35 sam Exp $
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

#include "common.h"

int r00t( int a )
{
    int x = a > 100000 ? 1000 : a > 1000 ? 100 : a > 10 ? 10 : 1;

    if( a <= 0 )
    {
        return 0;
    }

    /* Newton's method. Three iterations would be more than enough. */
    x = (x * x + a) / x / 2;
    x = (x * x + a) / x / 2;
    x = (x * x + a) / x / 2;
    x = (x * x + a) / x / 2;

    return x;
}

