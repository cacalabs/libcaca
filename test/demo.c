/*
 *   demo          demo using libee
 *   Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
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

#include "ee.h"

int main(int argc, char **argv)
{
    int quit = 0;

    if(ee_init())
    {
        return 1;
    }

    /* Go ! */
    while(!quit)
    {
        int i;
        char key = ee_get_key();
        if(key)
        {
            quit = 1;
        }

        ee_clear();
        for(i=1000; i--;)
        {
            ee_goto(ee_rand(0,ee_get_width()-1),
                    ee_rand(0,ee_get_height()-1));
            ee_color(ee_rand(1,10));
            ee_putchar('#');
        }

        ee_refresh();
    }

    /* Clean up */
    ee_end();

    return 0;
}

