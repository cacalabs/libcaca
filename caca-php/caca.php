<?php
/*
 *  php-caca      Php binding for Libcaca
 *  caca.php      object layer for caca-php
 *  Copyright (c) 2008 Vion Nicolas <nico@picapo.net>
 *
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */


class Canvas {
	private $cv;

	function importFile($path, $codec) {
		return caca_import_file($this->cv, $path, $codec);
	}

	function importString($codec) {
		return caca_import_string($this->cv, $codec);
	}

	function exportString($codec) {
		return caca_export_string($this->cv, $codec);
	}

	function freeFrame($id) {
		return caca_free_frame($this->cv, $id);
	}

	function frameCount() {
		return caca_get_frame_count($this->cv);
	}

	function createFrame($id) {
		return caca_create_frame($this->cv, $id);
	}

	function setFrameName($name) {
		return caca_set_frame_name($this->cv, $name);
	}

	function setFrame($id) {
		return caca_set_frame($this->cv, $id);
	}

	function putFigchar($char) {
		return caca_put_figchar($this->cv, $char);
	}

	function setFigfont($path) {
		return caca_canvas_set_figfont($this->cv, $path);
	}

	function putAttr($attr) {
		return caca_put_attr($this->cv, $attr); 
	}

	function stretchRight() {
		return caca_stretch_right($this->cv); 
	}

	function stretchLeft() {
		return caca_stretch_left($this->cv); 
	}

	function setBoundaries($width, $height) {
		return caca_set_canvas_boundaries($this->cv, $width, $height);
	}

	function setHandle($x, $y) {
		return caca_set_canvas_handle($this->cv, $x, $y);
	}

	function getHandleX() {
		return caca_get_canvas_handle_x($this->cv); 
	}

	function getHandleY() {
		return caca_get_canvas_handle_y($this->cv); 
	}

	function getCursorX() {
		return caca_get_cursor_x($this->cv); 
	}

	function getCursorY() {
		return caca_get_cursor_y($this->cv); 
	}

	function getChars() {
		return caca_get_canvas_chars($this->cv); 
	}

	function getAttrs() {
		return caca_get_canvas_attrs($this->cv); 
	}

	function setSize($width, $height) {
		return caca_set_canvas_size($this->cv, $width, $height);
	}

	function getWidth() {
		return caca_get_canvas_width($this->cv);
	}

	function getHeight() {
		return caca_get_canvas_height($this->cv);
	}
	
	function getAttr($x, $y) {
		return caca_get_attr($this->cv, $x, $y);
	}

	function setAttr($attr) {
		return caca_set_attr($this->cv, $x, $y, $attr);
	}

	function setColorANSI($foreground, $background) {
		return caca_set_color_ansi($this->cv, $foreground, $background);
	}

	function setColorARGB($foreground, $background) {
		return caca_set_color_argb($this->cv, $foreground, $background);
	}

	function putChar($x, $y, $c) {
		return caca_put_char($this->cv, $x, $y, $c);
	}

	function getChar($x, $y) {
		return caca_get_char($this->cv, $x, $y);
	}

	function putStr($x, $y, $str) {
		return caca_put_str($this->cv, $x, $y, $str);
	}

	function Clear() {
		return caca_clear_canvas($this->cv);
	}

	function Blit($x, $y, $canvas, $mask = false) {
		return caca_blit($this->cv, $x, $y, $canvas->get_resource(), ($mask != false) ? $mask->get_resource() : false );
	}

	function Invert() {
		return caca_invert($this->cv);
	}

	function Flip() {
		return caca_flip($this->cv);
	}

	function Flop() {
		return caca_flop($this->cv);
	}

	function Rotate180() {
		return caca_rotate_180($this->cv);
	}

	function RotateLeft() {
		return caca_rotate_left($this->cv);
	}

	function RotateRight() {
		return caca_rotate_right($this->cv);
	}

	function drawLine($x1, $y1, $x2, $y2, $char) {
		return caca_draw_line($this->cv, $x1, $y1, $x2, $y2, $color);
	}

	function drawPolyline($points, $char) {
		return caca_draw_polyline($this->cv, $points, $char);
	}

	function drawThinLine($x1, $y1, $x2, $y2) {
		return caca_draw_thin_line($this->cv, $x1, $y1, $x2, $y2);
	}

	function drawThinPolyline($points) {
		return caca_draw_thin_polyline($this->cv, $points);
	}

	function drawCircle($x, $y, $radius, $char) {
		return caca_draw_circle($this->cv, $x, $y, $radius, $char);
	}

	function drawEllipse($x1, $y1, $x2, $y2, $char) {
		return caca_draw_ellipse($this->cv, $x1, $y1, $x2, $y2, $char);	
	}

	function drawThinEllipse($x1, $y1, $x2, $y2) {
		return caca_draw_thin_ellipse($this->cv, $x1, $y1, $x2, $y2);	
	}

	function fillEllipse($x1, $y1, $x2, $y2, $char) {
		return caca_fill_ellipse($this->cv, $x1, $y1, $x2, $y2, $char);	
	}

	function drawBox($x1, $y1, $x2, $y2, $char) {
		return caca_draw_box($this->cv, $x1, $y1, $x2, $y2, $char);
	}

