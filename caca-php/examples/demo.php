#!/usr/bin/php5
<?

function display_menu($cv, $dp) {
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
	caca_put_str($cv, 4, 17, "'o': outline: ".((outline == 0) ? "none" : ((outline == 1) ? "solid" : "thin")));
	caca_put_str($cv, 4, 18, "'b': drawing boundaries: ".((bounds == 0) ? "screen" : "infinite"));
	caca_put_str($cv, 4, $yo - 2, "'q': quit");

	caca_refresh_display($dp);
}

function main() {
	$cv = caca_create_canvas(0, 0);
	if (!$cv) {
		die("Error while creating canvas\n");
	}

	$dp = caca_create_display($cv);
	if (!$dp) {
		die("Error while attaching canvas to display\n");
	}

	display_menu($cv, $dp);

	caca_set_display_time($dp, 40000);

	/* Disable cursor */
	caca_set_mouse($dp, 0);

	/* Main menu */
	display_menu($cv, $dp);
	caca_refresh_display($dp);

	sleep(1);

	for ($i = 0; $i < 100; $i++) {
		demo_lines($cv, true, 2);
		caca_refresh_display($dp);
	}

}

function demo_lines($cv, $bounds = true, $outline = 1) {
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
		caca_draw_line($cv, $xa, $ya, $xb, $yb, '#');
}


main();
