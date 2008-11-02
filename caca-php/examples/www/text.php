<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
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
?>
<head>
<title>demo for libcaca php binding</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<link rel="StyleSheet" href="caca-php.css" type="text/css" />
</head>
<body text="silver" bgcolor="black">
<?php

$string =  <<<EOT
              |_| 
   _,----._   | | 
  (/ @  @ \)   __ 
   |  OO  |   |_  
   \ `--' /   |__ 
    `----'        
              |_| 
 Hello world!  |  
EOT;
        
$pig = caca_create_canvas(0, 0);
caca_import_string($pig, $string, "text");

$cv = caca_create_canvas(caca_get_canvas_width($pig) * 2, caca_get_canvas_height($pig) * 2);

if (!$cv or !$pig) {
	die("Can't created canvas\n");
}

caca_blit($cv, 0, 0, $pig);
caca_flip($pig);
caca_blit($cv, caca_get_canvas_width($pig), 0, $pig);
caca_flip($pig);
caca_flop($pig);
caca_blit($cv, 0, caca_get_canvas_height($pig), $pig);
caca_flop($pig);
caca_rotate_180($pig);
caca_blit($cv, caca_get_canvas_width($pig), caca_get_canvas_height($pig), $pig);

for($j = 0; $j < caca_get_canvas_height($cv); $j++) {
	for($i = 0; $i < caca_get_canvas_width($cv); $i += 2) {
		caca_set_color_ansi($cv, CACA_LIGHTBLUE + ($i + $j) % 6, CACA_DEFAULT);
		$a = caca_get_attr($cv, -1, -1);
		caca_put_attr($cv, $i, $j, $a);
		caca_put_attr($cv, $i + 1, $j, $a);
	}
}

echo caca_export_string($cv, "html3");
caca_rotate_left($cv);
echo caca_export_string($cv, "html3");

?>
</body>
</html>
