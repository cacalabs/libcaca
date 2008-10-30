#!/usr/bin/php5
<?php
$img = imagecreatefrompng(dirname(__FILE__)."/logo-caca.png");
//$img = imagecreatefromgif(dirname(__FILE__)."/logo-caca-idx.gif");
if (!$img) 
	die("Can not open image.\n");

$dither = caca_create_dither($img);
if (!$dither) 
	die("Can not create dither. Maybe this image is not truecolor.\n");

$canvas = caca_create_canvas(0, 0);
$display = caca_create_display($canvas);
if (!$display) 
	die("Can not create display.\n");

caca_dither_bitmap($canvas, 0, 0, caca_get_canvas_width($canvas), caca_get_canvas_height($canvas), $dither, $img);
caca_refresh_display($display);

sleep(5);

?>
