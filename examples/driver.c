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
#   include <string.h>
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

int main(int argc, char *argv[])
{
    char const * const *list;
    caca_display_t *dp;
    cucul_canvas_t *cv;

    list = caca_get_display_driver_list();

    dp = caca_create_display(NULL);
    if(dp == NULL)
    {
        printf("cannot create display\n");
        return -1;
    }

    cv = caca_get_canvas(dp);
    cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLACK);

    while(1)
    {
        char const *driver;
        int i, cur = 0;

        cucul_put_str(cv, 1, 0, "Available drivers:");

        driver = caca_get_display_driver(dp);

        for(i = 0; list[i]; i += 2)
        {
            int match = !strcmp(list[i], driver);

            if(match)
                cur = i;
            cucul_draw_line(cv, 0, i + 2, 9999, i + 2, ' ');
            cucul_printf(cv, 2, i + 2, "%c %s (%s)",
                         match ? '*' : ' ', list[i], list[i + 1]);
        }

        cucul_put_str(cv, 1, i + 2, "Switching driver in 5 seconds");

        caca_refresh_display(dp);

        if(caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, 5000000))
            break;

        do
        {
            cur += 2;
            if(list[cur] && !strcmp(list[cur], "raw"))
                cur += 2;
            if(!list[cur])
                cur = 0;
        }
        while(caca_set_display_driver(dp, list[cur]));
    }

    caca_free_display(dp);

    return 0;
}

