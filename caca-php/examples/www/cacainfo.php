<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<?php
/*
 *  cacainfo.php      sample program for libcaca php binding
 *  Copyright (c) 2008 Nicolas Vion <nico@yojik.eu>
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */
?>
<head>
<title>sample program for libcaca php binding</title>
<link rel="StyleSheet" href="caca-php.css" type="text/css" />
</head>
<body text="silver" bgcolor="black">
<?php

//--- Just for fun ---//

function just_for_fun() {

$moo = <<<EOT
	 (__)  
	 (oo) 
   /------\/ 
  / |    ||   
 *  /\---/\ 
    ~~   ~~   
EOT;

	$cv = caca_create_canvas(0, 0);
	caca_set_color_ansi($cv, CACA_LIGHTBLUE, CACA_DEFAULT);
	caca_import_string($cv, $moo, "text");

	for($j = 0; $j < caca_get_canvas_height($cv); $j++) {
		for($i = 0; $i < caca_get_canvas_width($cv); $i += 2) {
			caca_set_color_ansi($cv, (caca_rand(1, 10) > 5 ? CACA_LIGHTBLUE  : CACA_WHITE), CACA_DEFAULT);
			$a = caca_get_attr($cv, -1, -1);
			caca_put_attr($cv, $i, $j, $a);
			caca_put_attr($cv, $i + 1, $j, $a);
		}
	}
	caca_set_color_ansi($cv, CACA_LIGHTGREEN, CACA_DEFAULT);
	caca_put_str($cv, 8, 0, "Moo!");
	caca_set_color_ansi($cv, CACA_LIGHTRED, CACA_DEFAULT);
	caca_put_char($cv, 8, 1, hexdec("2765")); //U+2765
	caca_put_char($cv, 10, 1, hexdec("2764")); //U+2764
	echo caca_export_string($cv, "html3");
}
	

just_for_fun();

?>
<?php

//--- Show caca's information ---//

?>
<p>libcaca version: <?php echo htmlspecialchars(caca_get_version()); ?></p>

<p>Available drivers:</p>
<ul>
<?php
$list = caca_get_display_driver_list();
foreach($list as $type => $name)
	echo '<li>' . htmlspecialchars("$name ($type)") . '</li>';
?>
</ul>

<p>Available import formats:</p>
<ul>
<?php
$list = caca_get_import_list();
foreach($list as $format => $name)
	echo '<li>' . htmlspecialchars("$name ($format)") . '</li>';
?>
</ul>

<p>Available export formats:</p>
<ul>
<?php
$list = caca_get_export_list();
foreach($list as $format => $name)
	echo '<li>' . htmlspecialchars("$name ($format)") . '</li>';
?>
</ul>

<p>Available caca fonts:</p>
<ul>
<?php
$list = caca_get_font_list();
foreach($list as $name)
	echo '<li>' . htmlspecialchars("$name") . '</li>';
?>
</ul>

</body>
</html>