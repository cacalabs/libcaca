/*
 *  libcaca       ASCII-Art library
 *  Copyright (c) 2002, 2003, 2004 Sam Hocevar <sam@zoy.org>
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

/** \file time.c
 *  \version \$Id$
 *  \author Sam Hocevar <sam@zoy.org>
 *  \brief Timer routines
 *
 *  This file contains simple timer routines.
 */

#include "config.h"

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "caca.h"
#include "caca_internals.h"

unsigned int _caca_getticks(struct caca_timer *timer)
{
    struct timeval tv;
    unsigned int ticks = 0;

    gettimeofday(&tv, NULL);

    if(timer->last_sec != 0)
    {
        /* If the delay was greater than 60 seconds, return 10 seconds
         * otherwise we may overflow our ticks counter. */
        if(tv.tv_sec >= timer->last_sec + 60)
            ticks = 60 * 1000000;
        else
        {
            ticks = (tv.tv_sec - timer->last_sec) * 1000000;
            ticks += tv.tv_usec;
            ticks -= timer->last_usec;
        }
    }

    timer->last_sec = tv.tv_sec;
    timer->last_usec = tv.tv_usec;

    return ticks;
}

