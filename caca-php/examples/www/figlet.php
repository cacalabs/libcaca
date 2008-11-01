<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<title>Caca power!</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<style type="text/css">
</style>
</head>
<body>
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

function show_figlet($str, $font) {
	$cv = caca_create_canvas(0, 0);

	if (!caca_canvas_set_figfont($cv, $font)) {
		return false;
	}
	
	$chars = unistr_to_ords($str);
	$color = 0;
	foreach ($chars as $c) {
		caca_set_color_ansi($cv, 1 + (($color += 4) % 15), CACA_WHITE);
		caca_put_figchar($cv, $c);
	}

	echo caca_export_string($cv, "html3");
}

$dir = opendir($path = "/usr/share/figlet/");
while (($it = readdir($dir)) != false) {
	if (is_file($path.$it) and ereg("\.[tf]lf$", $it)) {
		echo "<b>font : $it</b>\n<pre>";
		show_figlet("Libcaca", $path.$it);
		echo "</pre>";
	}
}
?>
</body>
</html>
