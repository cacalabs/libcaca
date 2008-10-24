/*
 *  php-caca      Php binding for Libcaca
 *  Copyright (c) 2008 Vion Nicolas <nico@picapo.net>
 *
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_caca.h"

static function_entry caca_functions[] = {
	PHP_FE(caca_manage_canvas, NULL)
	PHP_FE(caca_unmanage_canvas, NULL)
	PHP_FE(caca_set_canvas_size, NULL)
	PHP_FE(caca_get_canvas_width, NULL)
	PHP_FE(caca_get_canvas_height, NULL)
	PHP_FE(caca_free_canvas, NULL)
	PHP_FE(caca_rand, NULL)
	PHP_FE(caca_get_version, NULL)
	PHP_FE(caca_gotoxy, NULL)
	PHP_FE(caca_get_cursor_x, NULL)
	PHP_FE(caca_get_cursor_y, NULL)
	PHP_FE(caca_put_char, NULL)
	PHP_FE(caca_put_str, NULL)
	PHP_FE(caca_printf, NULL)
	PHP_FE(caca_clear_canvas, NULL)
	PHP_FE(caca_set_canvas_handle, NULL)
	PHP_FE(caca_get_canvas_handle_x, NULL)
	PHP_FE(caca_get_canvas_handle_y, NULL)
	PHP_FE(caca_blit, NULL)
	PHP_FE(caca_set_canvas_boundaries, NULL)
	PHP_FE(caca_invert, NULL)
	PHP_FE(caca_flip, NULL)
	PHP_FE(caca_flop, NULL)
	PHP_FE(caca_rotate_left, NULL)
	PHP_FE(caca_rotate_right, NULL)
	PHP_FE(caca_stretch_left, NULL)
	PHP_FE(caca_stretch_right, NULL)
	PHP_FE(caca_set_attr, NULL)
	PHP_FE(caca_put_attr, NULL)
	PHP_FE(caca_set_color_ansi, NULL)
	PHP_FE(caca_set_color_argb, NULL)
	PHP_FE(caca_draw_line, NULL)
	PHP_FE(caca_draw_polyline, NULL)
	PHP_FE(caca_draw_thin_line, NULL)
	PHP_FE(caca_draw_thin_polyline, NULL)
	PHP_FE(caca_draw_circle, NULL)
	PHP_FE(caca_draw_ellipse, NULL)
	PHP_FE(caca_draw_thin_ellipse, NULL)
	PHP_FE(caca_fill_ellipse, NULL)
	PHP_FE(caca_draw_box, NULL)
	PHP_FE(caca_draw_thin_box, NULL)
	PHP_FE(caca_fill_box, NULL)
	PHP_FE(caca_draw_triangle, NULL)
	PHP_FE(caca_draw_thin_triangle, NULL)
	PHP_FE(caca_fill_triangle, NULL)
	PHP_FE(caca_get_frame_count, NULL)
	PHP_FE(caca_set_frame, NULL)
	PHP_FE(caca_get_frame_name, NULL)
	PHP_FE(caca_set_frame_name, NULL)
	PHP_FE(caca_create_frame, NULL)
	PHP_FE(caca_free_frame, NULL)
	PHP_FE(caca_set_dither_palette, NULL)
	PHP_FE(caca_set_dither_brightness, NULL)
	PHP_FE(caca_get_dither_brightness, NULL)
	PHP_FE(caca_set_dither_gamma, NULL)
	PHP_FE(caca_get_dither_gamma, NULL)
	PHP_FE(caca_set_dither_contrast, NULL)
	PHP_FE(caca_get_dither_contrast, NULL)
	PHP_FE(caca_set_dither_antialias, NULL)
	PHP_FE(caca_get_dither_antialias_list, NULL)
	PHP_FE(caca_get_dither_antialias, NULL)
	PHP_FE(caca_set_dither_color, NULL)
	PHP_FE(caca_get_dither_color_list, NULL)
	PHP_FE(caca_get_dither_color, NULL)
	PHP_FE(caca_set_dither_charset, NULL)
	PHP_FE(caca_get_dither_charset_list, NULL)
	PHP_FE(caca_get_dither_charset, NULL)
	PHP_FE(caca_set_dither_algorithm, NULL)
	PHP_FE(caca_get_dither_algorithm_list, NULL)
	PHP_FE(caca_get_dither_algorithm, NULL)
	PHP_FE(caca_dither_bitmap, NULL)
	PHP_FE(caca_free_dither, NULL)
	PHP_FE(caca_get_font_list, NULL)
	PHP_FE(caca_get_font_width, NULL)
	PHP_FE(caca_get_font_height, NULL)
	PHP_FE(caca_render_canvas, NULL)
	PHP_FE(caca_free_font, NULL)
	PHP_FE(caca_canvas_set_figfont, NULL)
	PHP_FE(caca_put_figchar, NULL)
	PHP_FE(caca_flush_figlet, NULL)
	PHP_FE(caca_file_close, NULL)
	PHP_FE(caca_file_gets, NULL)
	PHP_FE(caca_file_eof, NULL)
	PHP_FE(caca_get_import_list, NULL)
	PHP_FE(caca_export_memory, NULL)
	PHP_FE(caca_get_export_list, NULL)
	PHP_FE(caca_get_display_driver_list, NULL)
	PHP_FE(caca_get_display_driver, NULL)
	PHP_FE(caca_set_display_driver, NULL)
	PHP_FE(caca_free_display, NULL)
	PHP_FE(caca_refresh_display, NULL)
	PHP_FE(caca_set_display_time, NULL)
	PHP_FE(caca_get_display_time, NULL)
	PHP_FE(caca_get_display_width, NULL)
	PHP_FE(caca_get_display_height, NULL)
	PHP_FE(caca_set_display_title, NULL)
	PHP_FE(caca_set_mouse, NULL)
	PHP_FE(caca_set_cursor, NULL)
	PHP_FE(caca_get_event, NULL)
	PHP_FE(caca_get_mouse_x, NULL)
	PHP_FE(caca_get_mouse_y, NULL)
	PHP_FE(caca_get_event_type, NULL)
	PHP_FE(caca_get_event_key_ch, NULL)
	PHP_FE(caca_get_event_mouse_button, NULL)
	PHP_FE(caca_get_event_mouse_x, NULL)
	PHP_FE(caca_get_event_mouse_y, NULL)
	PHP_FE(caca_get_event_resize_width, NULL)
	PHP_FE(caca_get_event_resize_height, NULL)

	{NULL, NULL, NULL}
};

zend_module_entry caca_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_CACA_EXTNAME,
	caca_functions,
	PHP_MINIT(caca),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(caca),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_CACA_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CACA
ZEND_GET_MODULE(caca)
#endif

PHP_MINFO_FUNCTION(caca) {
	php_info_print_table_start();
	php_info_print_table_row(2, "Caca library version", caca_get_version());
	php_info_print_table_end();
}
//--------INITIALIZATION---------//

PHP_MINIT_FUNCTION(caca) {
	
	REGISTER_LONG_CONSTANT("CACA_BLACK", CACA_BLACK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_BLUE", CACA_BLUE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_GREEN", CACA_GREEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_CYAN", CACA_CYAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_RED", CACA_RED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_MAGENTA", CACA_MAGENTA, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_BROWN", CACA_BROWN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_LIGHTGRAY", CACA_LIGHTGRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_DARKGRAY", CACA_DARKGRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_LIGHTBLUE", CACA_LIGHTBLUE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_LIGHTGREEN", CACA_LIGHTGREEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_LIGHTCYAN", CACA_LIGHTCYAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_LIGHTRED", CACA_LIGHTRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_LIGHTMAGENTA", CACA_LIGHTMAGENTA, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_YELLOW", CACA_YELLOW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_WHITE", CACA_WHITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_DEFAULT", CACA_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_TRANSPARENT", CACA_TRANSPARENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_BOLD", CACA_BOLD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_ITALICS", CACA_ITALICS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_UNDERLINE", CACA_UNDERLINE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_BLINK", CACA_BLINK, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

//-------CACA'S FUNCTIONS----------------//

PHP_FUNCTION(caca_manage_canvas) {
}

PHP_FUNCTION(caca_unmanage_canvas) {
}

PHP_FUNCTION(caca_set_canvas_size) {
}

PHP_FUNCTION(caca_get_canvas_width) {
}

PHP_FUNCTION(caca_get_canvas_height) {
}

PHP_FUNCTION(caca_free_canvas) {
}

PHP_FUNCTION(caca_rand) {
}

PHP_FUNCTION(caca_get_version) {
	RETURN_STRING(estrdup(caca_get_version()), 0);
}

PHP_FUNCTION(caca_gotoxy) {
}

PHP_FUNCTION(caca_get_cursor_x) {
}

PHP_FUNCTION(caca_get_cursor_y) {
}

PHP_FUNCTION(caca_put_char) {
}

PHP_FUNCTION(caca_put_str) {
}

PHP_FUNCTION(caca_printf) {
}

PHP_FUNCTION(caca_clear_canvas) {
}

PHP_FUNCTION(caca_set_canvas_handle) {
}

PHP_FUNCTION(caca_get_canvas_handle_x) {
}

PHP_FUNCTION(caca_get_canvas_handle_y) {
}

PHP_FUNCTION(caca_blit) {
}

PHP_FUNCTION(caca_set_canvas_boundaries) {
}

PHP_FUNCTION(caca_invert) {
}

PHP_FUNCTION(caca_flip) {
}

PHP_FUNCTION(caca_flop) {
}

PHP_FUNCTION(caca_rotate_180) {
}

PHP_FUNCTION(caca_rotate_left) {
}

PHP_FUNCTION(caca_rotate_right) {
}

PHP_FUNCTION(caca_stretch_left) {
}

PHP_FUNCTION(caca_stretch_right) {
}

PHP_FUNCTION(caca_set_attr) {
}

PHP_FUNCTION(caca_put_attr) {
}

PHP_FUNCTION(caca_set_color_ansi) {
}

PHP_FUNCTION(caca_set_color_argb) {
}

PHP_FUNCTION(caca_attr_to_argb64) {
}

PHP_FUNCTION(caca_utf32_to_ascii) {
}

PHP_FUNCTION(caca_utf32_is_fullwidth) {
}

PHP_FUNCTION(caca_draw_line) {
}

PHP_FUNCTION(caca_draw_polyline) {
}

PHP_FUNCTION(caca_draw_thin_line) {
}

PHP_FUNCTION(caca_draw_thin_polyline) {
}

PHP_FUNCTION(caca_draw_circle) {
}

PHP_FUNCTION(caca_draw_ellipse) {
}

PHP_FUNCTION(caca_draw_thin_ellipse) {
}

PHP_FUNCTION(caca_fill_ellipse) {
}

PHP_FUNCTION(caca_draw_box) {
}

PHP_FUNCTION(caca_draw_thin_box) {
}

PHP_FUNCTION(caca_draw_cp437_box) {
}

PHP_FUNCTION(caca_fill_box) {
}

PHP_FUNCTION(caca_draw_triangle) {
}

PHP_FUNCTION(caca_draw_thin_triangle) {
}

PHP_FUNCTION(caca_fill_triangle) {
}

PHP_FUNCTION(caca_get_frame_count) {
}

PHP_FUNCTION(caca_set_frame) {
}

PHP_FUNCTION(caca_get_frame_name) {
}

PHP_FUNCTION(caca_set_frame_name) {
}

PHP_FUNCTION(caca_create_frame) {
}

PHP_FUNCTION(caca_free_frame) {
}

PHP_FUNCTION(caca_set_dither_palette) {
}

PHP_FUNCTION(caca_set_dither_brightness) {
}

PHP_FUNCTION(caca_get_dither_brightness) {
}

PHP_FUNCTION(caca_set_dither_gamma) {
}

PHP_FUNCTION(caca_get_dither_gamma) {
}

PHP_FUNCTION(caca_set_dither_contrast) {
}

PHP_FUNCTION(caca_get_dither_contrast) {
}

PHP_FUNCTION(caca_set_dither_antialias) {
}

PHP_FUNCTION(caca_get_dither_antialias_list) {
}

PHP_FUNCTION(caca_get_dither_antialias) {
}

PHP_FUNCTION(caca_set_dither_color) {
}

PHP_FUNCTION(caca_get_dither_color_list) {
}

PHP_FUNCTION(caca_get_dither_color) {
}

PHP_FUNCTION(caca_set_dither_charset) {
}

PHP_FUNCTION(caca_get_dither_charset_list) {
}

PHP_FUNCTION(caca_get_dither_charset) {
}

PHP_FUNCTION(caca_set_dither_algorithm) {
}

PHP_FUNCTION(caca_get_dither_algorithm_list) {
}

PHP_FUNCTION(caca_get_dither_algorithm) {
}

PHP_FUNCTION(caca_dither_bitmap) {
}

PHP_FUNCTION(caca_free_dither) {
}

PHP_FUNCTION(caca_get_font_list) {
}

PHP_FUNCTION(caca_get_font_width) {
}

PHP_FUNCTION(caca_get_font_height) {
}

PHP_FUNCTION(caca_render_canvas) {
}

PHP_FUNCTION(caca_free_font) {
}

PHP_FUNCTION(caca_canvas_set_figfont) {
}

PHP_FUNCTION(caca_put_figchar) {
}

PHP_FUNCTION(caca_flush_figlet) {
}

PHP_FUNCTION(caca_file_close) {
}

PHP_FUNCTION(caca_file_gets) {
}

PHP_FUNCTION(caca_file_eof) {
}

PHP_FUNCTION(caca_get_import_list) {
}

PHP_FUNCTION(caca_export_memory) {
}

PHP_FUNCTION(caca_get_export_list) {
}

PHP_FUNCTION(caca_get_display_driver_list) {
}

PHP_FUNCTION(caca_get_display_driver) {
}

PHP_FUNCTION(caca_set_display_driver) {
}

PHP_FUNCTION(caca_free_display) {
}

PHP_FUNCTION(caca_refresh_display) {
}

PHP_FUNCTION(caca_set_display_time) {
}

PHP_FUNCTION(caca_get_display_time) {
}

PHP_FUNCTION(caca_get_display_width) {
}

PHP_FUNCTION(caca_get_display_height) {
}

PHP_FUNCTION(caca_set_display_title) {
}

PHP_FUNCTION(caca_set_mouse) {
}

PHP_FUNCTION(caca_set_cursor) {
}

PHP_FUNCTION(caca_get_event) {
}

PHP_FUNCTION(caca_get_mouse_x) {
}

PHP_FUNCTION(caca_get_mouse_y) {
}

PHP_FUNCTION(caca_get_event_type) {
}

PHP_FUNCTION(caca_get_event_key_ch) {
}

PHP_FUNCTION(caca_get_event_key_utf8) {
}

PHP_FUNCTION(caca_get_event_mouse_button) {
}

PHP_FUNCTION(caca_get_event_mouse_x) {
}

PHP_FUNCTION(caca_get_event_mouse_y) {
}

PHP_FUNCTION(caca_get_event_resize_width) {
}

PHP_FUNCTION(caca_get_event_resize_height) {
}


