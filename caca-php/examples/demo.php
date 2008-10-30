#!/usr/bin/php5
<?php
/*
 *  demo.php      demo for libcaca php binding
 *  Copyright (c) 2008 Nicolas Vion <nico@yojik.eu>
 *
 *  This file is a Php port of the official libcaca's sample program "demo.c" 
 *  which is: 
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

function main() {
	$cv = caca_create_canvas(0, 0);
	if (!$cv) {
		die("Error while creating canvas\n");
	}

	$dp = caca_create_display($cv);
	if (!$dp) {
		die("Error while attaching canvas to display\n");
	}

	caca_set_display_time($dp, 40000);

	/* Disable X cursor */
	caca_set_mouse($dp, 0);

	/* Main menu */
	display_menu($cv, $dp, $bounds, $outline);

	/* Go ! */
	$bounds = $outline = $dithering = 0;
	$need_refresh = false;
	while(!$quit) {
		while ($ev = caca_get_event($dp, CACA_EVENT_ANY, 1000)) {
			if (caca_get_event_type($ev) & CACA_EVENT_KEY_PRESS) {
				switch (caca_get_event_key_ch($ev)) {
					case ord('q'):
					case ord('Q'):
					case CACA_KEY_ESCAPE:
						$demo = false;
						$quit = 1;
						break;
					case ord('o'):
					case ord('O'):
						$outline = ($outline + 1) % 3;
						display_menu($cv, $dp, $bounds, $outline);
						break;
					case ord('b'):
					case ord('B'):
						$bounds = ($bounds + 1) % 2;
						display_menu($cv, $dp, $bounds, $outline);
						break;
					case ord('d'):
					case ord('D'):
						$dithering = ($dithering + 1) % 5;
						caca_set_feature($cv, $dithering);
						display_menu($cv, $dp, $bounds, $outline);
						break;
					case ord('f'):
					case ord('F'):
						demo_go($dp, "demo_all", $cv, $bounds, $outline);
						break;
					case ord('1'):
						demo_go($dp, "demo_dots", $cv, $bounds, $outline);
						break;
					case ord('2'):
						demo_go($dp, "demo_lines", $cv, $bounds, $outline);
						break;
					case ord('3'):
						demo_go($dp, "demo_boxes", $cv, $bounds, $outline);
						break;
					case ord('4'):
						demo_go($dp, "demo_triangles", $cv, $bounds, $outline);
						break;
					case ord('5'):
						demo_go($dp, "demo_ellipses", $cv, $bounds, $outline);
						break;
					case ord('s'):
					case ord('S'):
						demo_go($dp, "demo_sprites", $cv, $bounds, $outline);
						break;
					case ord('r'):
					case ord('R'):
						demo_go($dp, "demo_render", $cv, $bounds, $outline);
						break;
				}
			}
			else if(caca_get_event_type($ev) & CACA_EVENT_MOUSE_MOTION) {
				$x = caca_get_event_mouse_x($ev);
				$y = caca_get_event_mouse_y($ev);
				display_menu($cv, $dp, $bounds, $outline, false);
				caca_set_color_ansi($cv, CACA_RED, CACA_BLACK);
				caca_put_str($cv, $x, $y, ".");
				caca_put_str($cv, $x, $y + 1, "|\\");
				$need_refresh = true;
			}
			else if(caca_get_event_type($ev) & CACA_EVENT_RESIZE) {
				display_menu($cv, $dp, $bounds, $outline);
			}
		}

		if ($need_refresh) {
			caca_refresh_display($dp);
			$need_refresh = false;
		}
	}
}

