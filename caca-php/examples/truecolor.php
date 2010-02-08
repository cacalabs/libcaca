#!/usr/bin/php5
<?php
/*
 *  truecolor      truecolor canvas features
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "examples/truecolor.c"
 *  which is:
 *  Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
 *          All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

if (php_sapi_name() != "cli") {
    die("You have to run this program with php-cli!\n");
}

$cv = caca_create_canvas(32, 16);
if(!$cv) {
    die("Failed to create canvas\n");
}

$dp = caca_create_display($cv);
if(!$dp) {
    die("Failed to create display\n");
}

for($y = 0; $y < 16; $y++)
    for($x = 0; $x < 16; $x++)
    {
        $bgcolor = 0xff00 | ($y << 4) | $x;
        $fgcolor = 0xf000 | ((15 - $y) << 4) | ((15 - $x) << 8);

        caca_set_color_argb($cv, $fgcolor, $bgcolor);
        caca_put_str($cv, $x * 2, $y, "CA");
    }

caca_set_color_ansi($cv, CACA_WHITE, CACA_LIGHTBLUE);
caca_put_str($cv, 2, 1, " truecolor libcaca ");

caca_refresh_display($dp);

caca_get_event($dp, CACA_EVENT_KEY_PRESS, -1);

?>
