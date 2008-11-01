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
	private var cv;

	function setSize($width, $height) {
		return caca_set_canvas_width($this->cv, $width, $height);
	}

	function getWidth() {
		return caca_get_canvas_width($this->cv);
	}

	function getHeight() {
		return caca_get_canvas_height($this->cv);
	}
	
	function getAttr(, ) {
	}

	function setAttr() {
	}

	function setColorANSI($foreground, $background) {
		return caca_set_color_ansi($this->cv, $foreground, $background);
	}

	function setColorARGB($foreground, $background) {
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
		return caca_canvas_clear($this->cv);
	}

	function Blit(, , $c1, $c2) {
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

	function drawLine(, , , , ) {
	}

	function drawPolyline() {
	}

	function drawThinLine(, , , ) {
	}

	function drawThinPolyline() {
	}

	function drawCircle(, , , ) {
	}

	function drawEllipse(, , , , ) {
	}

	function drawThinEllipse(, , , ) {
	}

	function fillEllipse(, , , , ) {
	}

	function drawBox(, , , , ) {
	}

	function drawThinBox(, , , ) {
	}

	function drawCP437Box(, , , ) {
	}

	function fillBox(, , , , ) {
	}

	function drawTriangle(, , , , , , ) {
	}

	function drawThriangle(, , , , , ) {
	}

	function fillTriangle(, , , , , , ) {
	}

	function __construct($width = 0, $height = 0) {
		cv = caca_create_canvas($width, $height);
	}
}
