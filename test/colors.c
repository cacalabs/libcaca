/*
 *  colors        display all possible libcaca colour pairs
 *  Copyright (c) 2003-2004 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

int main(int argc, char **argv)
{
    cucul_canvas_t *cv;
    caca_display_t *dp;
    caca_event_t ev;
    int i, j;

    cv = cucul_create_canvas(0, 0);
    if(!cv)
        return 1;

    dp = caca_attach(cv);
    if(!dp)
        return 1;

    cucul_clear_canvas(cv, CUCUL_COLOR_BLACK);
    for(i = 0; i < 16; i++)
    {
        cucul_set_color(cv, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
        cucul_printf(cv, 4, i + (i >= 8 ? 4 : 3), "'%cv': %i (%s)",
                     'a' + i, i, cucul_get_color_name(i));
        for(j = 0; j < 16; j++)
        {
            cucul_set_color(cv, i, j);
            cucul_putstr(cv, (j >= 8 ? 41 : 40) + j * 2, i + (i >= 8 ? 4 : 3),
                         "# ");
        }
    }

    caca_display(dp);
    caca_get_event(dp, CACA_EVENT_KEY_PRESS, &ev, -1);

    caca_detach(dp);
    cucul_free_canvas(cv);

    return 0;
}

