#!/usr/bin/php5
<?php
/*
 *  transform       transformation test program
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "examples/transform.c"
 *  which is: 
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
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

if (php_sapi_name() != "cli") {
	die("You have to run this program with php-cli!\n");
}

$pig = (
	",--.   ,--.\n" .
	"\\  /-~-\\  /\n" .
	" )' o O `(\n" .
	"(  ,---.  )\n" .
	" `(_o_o_)'\n" .
	"   )`-'(\n");

$duck = (
	"                ,~~.\n" .
	"    __     ,   (  O )>\n" .
	"___( o)>   )`~~'   (\n" .
	"\\ <_. )   (  .__)   )\n" .
	" `---'     `-.____,'\n");

$cv = caca_create_canvas(0, 0);
if(! $cv)
{
	die("Can't created canvas\n");
}
$dp = caca_create_display($cv);
if(! $dp)
{
	die("Can't create display\n");
}

$image = caca_create_canvas(70, 6);
$tmp = caca_create_canvas(70, 6);
$sprite = caca_create_canvas(0, 0);

caca_set_color_ansi($sprite, CACA_LIGHTMAGENTA, CACA_BLACK);
caca_import_string($sprite, $pig, "text");
caca_blit($image, 55, 0, $sprite);

caca_set_color_ansi($sprite, CACA_LIGHTGREEN, CACA_BLACK);
caca_import_string($sprite, $duck, "text");
caca_blit($image, 30, 1, $sprite);

caca_set_color_ansi($image, CACA_LIGHTCYAN, CACA_BLACK);
caca_put_str($image, 1, 1, "hahaha mais vieux porc immonde !! [⽼ ⾗]");
caca_set_color_ansi($image, CACA_LIGHTRED, CACA_BLACK);
caca_put_char($image, 38, 1, ord('|'));

caca_set_color_ansi($image, CACA_YELLOW, CACA_BLACK);
caca_put_str($image, 4, 2, "\\o\\ \\o| _o/ \\o_ |o/ /o/");

caca_set_color_ansi($image, CACA_WHITE, CACA_LIGHTRED);
caca_put_str($image, 7, 3, "▙▘▌▙▘▞▖▞▖▌ ▞▖▌ ▌▌");
caca_put_str($image, 7, 4, "▛▖▌▛▖▚▘▚▘▚▖▚▘▚▖▖▖");
caca_set_color_ansi($image, CACA_BLACK, CACA_LIGHTRED);
caca_put_str($image, 4, 3, "▓▒░");
caca_put_str($image, 4, 4, "▓▒░");
caca_put_str($image, 24, 3, "░▒▓");
caca_put_str($image, 24, 4, "░▒▓");

/* Blit the transformed canvas onto the main canvas */
caca_set_color_ansi($cv, CACA_WHITE, CACA_BLUE);
caca_put_str($cv, 0, 0, "normal");
caca_blit($cv, 10, 0, $image);

caca_put_str($cv, 0, 6, "flip");
caca_blit($tmp, 0, 0, $image);
caca_flip($tmp);
caca_blit($cv, 10, 6, $tmp);

caca_put_str($cv, 0, 12, "flop");
caca_blit($tmp, 0, 0, $image);
caca_flop($tmp);
caca_blit($cv, 10, 12, $tmp);

caca_put_str($cv, 0, 18, "rotate");
caca_blit($tmp, 0, 0, $image);
caca_rotate_180($tmp);
caca_blit($cv, 10, 18, $tmp);

caca_refresh_display($dp);

caca_get_event($dp, CACA_EVENT_KEY_PRESS, -1);
?>