function display_menu($cv, $dp, $bounds, $outline, $refresh = true) {
	$xo = caca_get_canvas_width($cv) - 2;
	$yo = caca_get_canvas_height($cv) - 2;

	caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
	caca_clear_canvas($cv);
	caca_draw_thin_box($cv, 1, 1, $xo, $yo);

	caca_put_str($cv, ($xo - strlen("libcaca demo")) / 2, 3, "libcaca demo");
	caca_put_str($cv, ($xo - strlen("==============")) / 2, 4, "==============");

	caca_put_str($cv, 4, 6, "demos:");
	caca_put_str($cv, 4, 7, "'f': full");
	caca_put_str($cv, 4, 8, "'1': dots");
	caca_put_str($cv, 4, 9, "'2': lines");
	caca_put_str($cv, 4, 10, "'3': boxes");
	caca_put_str($cv, 4, 11, "'4': triangles");
	caca_put_str($cv, 4, 12, "'5': ellipses");
	caca_put_str($cv, 4, 13, "'c': colour");
	caca_put_str($cv, 4, 14, "'r': render");

	if ($sprite)
		caca_put_str($cv, 4, 15, "'s': sprites");

	caca_put_str($cv, 4, 16, "settings:");
	caca_put_str($cv, 4, 17, "'o': outline: ".(($outline == 0) ? "none" : (($outline == 1) ? "solid" : "thin")));
	caca_put_str($cv, 4, 18, "'b': drawing boundaries: ".(($bounds == 0) ? "screen" : "infinite"));
	caca_put_str($cv, 4, $yo - 2, "'q': quit");

	if ($refresh)
		caca_refresh_display($dp);
}

function demo_go($dp, $demo, $cv, $bounds, $outline) {
	caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
	caca_clear_canvas($cv);

	while (!caca_get_event($dp, CACA_EVENT_KEY_PRESS)) {
		if (function_exists($demo))
			$demo($cv, $bounds, $outline);

		caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
		caca_draw_thin_box($cv, 1, 1, caca_get_canvas_width($cv) - 2, caca_get_canvas_height($cv) - 2);
		$rate = sprintf("%01.2f", 1000000 / caca_get_display_time($dp));
		caca_put_str($cv, 4, 1, "[$rate fps]----");
		caca_refresh_display($dp);
	}

	display_menu($cv, $dp, $bounds, $outline);
	caca_refresh_display($dp);
}


