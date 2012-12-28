#!/usr/bin/php5
<?php
  /*
   *  colors        display all possible libcaca colour pairs
   *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
   *
   *  This file is a Php port of "examples/colors.c"
   *  Copyright (c) 2003-2004 Sam Hocevar <sam@hocevar.net>
   *                All Rights Reserved
   *
   *  This program is free software. It comes without any warranty, to
   *  the extent permitted by applicable law. You can redistribute it
   *  and/or modify it under the terms of the Do What the Fuck You Want
   *  to Public License, Version 2, as published by Sam Hocevar. See
   *  http://www.wtfpl.net/ for more details.
   */

if (php_sapi_name() != "cli") {
    die("You have to run this program with php-cli!\n");
}

$cv = caca_create_canvas(80, 24);
if(!$cv)
{
    die("Failed to create canvas\n");
}

$dp = caca_create_display($cv);
if(!$dp)
{
    die("Failed to create display\n");
}

caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
caca_clear_canvas($cv);
for($i = 0; $i < 16; $i++)
{
    caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
    caca_put_str($cv, 3, $i + ($i >= 8 ? 3 : 2), "ANSI " . $i);
    for($j = 0; $j < 16; $j++)
    {
        caca_set_color_ansi($cv, $i, $j);
        caca_put_str($cv, ($j >= 8 ? 13 : 12) + $j * 4, $i + ($i >= 8 ? 3 : 2),
                    "Aaホ");
    }
}

caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
caca_put_str($cv, 3, 20, "This is bold    This is blink    This is italics    This is underline");
caca_set_attr($cv, CACA_BOLD);
caca_put_str($cv, 3 + 8, 20, "bold");
caca_set_attr($cv, CACA_BLINK);
caca_put_str($cv, 3 + 24, 20, "blink");
caca_set_attr($cv, CACA_ITALICS);
caca_put_str($cv, 3 + 41, 20, "italics");
caca_set_attr($cv, CACA_UNDERLINE);
caca_put_str($cv, 3 + 60, 20, "underline");

caca_refresh_display($dp);
caca_get_event($dp, CACA_EVENT_KEY_PRESS, -1);

?>
