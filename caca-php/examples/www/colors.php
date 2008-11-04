<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<?php
  /*
   *  colors        display all possible libcaca colour pairs
   *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
   *
   *  This file is a Php port of "examples/colors.c"
   *  Copyright (c) 2003-2004 Sam Hocevar <sam@zoy.org>
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
?>
<head>
<title>display all possible libcaca colour pairs</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
</head>
<body text="silver" bgcolor="black">
<?php

$cv = caca_create_canvas(80, 24);
if(!$cv)
{
	die("Failed to create canvas\n");
}

caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
caca_clear_canvas($cv);
for($i = 0; $i < 16; $i++)
{
	caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
	caca_put_str($cv, 3, $i + ($i >= 8 ? 3 : 2), "ANSI " . $i);
	for($j = 0; $j < 16; $j++)
	{
		caca_set_color_ansi($cv, $i, $j);
		caca_put_str($cv, ($j >= 8 ? 13 : 12) + $j * 4, $i + ($i >= 8 ? 3 : 2),
					"Aaホ");
	}
}

caca_set_color_ansi($cv, CACA_LIGHTGRAY, CACA_BLACK);
caca_put_str($cv, 3, 20, "This is bold    This is blink    This is italics    This is underline");
caca_set_attr($cv, CACA_BOLD);
caca_put_str($cv, 3 + 8, 20, "bold");
caca_set_attr($cv, CACA_BLINK);
caca_put_str($cv, 3 + 24, 20, "blink");
caca_set_attr($cv, CACA_ITALICS);
caca_put_str($cv, 3 + 41, 20, "italics");
caca_set_attr($cv, CACA_UNDERLINE);
caca_put_str($cv, 3 + 60, 20, "underline");

echo caca_export_string($cv, "html3");

?>
</body>
</html>