function demo_all($cv, $bounds, $outline) {
	global $i;

	$i++;

	caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
	caca_clear_canvas($cv);

	/* Draw the sun */
	caca_set_color_ansi($cv, CACA_YELLOW, CACA_BLACK);
	$xo = caca_get_canvas_width($cv) / 4;
	$yo = caca_get_canvas_height($cv) / 4 + 5 * sin(0.03 * $i);

	for ($j = 0; $j < 16; $j++) {
		$xa = $xo - (30 + sin(0.03 * $i) * 8) * sin(0.03 * $i + M_PI * $j / 8);
		$ya = $yo + (15 + sin(0.03 * $i) * 4) * cos(0.03 * $i + M_PI * $j / 8);
		caca_draw_thin_line($cv, $xo, $yo, $xa, $ya);
	}

	$j = 15 + sin(0.03 * $i) * 8;
	caca_set_color_ansi($cv, CACA_WHITE, CACA_BLACK);
	caca_fill_ellipse($cv, $xo, $yo, $j, $j / 2, ord('#'));
	caca_set_color_ansi($cv, CACA_YELLOW, CACA_BLACK);
	caca_draw_ellipse($cv, $xo, $yo, $j, $j / 2, ord('#'));

	/* Draw the pyramid */
	$xo = caca_get_canvas_width($cv) * 5 / 8;
	$yo = 2;

	$xa = caca_get_canvas_width($cv) / 8 + sin(0.03 * $i) * 5;
	$ya = caca_get_canvas_height($cv) / 2 + cos(0.03 * $i) * 5;

	$xb = caca_get_canvas_width($cv) - 10 - cos(0.02 * $i) * 10;
	$yb = caca_get_canvas_height($cv) * 3 / 4 - 5 + sin(0.02 * $i) * 5;

	$xc = caca_get_canvas_width($cv) / 4 - sin(0.02 * $i) * 5;
	$yc = caca_get_canvas_height($cv) * 3 / 4 + cos(0.02 * $i) * 5;

	caca_set_color_ansi($cv, CACA_GREEN, CACA_BLACK);
	caca_fill_triangle($cv, $xo, $yo, $xb, $yb, $xa, $ya, ord('%'));
	caca_set_color_ansi($cv, CACA_YELLOW, CACA_BLACK);
	caca_draw_thin_triangle($cv, $xo, $yo, $xb, $yb, $xa, $ya);

	caca_set_color_ansi($cv, CACA_RED, CACA_BLACK);
	caca_fill_triangle($cv, $xa, $ya, $xb, $yb, $xc, $yc, ord('#'));
	caca_set_color_ansi($cv, CACA_YELLOW, CACA_BLACK);
	caca_draw_thin_triangle($cv, $xa, $ya, $xb, $yb, $xc, $yc);

	caca_set_color_ansi($cv, CACA_BLUE, CACA_BLACK);
	caca_fill_triangle($cv, $xo, $yo, $xb, $yb, $xc, $yc, ord('%'));
	caca_set_color_ansi($cv, CACA_YELLOW, CACA_BLACK);
	caca_draw_thin_triangle($cv, $xo, $yo, $xb, $yb, $xc, $yc);

	/* Draw a background triangle */
	$xa = 2;
	$ya = 2;

	$xb = caca_get_canvas_width($cv) - 3;
	$yb = caca_get_canvas_height($cv) / 2;

	$xc = caca_get_canvas_width($cv) / 3;
	$yc = caca_get_canvas_height($cv) - 3;

	caca_set_color_ansi($cv, CACA_CYAN, CACA_BLACK);
	caca_draw_thin_triangle($cv, $xa, $ya, $xb, $yb, $xc, $yc);

	$xo = caca_get_canvas_width($cv) / 2 + cos(0.027 * $i) * caca_get_canvas_width($cv) / 3;
	$yo = caca_get_canvas_height($cv) / 2 - sin(0.027 * $i) * caca_get_canvas_height($cv) / 2;

	caca_draw_thin_line($cv, $xa, $ya, $xo, $yo);
	caca_draw_thin_line($cv, $xb, $yb, $xo, $yo);
	caca_draw_thin_line($cv, $xc, $yc, $xo, $yo);

	/* Draw a trail behind the foreground sprite */
	for($j = $i - 60; $j < $i; $j++) {
		$delta = caca_rand(-5, 6);
		caca_set_color_ansi($cv, caca_rand(0, 16), caca_rand(0, 16));
		caca_put_char($cv, 
			caca_get_canvas_width($cv) / 2 + cos(0.02 * $j) * ($delta + caca_get_canvas_width($cv) / 4),
			caca_get_canvas_height($cv) / 2 + sin(0.02 * $j) * ($delta + caca_get_canvas_height($cv) / 3),
			ord('#'));
	}
}

function demo_dots($cv, $bounds, $outline) {
	$xmax = caca_get_canvas_width($cv);
	$ymax = caca_get_canvas_height($cv);

	$chars = array('+', '-', '*', '#', 'X', '@', '%', '$', 'M', 'W');

	for ($i = 1000; $i--;) {
		/* Putpixel */
		caca_set_color_ansi($cv, caca_rand(0, 16), caca_rand(0, 16));
		caca_put_char($cv, caca_rand(0, $xmax), caca_rand(0, $ymax), ord($chars[caca_rand(0, 9)]));
	}
}

function demo_lines($cv, $bounds, $outline) {
	$w = caca_get_canvas_width($cv);
	$h = caca_get_canvas_height($cv);

	if ($bounds) {
		$xa = caca_rand(- $w, 2 * $w); $ya = caca_rand(- $h, 2 * $h);
		$xb = caca_rand(- $w, 2 * $w); $yb = caca_rand(- $h, 2 * $h);
	}
	else {
		$xa = caca_rand(0, $w); $ya = caca_rand(0, $h);
		$xb = caca_rand(0, $w); $yb = caca_rand(0, $h);
	}

	caca_set_color_ansi($cv, caca_rand(0, 16), CACA_BLACK);
	if ($outline > 1)
		caca_draw_thin_line($cv, $xa, $ya, $xb, $yb);
	else
		caca_draw_line($cv, $xa, $ya, $xb, $yb, ord('#'));
}

