/*
 *  colors        display all possible libcaca colour pairs
 *  Copyright (c) 2003-2004 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

#include "config.h"

#include <stdio.h>

#include "caca.h"

int main(int argc, char **argv)
{
    int i, j;

    if(caca_init())
        return 1;

    caca_clear();
    for(i = 0; i < 16; i++)
    {
        caca_set_color(CACA_COLOR_LIGHTGRAY, CACA_COLOR_BLACK);
        caca_printf(4, i + (i >= 8 ? 4 : 3), "'%c': %i (%s)",
                    'a' + i, i, caca_get_color_name(i));
        for(j = 0; j < 16; j++)
        {
            caca_set_color(i, j);
            caca_putstr((j >= 8 ? 41 : 40) + j * 2, i + (i >= 8 ? 4 : 3), "# ");
        }
    }

    caca_refresh();
    caca_wait_event(CACA_EVENT_KEY_PRESS);
    caca_end();

    return 0;
}

