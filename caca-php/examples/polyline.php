#!/usr/bin/php5
<?php
/*
 *  polyline.php      sample program for libcaca php binding
 *  Copyright (c) 2008 Nicolas Vion <nico@yojik.eu>
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

function transform($tbl, $tx, $ty, $sx, $sy) {
	$result = array();
	foreach($tbl as $pt)
		$result[] = array($pt[0] * $sx + $tx, $pt[1] * $sy + $ty);
	return $result;
}

if (!posix_isatty(STDOUT))
	die("You have to run this program with php-cli!\n");

$canvas = caca_create_canvas(0, 0);
$display = caca_create_display($canvas);
if (!$display) {
	die("Error while attaching canvas to display\n");
}

$tbl = array(
	array(5, 2),
	array(15, 2),
	array(20, 4),
	array(25, 2),
	array(34, 2),
	array(37, 4),
	array(36, 9),
	array(20, 16),
	array(3, 9),
	array(2, 4),
	array(5, 2)
);

for ($i = 0; $i < 10; $i++) {
	caca_set_color_ansi($canvas, 1 + (($color += 4) % 15), CACA_TRANSPARENT);
	$scale = caca_rand(4, 10) / 10;
	$translate = array(caca_rand(-5, 55), caca_rand(-2, 25));
	$pts = transform($tbl, $translate[0], $translate[1], $scale, $scale);
	caca_draw_thin_polyline($canvas, $pts);
}

caca_put_str($canvas, 1, 1, "Caca forever...");
caca_refresh_display($display);
caca_get_event($display, CACA_EVENT_KEY_PRESS, 5000000);

?>
