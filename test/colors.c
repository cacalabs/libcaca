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
    cucul_t *qq;
    caca_t *kk;
    int i, j;

    qq = cucul_init(0, 0);
    if(!qq)
        return 1;

    kk = caca_attach(qq);
    if(!kk)
        return 1;

    cucul_clear(qq);
    for(i = 0; i < 16; i++)
    {
        cucul_set_color(qq, CUCUL_COLOR_LIGHTGRAY, CUCUL_COLOR_BLACK);
        cucul_printf(qq, 4, i + (i >= 8 ? 4 : 3), "'%c': %i (%s)",
                     'a' + i, i, cucul_get_color_name(i));
        for(j = 0; j < 16; j++)
        {
            cucul_set_color(qq, i, j);
            cucul_putstr(qq, (j >= 8 ? 41 : 40) + j * 2, i + (i >= 8 ? 4 : 3),
                         "# ");
        }
    }

    caca_display(kk);
    caca_wait_event(kk, CACA_EVENT_KEY_PRESS);

    caca_detach(kk);
    cucul_end(qq);

    return 0;
}

