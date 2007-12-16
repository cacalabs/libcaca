/*
 *  driver        libcaca Unicode rendering test program
 *  Copyright (c) 2007 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"
#include "common.h"

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

int main(int argc, char *argv[])
{
    char const * const * list;
    caca_display_t *dp;
    cucul_canvas_t *cv;

    list = caca_get_display_driver_list();
    if(argc <= 1)
    {
        printf("Available drivers:\n");

        while(*list)
        {
            printf("  %s (%s)\n", list[0], list[1]);
            list += 2;
        }

        return 0;
    }

    cv = cucul_create_canvas(0, 0);
    if(cv == NULL)
    {
        printf("cannot create canvas\n");
        return -1;
    }
    dp = caca_create_display_with_driver(cv, argv[1]);
    if(dp == NULL)
    {
        printf("cannot create display\n");
        return -1;
    }

    cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLUE);
    cucul_draw_line(cv, 0, 0, 9999, 0, ' ');
    cucul_printf(cv, 1, 0, "Current driver: %s", argv[1]);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

