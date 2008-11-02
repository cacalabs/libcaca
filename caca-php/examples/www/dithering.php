<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

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
 *  dithering.php      sample program for libcaca php binding
 *  Copyright (c) 2008 Nicolas Vion <nico@yojik.eu>
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

$src = "logo-caca.png";
$img = imagecreatefrompng(dirname(__FILE__)."/".$src);
if (!$img)  {
	die("Can not open image.\n");
}

$dither = caca_create_dither($img);
if (!$dither)  {
	die("Can not create dither. Maybe you compiled caca-php without gd support.\n");
}

$canvas = caca_create_canvas(100, 40);

caca_dither_bitmap($canvas, 0, 0, caca_get_canvas_width($canvas), caca_get_canvas_height($canvas), $dither, $img);

echo caca_export_string($canvas, "html3");
echo "<img src='./$src' alt=''/>";

?>
</body>
</html>

