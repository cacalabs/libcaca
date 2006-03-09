/*
 *  unicode       libcaca Unicode rendering test program
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

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif

#include "cucul.h"
#include "caca.h"

int main(void)
{
    cucul_t *qq;
    caca_t *kk;

    qq = cucul_init();
    kk = caca_attach(qq);

    cucul_putstr(qq, 1, 1, "This is ASCII:   [ abc DEF 123 !@# ]");
    cucul_putstr(qq, 1, 2, "This is Unicode: [ äβç ΔЗҒ ░▒▓ ♩♔✈ ]");

    cucul_putstr(qq, 1, 4, "If the two lines do not have the same length, there is a bug somewhere.");

    caca_display(kk);

    while(!caca_get_event(kk, CACA_EVENT_KEY_PRESS));

    caca_detach(kk);
    cucul_end(qq);

    return 0;
}

