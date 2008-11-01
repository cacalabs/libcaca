#!/usr/bin/php5
<?php
/*
 *  figfont.php      sample program for libcaca php binding
 *  Copyright (c) 2008 Nicolas Vion <nico@yojik.eu>
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

function unistr_to_ords($str, $encoding = 'UTF-8'){       
	$str = mb_convert_encoding($str, "UCS-4BE", $encoding);
	$result = array();

	for ($i = 0; $i < mb_strlen($str, "UCS-4BE"); $i++){       
		$c = mb_substr($str, $i, 1, "UCS-4BE");                   
		$val = unpack("N", $c);           
		$result[] = $val[1];               
	}       
	return $result;
}

if (php_sapi_name() != "cli") {
	die("You have to run this program with php-cli!\n");
}

if ($argc < 3) {
	die("Too few arguments.\nUsage: cmd <path of font> <utf8 string>\n");
}

$cv = caca_create_canvas(0, 0);

if (!caca_canvas_set_figfont($cv, $argv[1])) {
	die("Could not open font\n");
}

$chars = unistr_to_ords($argv[2]);
$color = 0;
foreach ($chars as $c) {
	caca_set_color_ansi($cv, 1 + (($color += 4) % 15), CACA_TRANSPARENT);
	caca_put_figchar($cv, $c);
}

echo caca_export_string($cv, "utf8");

?>