function demo_boxes($cv, $bounds, $outline) {
	$w = caca_get_canvas_width($cv);
	$h = caca_get_canvas_height($cv);

	if ($bounds) {
		$xa = caca_rand(- $w, 2 * $w); $ya = caca_rand(- $h, 2 * $h);
		$xb = caca_rand(- $w, 2 * $w); $yb = caca_rand(- $h, 2 * $h);
	}
	else {
		$xa = caca_rand(0, $w); $ya = caca_rand(0, $h);
		$xb = caca_rand(0, $w); $yb = caca_rand(0, $h);
	}

	caca_set_color_ansi($cv, caca_rand(0, 16), caca_rand(0, 16));
	caca_fill_box($cv, $xa, $ya, $xb, $yb, ord('#'));

	caca_set_color_ansi($cv, caca_rand(0, 16), CACA_BLACK);
	if($outline == 2)
		caca_draw_thin_box($cv, $xa, $ya, $xb, $yb);
	else if($outline == 1)
		caca_draw_box($cv, $xa, $ya, $xb, $yb, ord('#'));
}

function demo_ellipses($cv, $bounds, $outline) {
	$w = caca_get_canvas_width($cv);
	$h = caca_get_canvas_height($cv);

	if ($bounds) {
		$x = caca_rand(- $w, 2 * $w); $y = caca_rand(- $h, 2 * $h);
		$a = caca_rand(0, $w); $b = caca_rand(0, $h);
	}
	else {
		do {
			$x = caca_rand(0, $w); $y = caca_rand(0, $h);
			$a = caca_rand(0, $w); $b = caca_rand(0, $h);
		} while ($x - $a < 0 || $x + $a >= $w || $y - $b < 0 || $y + $b >= $h);
	}

	caca_set_color_ansi($cv, caca_rand(0, 16), caca_rand(0, 16));
	caca_fill_ellipse($cv, $x, $y, $a, $b, ord('#'));

	caca_set_color_ansi($cv, caca_rand(0, 16), CACA_BLACK);
	if ($outline == 2)
		caca_draw_thin_ellipse($cv, $x, $y, $a, $b);
	else if ($outline == 1)
		caca_draw_ellipse($cv, $x, $y, $a, $b, ord('#'));
}

function demo_triangles($cv, $bounds, $outline) {
	$w = caca_get_canvas_width($cv);
	$h = caca_get_canvas_height($cv);

	if ($bounds) {
		$xa = caca_rand(- $w, 2 * $w); $ya = caca_rand(- $h, 2 * $h);
		$xb = caca_rand(- $w, 2 * $w); $yb = caca_rand(- $h, 2 * $h);
		$xc = caca_rand(- $w, 2 * $w); $yc = caca_rand(- $h, 2 * $h);
	}
	else {
		$xa = caca_rand(0, $w); $ya = caca_rand(0, $h);
		$xb = caca_rand(0, $w); $yb = caca_rand(0, $h);
		$xc = caca_rand(0, $w); $yc = caca_rand(0, $h);
	}

	caca_set_color_ansi($cv, caca_rand(0, 16), caca_rand(0, 16));
	caca_fill_triangle($cv, $xa, $ya, $xb, $yb, $xc, $yc, ord('#'));

	caca_set_color_ansi($cv, caca_rand(0, 16), CACA_BLACK);
	if ($outline == 2)
		caca_draw_thin_triangle($cv, $xa, $ya, $xb, $yb, $xc, $yc);
	else if ($outline == 1)
		caca_draw_triangle($cv, $xa, $ya, $xb, $yb, $xc, $yc, ord('#'));
}

function demo_render($cv, $bounds, $outline) {
}

main();

?>
