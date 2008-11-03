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

define('WIDTH', 80);
define('HEIGHT', 32);

$pixels = imagecreatetruecolor(256, 256);

$exports = caca_get_export_list();

$file = isset($_FILES['file']) ? $_FILES['file']['tmp_name'] : NULL;
$format = isset($_REQUEST['format']) ? $_REQUEST['format'] : NULL;

if((! $format) || (! array_key_exists($format, $exports)))
{
	header("Content-type: text/html; charset=UTF-8");

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>libcaca export test program</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
</head>
<body>

<form action="#" enctype="multipart/form-data" method="post">
<label for="file">File:</label>
<input name="file" type="file" /> <em>(optional)</em>
<br />
<input type="submit" value="Export" />
<label for="format">as</label>
<select name="format" id="format">
<?php
	foreach($exports as $format => $name)
	{
		?><option value="<?= htmlspecialchars($format) ?>"<?=
			($format == 'html') ? ' selected="selected"' : '' ?>><?=
			htmlspecialchars($name . " (" . $format . ")") ?></option><?php
	}
?>
</select>
</form>
</body>
</html>
<?php
	exit(0);
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

$content_type_map = array(
	'ansi' => 'text/plain; charset=CP437',
	'utf8' => 'text/plain; charset=UTF-8',
	'utf8cr' => 'text/plain; charset=UTF-8',
	'html' => 'text/html; charset=UTF-8',
	'html3' => 'text/html; charset=UTF-8',
	'bbfr' => 'text/plain; charset=UTF-8',
	'irc' => 'text/plain; charset=UTF-8',
	'ps' => 'application/postscript',
	'svg' => 'image/svg+xml',
	'tga' => 'image/x-targa'
	);

$download_extension_map = array(
	'caca' => 'caca',
	'ansi' => 'txt',
	'utf8' => 'txt',
	'utf8cr' => 'txt',
	'irc' => 'txt',
	'tga' => 'tga'
	);

$inline_extension_map = array(
	'bbfr' => 'txt',
	'ps' => 'ps',
	'svg' => 'svg'
	);

if (! array_key_exists($format, $content_type_map))
	$content_type = 'application/octet-stream';
else
	$content_type = $content_type_map[$format];

header('Content-Type: ' . $content_type);
if (array_key_exists($format, $download_extension_map))
	header('Content-Disposition: attachment; filename=export.' . $download_extension_map[$format]);
else if (array_key_exists($format, $inline_extension_map))
	header('Content-Disposition: inline; filename=export.' . $inline_extension_map[$format]);

echo caca_export_string($cv, $format);

?>
