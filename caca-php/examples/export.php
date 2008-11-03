#!/usr/bin/php5
<?php
/*
 *  export        libcaca export test program
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "examples/export.c"
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

define('WIDTH', 80);
define('HEIGHT', 32);

$pixels = imagecreatetruecolor(256, 256);

$exports = caca_get_export_list();

if($argc < 2 || $argc > 3)
{
	$msg = ($argv[0] . ": wrong argument count\n" .
			"usage: " . $argv[0] . " [file] <format>\n" .
			"where <format> is one of:\n");
	foreach($exports as $format => $name)
		$msg .= " \"" . $name . "\" (" . $format . ")\n";
	die($msg);
}

if($argc == 2)
{
	$file = NULL;
	$format = $argv[1];
}
else
{
	$file = $argv[1];
	$format = $argv[2];
}

if(! array_key_exists($format, $exports))
{
	$msg = ($argv[0] . ": unknown format `" . $format . "'\n" .
			"please use one of:\n");
	foreach($exports as $format => $name)
		$msg .= " \"" . $name . "\" (" . $format . ")\n";
	die($msg);
}

if($file)
{
	$cv = caca_create_canvas(0, 0);
	if(caca_import_file($cv, $file, "") < 0)
	{
		die($argv[0] . ": `" . $file . "' has unknown format\n");
	}
}
else
{
	$cv = caca_create_canvas(WIDTH, HEIGHT);

	for($y = 0; $y < 256; $y++)
	{
		for($x = 0; $x < 256; $x++)
		{
			$r = $x;
			$g = (255 - $y + $x) / 2;
			$b = $y * (255 - $x) / 256;
			imagesetpixel($pixels, $x, $y, imagecolorallocate($pixels, $r, $g, $b));
		}
	}

	$dither = caca_create_dither($pixels);
	if(($format == "ansi") || ($format == "utf8"))
		caca_set_dither_charset($dither, "shades");
	caca_dither_bitmap($cv, 0, 0, caca_get_canvas_width($cv),
   					caca_get_canvas_height($cv), $dither, $pixels);

	caca_set_color_ansi($cv, CACA_WHITE, CACA_BLACK);
	caca_draw_thin_box($cv, 0, 0, WIDTH - 1, HEIGHT - 1);

	caca_set_color_ansi($cv, CACA_BLACK, CACA_WHITE);
	caca_fill_ellipse($cv, WIDTH / 2, HEIGHT / 2,
  					WIDTH / 4, HEIGHT / 4, ord(' '));

	caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
	caca_put_str($cv, WIDTH / 2 - 12, HEIGHT / 2 - 6,
 				"   lightgray on black   ");
	caca_set_color_ansi($cv, CACA_DEFAULT, CACA_TRANSPARENT);
	caca_put_str($cv, WIDTH / 2 - 12, HEIGHT / 2 - 5,
 				" default on transparent ");
	caca_set_color_ansi($cv, CACA_BLACK, CACA_WHITE);
	caca_put_str($cv, WIDTH / 2 - 12, HEIGHT / 2 - 4,
 				"     black on white     ");

	caca_set_color_ansi($cv, CACA_BLACK, CACA_WHITE);
	caca_put_str($cv, WIDTH / 2 - 8, HEIGHT / 2 - 3, "[<><><><> <>--<>]");
	caca_put_str($cv, WIDTH / 2 - 8, HEIGHT / 2 - 2, "[ドラゴン ボーレ]");
	caca_put_str($cv, WIDTH / 2 - 7, HEIGHT / 2 + 2, "äβç ░▒▓█▓▒░ ΔЗҒ");
	caca_put_str($cv, WIDTH / 2 - 5, HEIGHT / 2 + 4, "(\") \\o/ <&>");

	caca_set_attr($cv, CACA_BOLD, CACA_DEFAULT);
	caca_put_str($cv, WIDTH / 2 - 16, HEIGHT / 2 + 3, "Bold");
	caca_set_attr($cv, CACA_BLINK, CACA_DEFAULT);
	caca_put_str($cv, WIDTH / 2 - 9, HEIGHT / 2 + 3, "Blink");
	caca_set_attr($cv, CACA_ITALICS, CACA_DEFAULT);
	caca_put_str($cv, WIDTH / 2 - 1, HEIGHT / 2 + 3, "Italics");
	caca_set_attr($cv, CACA_UNDERLINE, CACA_DEFAULT);
	caca_put_str($cv, WIDTH / 2 + 8, HEIGHT / 2 + 3, "Underline");
	caca_set_attr($cv, 0, CACA_DEFAULT);

	caca_set_color_ansi($cv, CACA_WHITE, CACA_LIGHTBLUE);
	caca_put_str($cv, WIDTH / 2 - 7, HEIGHT / 2, "    LIBCACA    ");

	for($x = 0; $x < 16; $x++)
	{
		caca_set_color_argb($cv, 0xff00 | $x, 0xf00f | ($x << 4));
		caca_put_char($cv, WIDTH / 2 - 7 + $x, HEIGHT / 2 + 6, ord('#'));
	}
}

echo caca_export_string($cv, $format);

?>