	function drawThinBox($x1, $y1, $x2, $y2) {
		return caca_draw_thin_box($this->cv, $x1, $y1, $x2, $y2);
	}

	function drawCP437Box($x1, $y1, $x2, $y2) {
		return caca_draw_cp437_box($this->cv, $x1, $y1, $x2, $y2);
	}

	function fillBox($x1, $y1, $x2, $y2, $char) {
		return caca_fill_box($this->cv, $x1, $y1, $x2, $y2, $char);
	}

	function drawTriangle($x1, $y1, $x2, $y2, $x3, $y3, $char) {
		return caca_draw_triangle($this->cv, $x1, $y1, $x2, $y2, $x3, $y3, $char);
	}

	function drawThinTriangle($x1, $y1, $x2, $y2, $x3, $y3) {
		return caca_draw_thin_triangle($this->cv, $x1, $y1, $x2, $y2, $x3, $y3);
	}

	function fillTriangle($x1, $y1, $x2, $y2, $x3, $y3, $char) {
		return caca_fill_triangle($this->cv, $x1, $y1, $x2, $y2, $x3, $y3, $char);
	}

	function __construct($width = 0, $height = 0) {
		$this->cv = caca_create_canvas($width, $height);
	}
	
	function get_resource() {
		return $this->cv;
	}
}

class Display {
	private $dp;

	function setCursor($visible) {
		return caca_set_cursor($this->dp, $visible);
	}

	function refresh() {
		return caca_refresh_display($this->dp);
	}

	function getDriver() {
		return caca_get_display_driver($this->dp);
	}

	function setDriver($name) {
		return caca_set_display_driver($this->dp, $name);
	}

	function setDisplayTime($time) {
		return caca_set_display_time($this->dp, $time);
	}

	function getDisplayTime() {
		return caca_get_display_time($this->dp);
	}

	function getWidth() {
		return caca_get_display_width($this->dp);
	}

	function getHeight() {
		return caca_get_display_height($this->dp);
	}

	function setTitle($title) {
		return caca_set_display_title($this->dp, $title);
	}

	function gotoXY($x, $y) {
		return caca_gotoxy($this->dp, $x, $y);
	}

	function getMouseX() {
		return caca_get_mouse_x($this->dp);
	}

	function getMouseY() {
		return caca_get_mouse_y($this->dp);
	}

	function setMouse($state) {
		return caca_set_mouse($this->dp, $state);
	}

	function __construct($canvas, $driver = false) {
		if ($driver)
			$this->dp = caca_create_display_with_driver($canvas->get_resource(), $driver);
		else
			$this->dp = caca_create_display($canvas->get_resource());
	} 

	function get_resource() {
		return $this->dp;
	}
}

class Dither {
	private $dt;
	private $img;

	function setPalette($colors) {
		return caca_set_dither_palette($this->dt, $colors);
	}

	function setBrightness($value) {
		return caca_set_dither_brightness($this->dt, $value);
	}

	function getBrightness() {
		return caca_get_dither_brightness($this->dt);
	}
	
	function setGamme($value) {
		return caca_set_dither_gamma($this->dt, $value);
	}
	
	function getGamma() {
		return caca_get_dither_gamma($this->dt);
	}
	
	function setContrast($value) {
		return caca_set_dither_contrast($this->dt, $value);
	}
	
	function getContrast() {
		return caca_get_dither_contrast($this->dt);
	}
	
	function setAntialias($value) {
		return caca_set_dither_antialias($this->dt, $value);
	}
	
	function getAntialiasList() {
		return caca_get_dither_antialias_list($this->dt);
	}
	
	function getAntialias() {
		return caca_get_dither_antialias($this->dt);
	}
	
	function setColor($color) {
		return caca_set_dither_color($this->dt, $color);
	}
	
	function getColorList() {
		return caca_get_dither_color_list($this->dt);
	}
	
	function getColor() {
		return caca_get_dither_color($this->dt);
	}
	
	function setCharset($value) {
		return caca_set_dither_charset($this->dt, $value);
	}
	
	function getCharsetList() {
		return caca_get_dither_charset_list($this->dt);
	}
	
	function getCharset() {
		return caca_get_dither_charset($this->dt);
	}
	
	function setAlgorithm($name) {
		return caca_set_dither_algorithm($this->dt, $name);
	}
	
	function getAlgorithmList() {
		return caca_get_dither_algorithm_list($this->dt);
	}
	
	function getAlgorithm() {
		return caca_get_dither_algorithm($this->dt);
	}
	
	function bitmap($canvas, $x, $y, $width, $height, $load_palette = true) {
		return caca_dither_bitmap($canvas, $x, $y, $width, $height, $this->dt, $this->img, $load_palette);
	}

	function __construct($image) {
		$this->dt = caca_create_dither($image);
		$this->img = $image;
	} 
}

class Font {
	private $f;
	
	function getWidth() {
		return caca_get_font_width($this->f);
	}

	function getHeight() {
		return caca_get_font_height($this->f);
	}

	function getBlocks() {
		return caca_get_font_blocks($this->f);
	}

	function __construct($name) {
		$this->f = caca_load_builtin_font($name);
	} 
}
