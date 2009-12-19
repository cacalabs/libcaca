<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<?php
/*
 *  fullwidth     libcaca fullwidth Unicode characters test program
 *  Copyright (c) 2008 Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  This file is a Php port of "examples/fullwidth.c"
 *  which is: 
 *  Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
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
<title>sample program for libcaca php binding</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
</head>
<body text="silver" bgcolor="black">
<?php

define('CACA', "쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊쫊");

$cv = caca_create_canvas(36, 12);
if(! $cv)
{
	die("Can't created canvas\n");
}

$caca = caca_create_canvas(6, 10);
$line = caca_create_canvas(2, 1);

/* Line of x's */
for($i = 0; $i < 10; $i++)
{
	caca_set_color_ansi($caca, CACA_WHITE, CACA_BLUE);
	caca_put_str($caca, 0, $i, CACA);
	caca_set_color_ansi($caca, CACA_WHITE, CACA_RED);
	caca_put_char($caca, $i - 2, $i, ord('x'));
}

caca_blit($cv, 1, 1, $caca);

/* Line of ホ's */
for($i = 0; $i < 10; $i++)
{
	caca_set_color_ansi($caca, CACA_WHITE, CACA_BLUE);
	caca_put_str($caca, 0, $i, CACA);
	caca_set_color_ansi($caca, CACA_WHITE, CACA_GREEN);
	caca_put_str($caca, $i - 2, $i, "ホ");
}

caca_blit($cv, 15, 1, $caca);

/* Line of canvas */
caca_set_color_ansi($line, CACA_WHITE, CACA_MAGENTA);
caca_put_str($line, 0, 0, "ほ");
for($i = 0; $i < 10; $i++)
{
	caca_set_color_ansi($caca, CACA_WHITE, CACA_BLUE);
	caca_put_str($caca, 0, $i, CACA);
	caca_blit($caca, $i - 2, $i, $line);
}

caca_blit($cv, 29, 1, $caca);

echo caca_export_string($cv, "html3");

?>
</body>
</html>
