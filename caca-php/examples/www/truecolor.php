<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<?php
/*
 *  truecolor	  truecolor canvas features
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "examples/truecolor.c"
 *  which is: 
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *		  All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */
?>
<head>
<title>truecolor canvas features</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
</head>
<body text="silver" bgcolor="black">
<?php

$cv = caca_create_canvas(32, 16);
if(!$cv) {
	die("Failed to create canvas\n");
}

for($y = 0; $y < 16; $y++)
	for($x = 0; $x < 16; $x++)
	{
		$bgcolor = 0xff00 | ($y << 4) | $x;
		$fgcolor = 0xf000 | ((15 - $y) << 4) | ((15 - $x) << 8);
		
		caca_set_color_argb($cv, $fgcolor, $bgcolor);
		caca_put_str($cv, $x * 2, $y, "CA");
	}

caca_set_color_ansi($cv, CACA_WHITE, CACA_LIGHTBLUE);
caca_put_str($cv, 2, 1, " truecolor libcaca ");

echo caca_export_string($cv, "html3");

?>
</body>
</html>
