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

/**  \file math.c
 *   \version \$Id$
 *   \author Sam Hocevar <sam@zoy.org>
 *   \brief Math functions
 *
 *   This file contains simple mathematical routines.
 */

#include "config.h"

#include <stdlib.h>

#include "caca.h"
#include "caca_internals.h"

/**
 * \brief Generate a random integer within a range.
 *
 * \param min The lower bound of the integer range.
 * \param max The upper bound of the integer range.
 * \return A random integer comprised between \p min and \p max, inclusive.
 */
int caca_rand(int min, int max)
{
    return min + (int)((1.0*(max-min+1)) * rand() / (RAND_MAX+1.0));
}

/**
 * \brief Approximate a square root, using Newton's method to avoid
 *        costly floating point calculations.
 *
 * \param a A positive integer.
 * \return The approximate square root of \p a.
 */
unsigned int caca_sqrt(unsigned int a)
{
    if(a == 0)
        return 0;

    if(a < 1000000000)
    {
        unsigned int x = a < 10 ? 1
                       : a < 1000 ? 10
                       : a < 100000 ? 100
                       : a < 10000000 ? 1000
                       : 10000;

        /* Newton's method. Three iterations would be more than enough. */
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;
        x = (x * x + a) / x / 2;

        return x;
    }

    return 2 * caca_sqrt(a / 4);
}

