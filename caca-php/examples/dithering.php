#!/usr/bin/php5
<?
$img = imagecreatefrompng(dirname(__FILE__)."/logo-caca.png");
//$img = imagecreatefromjpeg("/home/nicolas/images/Clown Fish-mini.jpg");
$canvas = caca_create_canvas(0, 0);

$dither = caca_create_dither(16, 128, 128, 3 * 256, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
caca_set_dither_gamma($dither, -1.0);

if (caca_dither_bitmap_gd($canvas, 0, 0, 30, 30, $dither, $img)) {
	$dp = caca_create_display($canvas);
	caca_refresh_display($dp);
	sleep(5);
}

