<?php
/*
 *  php-caca      Php binding for Libcaca
 *  caca.php      object layer for caca-php
 *  Copyright (c) 2008 Vion Nicolas <nico@picapo.net>
 *
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

class Libcaca
{
	static function Rand($min, $max)
	{
		return caca_rand($min, $max);
	}

	static function getVersion()
	{
		return caca_get_version();
	}
}

class AnsiColor
{
	const BLACK = CACA_BLACK;
	const BLUE = CACA_BLUE;
	const GREEN = CACA_GREEN;
	const CYAN = CACA_CYAN;
	const RED = CACA_RED;
	const MAGENTA = CACA_MAGENTA;
	const BROWN = CACA_BROWN;
	const LIGHTGRAY = CACA_LIGHTGRAY;
	const DARKGRAY = CACA_DARKGRAY;
	const LIGHTBLUE = CACA_LIGHTBLUE;
	const LIGHTGREEN = CACA_LIGHTGREEN;
	const LIGHTCYAN = CACA_LIGHTCYAN;
	const LIGHTRED = CACA_LIGHTRED;
	const LIGHTMAGENTA = CACA_LIGHTMAGENTA;
	const YELLOW = CACA_YELLOW;
	const WHITE = CACA_WHITE;
	/* NOTE: We can't call this one DEFAULT because that's a reserved
	 * token in PHP. */
	const DEFAULTCOLOR = CACA_DEFAULT;
	const TRANSPARENT = CACA_TRANSPARENT;
}

class Canvas {
	private $cv;

	function importFile($path, $codec) {
		return caca_import_file($this->cv, $path, $codec);
	}

	function importString($str, $codec) {
		return caca_import_string($this->cv, $str, $codec);
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

	function whereX() {
		return caca_wherex($this->cv);
	}

	function whereY() {
		return caca_wherey($this->cv);
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

	function Blit($x, $y, $canvas, $mask = NULL) {
		if($mask)
			return caca_blit($this->cv, $x, $y, $canvas->get_resource(), $mask->get_resource());
		return caca_blit($this->cv, $x, $y, $canvas->get_resource());
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
		return caca_draw_line($this->cv, $x1, $y1, $x2, $y2, $char);
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

class EventType
{
	const NONE = CACA_EVENT_NONE;

	const KEY_PRESS = CACA_EVENT_KEY_PRESS;
	const KEY_RELEASE = CACA_EVENT_KEY_RELEASE;
	const MOUSE_PRESS = CACA_EVENT_MOUSE_PRESS;
	const MOUSE_RELEASE = CACA_EVENT_MOUSE_RELEASE;
	const MOUSE_MOTION = CACA_EVENT_MOUSE_MOTION;
	const RESIZE = CACA_EVENT_RESIZE;
	const QUIT = CACA_EVENT_QUIT;

	const ANY = CACA_EVENT_ANY;
}

class EventKey
{
	const UNKNOWN = CACA_KEY_UNKNOWN;

	const CTRL_A = CACA_KEY_CTRL_A;
	const CTRL_B = CACA_KEY_CTRL_B;
	const CTRL_C = CACA_KEY_CTRL_C;
	const CTRL_D = CACA_KEY_CTRL_D;
	const CTRL_E = CACA_KEY_CTRL_E;
	const CTRL_F = CACA_KEY_CTRL_F;
	const CTRL_G = CACA_KEY_CTRL_G;
	const BACKSPACE = CACA_KEY_BACKSPACE;
	const TAB = CACA_KEY_TAB;
	const CTRL_J = CACA_KEY_CTRL_J;
	const CTRL_K = CACA_KEY_CTRL_K;
	const CTRL_L = CACA_KEY_CTRL_L;
	/* NOTE: We can't call this one RETURN because that's a
	 * reserved token in PHP */
	const RETURN_KEY = CACA_KEY_RETURN;
	const CTRL_N = CACA_KEY_CTRL_N;
	const CTRL_O = CACA_KEY_CTRL_O;
	const CTRL_P = CACA_KEY_CTRL_P;
	const CTRL_Q = CACA_KEY_CTRL_Q;
	const CTRL_R = CACA_KEY_CTRL_R;
	const PAUSE = CACA_KEY_PAUSE;
	const CTRL_T = CACA_KEY_CTRL_T;
	const CTRL_U = CACA_KEY_CTRL_U;
	const CTRL_V = CACA_KEY_CTRL_V;
	const CTRL_W = CACA_KEY_CTRL_W;
	const CTRL_X = CACA_KEY_CTRL_X;
	const CTRL_Y = CACA_KEY_CTRL_Y;
	const CTRL_Z = CACA_KEY_CTRL_Z;
	const ESCAPE = CACA_KEY_ESCAPE;
	const DELETE = CACA_KEY_DELETE;

	const UP = CACA_KEY_UP;
	const DOWN = CACA_KEY_DOWN;
	const LEFT = CACA_KEY_LEFT;
	const RIGHT = CACA_KEY_RIGHT;

	const INSERT = CACA_KEY_INSERT;
	const HOME = CACA_KEY_HOME;
	const END = CACA_KEY_END;
	const PAGEUP = CACA_KEY_PAGEUP;
	const PAGEDOWN = CACA_KEY_PAGEDOWN;

	const F1 = CACA_KEY_F1;
	const F2 = CACA_KEY_F2;
	const F3 = CACA_KEY_F3;
	const F4 = CACA_KEY_F4;
	const F5 = CACA_KEY_F5;
	const F6 = CACA_KEY_F6;
	const F7 = CACA_KEY_F7;
	const F8 = CACA_KEY_F8;
	const F9 = CACA_KEY_F9;
	const F10 = CACA_KEY_F10;
	const F11 = CACA_KEY_F11;
	const F12 = CACA_KEY_F12;
	const F13 = CACA_KEY_F13;
	const F14 = CACA_KEY_F14;
	const F15 = CACA_KEY_F15;
}

class Event {
	private $ev;

	function getType() {
		return caca_get_event_type($this->ev);
	}

	function getKeyCh() {
		return caca_get_event_key_ch($this->ev);
	}

	function getMouseX() {
		return caca_get_event_mouse_x($this->ev);
	}

	function getResizeWidth() {
		return caca_get_event_resize_width($this->ev);
	}

	function getResizeHeight() {
		return caca_get_event_resize_height($this->ev);
	}

	function __construct($_ev) {
		$this->ev = $_ev;
	}

	function get_resource() {
		return $this->ev;
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

	function getEvent($t, $timeout = 0) {
		$ev = caca_get_event($this->dp, $t, $timeout);
		if(! $ev) {
			return NULL;
		}
		return new Event($ev);
	}

	function __construct($canvas, $driver = NULL) {
		if($driver)
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
		return caca_dither_bitmap($canvas->get_resource(), $x, $y, $width, $height, $this->dt, $this->img, $load_palette);
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

	function Render($cv, $image) {
		return caca_render_canvas($cv->get_resource(), $this->f, $image);
	}

	static function getList() {
		return caca_get_font_list();
	}

	function __construct($name) {
		$this->f = caca_load_builtin_font($name);
	}
}
