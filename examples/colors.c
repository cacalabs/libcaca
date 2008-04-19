/*
 *  colors        display all possible libcaca colour pairs
 *  Copyright (c) 2003-2004 Sam Hocevar <sam@zoy.org>
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

#if !defined(__KERNEL__)
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

int main(int argc, char **argv)
{
    cucul_canvas_t *cv;
    caca_display_t *dp;
    int i, j;

    cv = cucul_create_canvas(80, 24);
    if(cv == NULL)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Failed to create display\n");
        return 1;
    }

    cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
    cucul_clear_canvas(cv);
    for(i = 0; i < 16; i++)
    {
        cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
        cucul_printf(cv, 3, i + (i >= 8 ? 3 : 2), "ANSI %i", i);
        for(j = 0; j < 16; j++)
        {
            cucul_set_color_ansi(cv, i, j);
            cucul_put_str(cv, (j >= 8 ? 13 : 12) + j * 4, i + (i >= 8 ? 3 : 2),
                          "Aaホ");
        }
    }

    cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
    cucul_put_str(cv, 3, 20, "This is bold    This is blink    This is italics    This is underline");
    cucul_set_attr(cv, CUCUL_BOLD);
    cucul_put_str(cv, 3 + 8, 20, "bold");
    cucul_set_attr(cv, CUCUL_BLINK);
    cucul_put_str(cv, 3 + 24, 20, "blink");
    cucul_set_attr(cv, CUCUL_ITALICS);
    cucul_put_str(cv, 3 + 41, 20, "italics");
    cucul_set_attr(cv, CUCUL_UNDERLINE);
    cucul_put_str(cv, 3 + 60, 20, "underline");

    caca_refresh_display(dp);
    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

