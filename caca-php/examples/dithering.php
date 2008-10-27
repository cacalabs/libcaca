#!/usr/bin/php5
<?
$img = imagecreatefrompng(dirname(__FILE__)."/logo-caca.png");
$canvas = caca_create_canvas(0, 0);

$dither = caca_create_dither(32, 128, 128, 4 * 128, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);

$display = caca_create_display($canvas);
caca_set_display_time($display, 80000);

caca_dither_bitmap_gd($canvas, 0, 0, caca_get_canvas_width($canvas), caca_get_canvas_height($canvas), $dither, $img);
caca_refresh_display($display);

sleep(5);




