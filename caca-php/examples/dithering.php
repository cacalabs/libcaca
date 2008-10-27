#!/usr/bin/php5
<?
$img = imagecreatefrompng(dirname(__FILE__)."/logo-caca.png");
$dither = caca_create_dither_gd($img);
if (!$dither) 
	die("Can not create dither. Maybe this image is not truecolor.\n");

$canvas = caca_create_canvas(0, 0);
$display = caca_create_display($canvas);
if (!$display) 
	die("Can not create display.\n");

caca_dither_bitmap_gd($canvas, 0, 0, caca_get_canvas_width($canvas), caca_get_canvas_height($canvas), $dither, $img);
caca_refresh_display($display);

sleep(5);




