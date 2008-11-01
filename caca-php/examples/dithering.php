#!/usr/bin/php5
<?php
/*
 *  dithering.php      sample program for libcaca php binding
 *  Copyright (c) 2008 Nicolas Vion <nico@yojik.eu>
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

$img = imagecreatefrompng(dirname(__FILE__)."/logo-caca.png");
if (!$img) 
	die("Can not open image.\n");

$dither = caca_create_dither($img);
if (!$dither) 
	die("Can not create dither. Maybe you compiled caca-php without gd support.\n");

$canvas = caca_create_canvas(0, 0);
$display = caca_create_display($canvas);
if (!$display) 
	die("Can not create display.\n");

caca_dither_bitmap($canvas, 0, 0, caca_get_canvas_width($canvas), caca_get_canvas_height($canvas), $dither, $img);
caca_refresh_display($display);

sleep(5);

?>
