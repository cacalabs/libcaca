#!/usr/bin/php5
<?php

if ($argc < 3) {
	die("Too few arguments.\nUsage: cmd <path of font> <string>\n");
}

$cv = caca_create_canvas(0, 0);

if (!caca_canvas_set_figfont($cv, $argv[1])) {
	die("Could not open font\n");
}

$str = $argv[2];
$color = 0;
for ($i = 0; $i < strlen($str); $i++) {
	caca_set_color_ansi($cv, 1 + (($color += 4) % 15), CACA_TRANSPARENT);
	caca_put_figchar($cv, ord($str[$i]));
}

echo caca_export_string($cv, "utf8");



