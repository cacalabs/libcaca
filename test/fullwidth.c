/*
 *  fullwidth     libcaca fullwidth Unicode characters test program
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
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
#include "common.h"

#if !defined(__KERNEL__)
#   if defined(HAVE_INTTYPES_H)
#      include <inttypes.h>
#   endif
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

#define HOHO "ホホホホホホホホホホホホホホホ"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv, *hoho;
    caca_display_t *dp;

    unsigned int i;

    cv = cucul_create_canvas(0, 0);
    dp = caca_create_display(cv);

    hoho = cucul_create_canvas(6, 10);

    for(i = 0; i < 10; i++)
    {
        cucul_set_color(hoho, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_putstr(hoho, 0, i, HOHO);
        cucul_set_color(hoho, CUCUL_COLOR_WHITE, CUCUL_COLOR_RED);
        cucul_putchar(hoho, i - 2, i, 'x');
    }

    cucul_blit(cv, 1, 1, hoho, NULL);

    for(i = 0; i < 10; i++)
    {
        cucul_set_color(hoho, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLUE);
        cucul_putstr(hoho, 0, i, HOHO);
        cucul_set_color(hoho, CUCUL_COLOR_WHITE, CUCUL_COLOR_GREEN);
        cucul_putstr(hoho, i - 2, i, "ホ");
    }

    cucul_blit(cv, 15, 1, hoho, NULL);

    caca_refresh_display(dp);

    caca_get_event(dp, CACA_EVENT_KEY_PRESS, NULL, -1);

    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

