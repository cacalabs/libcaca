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
	PHP_FE(caca_create_event, NULL)
	PHP_FE(caca_create_canvas, NULL)
	PHP_FE(caca_manage_canvas, NULL)
	PHP_FE(caca_unmanage_canvas, NULL)
	PHP_FE(caca_set_canvas_size, NULL)
	PHP_FE(caca_get_canvas_width, NULL)
	PHP_FE(caca_get_canvas_height, NULL)
	PHP_FE(caca_get_canvas_chars, NULL)
	PHP_FE(caca_get_canvas_attrs, NULL)
	PHP_FE(caca_rand, NULL)
	PHP_FE(caca_get_version, NULL)
	PHP_FE(caca_gotoxy, NULL)
	PHP_FE(caca_get_cursor_x, NULL)
	PHP_FE(caca_get_cursor_y, NULL)
	PHP_FE(caca_put_char, NULL)
	PHP_FE(caca_get_char, NULL)
	PHP_FE(caca_put_str, NULL)
	PHP_FE(caca_clear_canvas, NULL)
	PHP_FE(caca_set_canvas_handle, NULL)
	PHP_FE(caca_get_canvas_handle_x, NULL)
	PHP_FE(caca_get_canvas_handle_y, NULL)
	PHP_FE(caca_blit, NULL)
	PHP_FE(caca_set_canvas_boundaries, NULL)
	PHP_FE(caca_invert, NULL)
	PHP_FE(caca_flip, NULL)
	PHP_FE(caca_flop, NULL)
	PHP_FE(caca_rotate_180, NULL)
	PHP_FE(caca_rotate_left, NULL)
	PHP_FE(caca_rotate_right, NULL)
	PHP_FE(caca_stretch_left, NULL)
	PHP_FE(caca_stretch_right, NULL)
	PHP_FE(caca_get_attr, NULL)
	PHP_FE(caca_set_attr, NULL)
	PHP_FE(caca_put_attr, NULL)
	PHP_FE(caca_set_color_ansi, NULL)
	PHP_FE(caca_set_color_argb, NULL)
	PHP_FE(caca_attr_to_ansi, NULL)
	PHP_FE(caca_attr_to_ansi_fg, NULL)
	PHP_FE(caca_attr_to_ansi_bg, NULL)
	PHP_FE(caca_attr_to_rgb12_fg, NULL)
	PHP_FE(caca_attr_to_rgb12_bg, NULL)
	PHP_FE(caca_attr_to_argb64, NULL)
	PHP_FE(caca_utf8_to_utf32, NULL)
	PHP_FE(caca_utf32_to_utf8, NULL)
	PHP_FE(caca_utf32_to_cp437, NULL)
	PHP_FE(caca_cp437_to_utf32, NULL)
	PHP_FE(caca_utf32_to_ascii, NULL)
	PHP_FE(caca_utf32_is_fullwidth, NULL)
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
	PHP_FE(caca_draw_cp437_box, NULL)
	PHP_FE(caca_fill_box, NULL)
	PHP_FE(caca_draw_triangle, NULL)
	PHP_FE(caca_draw_thin_triangle, NULL)
	PHP_FE(caca_fill_triangle, NULL)
	PHP_FE(caca_get_frame_count, NULL)
	PHP_FE(caca_set_frame, NULL)
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
	PHP_FE(caca_get_font_list, NULL)
	PHP_FE(caca_get_font_width, NULL)
	PHP_FE(caca_get_font_height, NULL)
	PHP_FE(caca_render_canvas, NULL)
	PHP_FE(caca_canvas_set_figfont, NULL)
	PHP_FE(caca_put_figchar, NULL)
	PHP_FE(caca_flush_figlet, NULL)
	PHP_FE(caca_file_close, NULL)
	PHP_FE(caca_file_tell, NULL)
	PHP_FE(caca_file_read, NULL)
	PHP_FE(caca_file_write, NULL)
	PHP_FE(caca_file_gets, NULL)
	PHP_FE(caca_file_eof, NULL)
	PHP_FE(caca_import_string, NULL)
	PHP_FE(caca_import_file, NULL)
	PHP_FE(caca_get_import_list, NULL)
	PHP_FE(caca_export_string, NULL)
	PHP_FE(caca_get_export_list, NULL)
	PHP_FE(caca_create_display, NULL)
	PHP_FE(caca_create_display_with_driver, NULL)
	PHP_FE(caca_get_display_driver_list, NULL)
	PHP_FE(caca_get_display_driver, NULL)
	PHP_FE(caca_set_display_driver, NULL)
	PHP_FE(caca_get_canvas, NULL)
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

//-------- Caca's ressources destructors -----------//

static void php_caca_canvas_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	caca_free_canvas(rsrc->ptr);
}

static void php_caca_dither_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	caca_free_dither(rsrc->ptr);
}

static void php_caca_font_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	caca_free_font(rsrc->ptr);
}

static void php_caca_file_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
}

static void php_caca_display_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	caca_free_display(rsrc->ptr);
}

static void php_caca_event_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	efree((caca_event_t *) rsrc->ptr);
}

//-------- Initialization ---------//

PHP_MINIT_FUNCTION(caca) {

	le_caca_canvas = zend_register_list_destructors_ex(php_caca_canvas_dtor, NULL, PHP_CACA_CANVAS_RES_NAME, module_number);
	le_caca_dither = zend_register_list_destructors_ex(php_caca_dither_dtor, NULL, PHP_CACA_DITHER_RES_NAME, module_number);
	le_caca_font = zend_register_list_destructors_ex(php_caca_font_dtor, NULL, PHP_CACA_FONT_RES_NAME, module_number);
	le_caca_file = zend_register_list_destructors_ex(php_caca_file_dtor, NULL, PHP_CACA_FILE_RES_NAME, module_number);
	le_caca_display = zend_register_list_destructors_ex(php_caca_display_dtor, NULL, PHP_CACA_DISPLAY_RES_NAME, module_number);
	le_caca_event = zend_register_list_destructors_ex(php_caca_event_dtor, NULL, PHP_CACA_EVENT_RES_NAME, module_number);
	
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

	REGISTER_LONG_CONSTANT("CACA_EVENT_NONE", CACA_EVENT_NONE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_KEY_PRESS", CACA_EVENT_KEY_PRESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_KEY_RELEASE", CACA_EVENT_KEY_RELEASE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_MOUSE_PRESS", CACA_EVENT_MOUSE_PRESS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_MOUSE_RELEASE", CACA_EVENT_MOUSE_RELEASE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_MOUSE_MOTION", CACA_EVENT_MOUSE_MOTION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_RESIZE", CACA_EVENT_RESIZE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_QUIT", CACA_EVENT_QUIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_EVENT_ANY", CACA_EVENT_ANY, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_A", CACA_KEY_CTRL_A, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_B", CACA_KEY_CTRL_B, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_C", CACA_KEY_CTRL_C, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_D", CACA_KEY_CTRL_D, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_E", CACA_KEY_CTRL_E, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_F", CACA_KEY_CTRL_F, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_G", CACA_KEY_CTRL_G, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_BACKSPACE", CACA_KEY_BACKSPACE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_TAB", CACA_KEY_TAB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_J", CACA_KEY_CTRL_J, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_K", CACA_KEY_CTRL_K, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_L", CACA_KEY_CTRL_L, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_RETURN", CACA_KEY_RETURN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_N", CACA_KEY_CTRL_N, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_O", CACA_KEY_CTRL_O, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_P", CACA_KEY_CTRL_P, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_Q", CACA_KEY_CTRL_Q, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_R", CACA_KEY_CTRL_R, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_PAUSE", CACA_KEY_PAUSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_T", CACA_KEY_CTRL_T, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_U", CACA_KEY_CTRL_U, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_V", CACA_KEY_CTRL_V, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_W", CACA_KEY_CTRL_W, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_X", CACA_KEY_CTRL_X, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_Y", CACA_KEY_CTRL_Y, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_CTRL_Z", CACA_KEY_CTRL_Z, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_ESCAPE", CACA_KEY_ESCAPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_DELETE", CACA_KEY_DELETE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_UP", CACA_KEY_UP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_DOWN", CACA_KEY_DOWN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_LEFT", CACA_KEY_LEFT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_RIGHT", CACA_KEY_RIGHT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_INSERT", CACA_KEY_INSERT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_HOME", CACA_KEY_HOME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_END", CACA_KEY_END, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_PAGEUP", CACA_KEY_PAGEUP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_PAGEDOWN", CACA_KEY_PAGEDOWN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F1", CACA_KEY_F1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F2", CACA_KEY_F2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F3", CACA_KEY_F3, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F4", CACA_KEY_F4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F5", CACA_KEY_F5, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F6", CACA_KEY_F6, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F7", CACA_KEY_F7, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F8", CACA_KEY_F8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F9", CACA_KEY_F9, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F10", CACA_KEY_F10, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F11", CACA_KEY_F11, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F12", CACA_KEY_F12, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F13", CACA_KEY_F13, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F14", CACA_KEY_F14, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CACA_KEY_F15", CACA_KEY_F15, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

//---------- Some usefull macros ---------------//

#define FETCH_CANVAS(canvas) \
	zval *_zval; \
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &_zval) == FAILURE) { \
		RETURN_FALSE; \
	} \
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas); 

#define FETCH_DISPLAY(display) \
	zval *_zval; \
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &_zval) == FAILURE) { \
		RETURN_FALSE; \
	} \
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display); 

#define FETCH_EVENT(event) \
	zval *_zval; \
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &_zval) == FAILURE) { \
		RETURN_FALSE; \
	} \
	ZEND_FETCH_RESOURCE(event, caca_event_t*, &_zval, -1, PHP_CACA_EVENT_RES_NAME, le_caca_event); 

#define FETCH_LONG(l) \
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &l) == FAILURE) { \
		RETURN_FALSE; \
	}

#define RETURN_CHAR(c) \
	char *str = emalloc(2); \
	str[0] = c; \
	str[1] = '\0'; \
	RETURN_STRING(str, 0);

#define RETURN_SUCCESS(i) \
	RETURN_BOOL((i) == 0);

//------- PHP Binding's specific functions ----------//

PHP_FUNCTION(caca_create_event) {
	caca_event_t *event;
	event = emalloc(sizeof(caca_event_t));
	ZEND_REGISTER_RESOURCE(return_value, event, le_caca_event);
}

//------- Caca's functions ----------------//

PHP_FUNCTION(caca_create_canvas) {
	long width, height = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &width, &height) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas = caca_create_canvas(width, height);
	if (!canvas) {
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, canvas, le_caca_canvas);
}

PHP_FUNCTION(caca_manage_canvas) {
}

PHP_FUNCTION(caca_unmanage_canvas) {
}

PHP_FUNCTION(caca_set_canvas_size) {
	zval *_zval;
	long width, height = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &width, &height) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_canvas_size(canvas, width, height));
}

PHP_FUNCTION(caca_get_canvas_width) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_LONG(caca_get_canvas_width(canvas));
}

PHP_FUNCTION(caca_get_canvas_height) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_LONG(caca_get_canvas_height(canvas));
}

PHP_FUNCTION(caca_get_canvas_chars) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_STRING((char *) caca_get_canvas_chars(canvas), 1); //TODO: check that return \0 terminated string
}

PHP_FUNCTION(caca_get_canvas_attrs) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_STRING((char *) caca_get_canvas_attrs(canvas), 1); //TODO: check that return \0 terminated string
}

PHP_FUNCTION(caca_rand) {
	long min, max = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &min, &max) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_LONG(caca_rand(min, max));
}

PHP_FUNCTION(caca_get_version) {
	RETURN_STRING((char *) caca_get_version(), 1);
}

PHP_FUNCTION(caca_gotoxy) {
	zval *_zval;
	long x, y = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &x, &y) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_gotoxy(canvas, x, y));
}

PHP_FUNCTION(caca_get_cursor_x) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_LONG(caca_get_cursor_x(canvas));
}

PHP_FUNCTION(caca_get_cursor_y) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_LONG(caca_get_cursor_y(canvas));
}

PHP_FUNCTION(caca_put_char) {
	zval *_zval;
	char *str;
	long str_len, x, y = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlls", &_zval, &x, &y, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_put_char(canvas, x, y, str[0]));
}

PHP_FUNCTION(caca_get_char) {
	zval *_zval;
	long x, y = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &x, &y) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_CHAR(caca_get_char(canvas, x, y));
}

PHP_FUNCTION(caca_put_str) {
	zval *_zval;
	char *str;
	long x, y, str_len = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlls", &_zval, &x, &y, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_put_str(canvas, x, y, str));
}

PHP_FUNCTION(caca_clear_canvas) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_LONG(caca_clear_canvas(canvas));
}

PHP_FUNCTION(caca_set_canvas_handle) {
	zval *_zval;
	long x, y = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &x, &y) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_canvas_handle(canvas, x, y));
}

PHP_FUNCTION(caca_get_canvas_handle_x) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_LONG(caca_get_canvas_handle_x(canvas));
}

PHP_FUNCTION(caca_get_canvas_handle_y) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_LONG(caca_get_canvas_handle_y(canvas));
}

PHP_FUNCTION(caca_blit) {
	zval *_zval1, *_zval2, *_zval3 = NULL;
	long x, y = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllr|r", &_zval1, &x, &y, &_zval2, &_zval3) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *dst, *src, *mask = NULL;
	ZEND_FETCH_RESOURCE(dst, caca_canvas_t*, &_zval1, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	ZEND_FETCH_RESOURCE(src, caca_canvas_t*, &_zval2, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (_zval3) {
		ZEND_FETCH_RESOURCE(mask, caca_canvas_t*, &_zval3, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	}
	RETURN_SUCCESS(caca_blit(dst, x, y, src, NULL));
}

PHP_FUNCTION(caca_set_canvas_boundaries) {
	zval *_zval;
	long x, y, width, height = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllll", &_zval, &x, &y, &width, &height) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_canvas_boundaries(canvas, x, y, width, height));
}

PHP_FUNCTION(caca_invert) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_invert(canvas));
}

PHP_FUNCTION(caca_flip) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_flip(canvas));
}

PHP_FUNCTION(caca_flop) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_flop(canvas));
}

PHP_FUNCTION(caca_rotate_180) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_rotate_180(canvas));
}

PHP_FUNCTION(caca_rotate_left) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_rotate_left(canvas));
}

PHP_FUNCTION(caca_rotate_right) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_rotate_right(canvas));
}

PHP_FUNCTION(caca_stretch_left) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_stretch_left(canvas));
}

PHP_FUNCTION(caca_stretch_right) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);
	RETURN_SUCCESS(caca_stretch_right(canvas));
}

PHP_FUNCTION(caca_get_attr) {
	zval *_zval;
	long x, y = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &x, &y) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_LONG(caca_get_attr(canvas, x, y));
}

PHP_FUNCTION(caca_set_attr) {
	zval *_zval;
	long attr = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &attr) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_attr(canvas, attr));
}

PHP_FUNCTION(caca_put_attr) {
	zval *_zval;
	long x, y, attr = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlll", &_zval, &x, &y, &attr) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_put_attr(canvas, x, y, attr));
}

PHP_FUNCTION(caca_set_color_ansi) {
	zval *_zval;
	long foreground, background = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &foreground, &background) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_color_ansi(canvas, foreground, background));
}

PHP_FUNCTION(caca_set_color_argb) {
	zval *_zval;
	long foreground, background = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &_zval, &foreground, &background) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_color_argb(canvas, foreground, background));
}

PHP_FUNCTION(caca_attr_to_ansi) {
	long l;
	FETCH_LONG(l);
	RETURN_LONG(caca_attr_to_ansi(l));
}

PHP_FUNCTION(caca_attr_to_ansi_fg) {
	long l;
	FETCH_LONG(l);
	RETURN_LONG(caca_attr_to_ansi_fg(l));
}

PHP_FUNCTION(caca_attr_to_ansi_bg) {
	long l;
	FETCH_LONG(l);
	RETURN_LONG(caca_attr_to_ansi_bg(l));
}

PHP_FUNCTION(caca_attr_to_rgb12_fg) {
	long l;
	FETCH_LONG(l);
	RETURN_LONG(caca_attr_to_rgb12_fg(l));
}

PHP_FUNCTION(caca_attr_to_rgb12_bg) {
	long l;
	FETCH_LONG(l);
	RETURN_LONG(caca_attr_to_rgb12_bg(l));
}

PHP_FUNCTION(caca_attr_to_argb64) {
}

PHP_FUNCTION(caca_utf8_to_utf32) {
}

PHP_FUNCTION(caca_utf32_to_utf8) {
}

PHP_FUNCTION(caca_utf32_to_cp437) {
}

PHP_FUNCTION(caca_cp437_to_utf32) {
}

PHP_FUNCTION(caca_utf32_to_ascii) {
}

PHP_FUNCTION(caca_utf32_is_fullwidth) {
}

PHP_FUNCTION(caca_draw_line) {
	zval *_zval;
	char *str;
	long str_len, xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlllls", &_zval, &xa, &ya, &xb, &yb, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_draw_line(canvas, xa, ya, xb, yb, str[0]));
}

PHP_FUNCTION(caca_draw_polyline) {
}

PHP_FUNCTION(caca_draw_thin_line) {
	zval *_zval;
	long xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllll", &_zval, &xa, &ya, &xb, &yb) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_draw_thin_line(canvas, xa, ya, xb, yb));
}

PHP_FUNCTION(caca_draw_thin_polyline) {
}

PHP_FUNCTION(caca_draw_circle) {
	zval *_zval;
	char *str;
	long str_len, x, y, r = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllls", &_zval, &x, &y, &r, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_draw_circle(canvas, x, y, r, str[0]));
}

PHP_FUNCTION(caca_draw_ellipse) {
	zval *_zval;
	char *str;
	long str_len, xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlllls", &_zval, &xa, &ya, &xb, &yb, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_draw_ellipse(canvas, xa, ya, xb, yb, str[0]));
}

PHP_FUNCTION(caca_draw_thin_ellipse) {
	zval *_zval;
	long xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllll", &_zval, &xa, &ya, &xb, &yb) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_draw_thin_ellipse(canvas, xa, ya, xb, yb));
}

PHP_FUNCTION(caca_fill_ellipse) {
	zval *_zval;
	char *str;
	long str_len, xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlllls", &_zval, &xa, &ya, &xb, &yb, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_fill_ellipse(canvas, xa, ya, xb, yb, str[0]));
}

PHP_FUNCTION(caca_draw_box) {
	zval *_zval;
	char *str;
	long str_len, xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlllls", &_zval, &xa, &ya, &xb, &yb, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_draw_box(canvas, xa, ya, xb, yb, str[0]));
}

PHP_FUNCTION(caca_draw_thin_box) {
	zval *_zval;
	long xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllll", &_zval, &xa, &ya, &xb, &yb) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_draw_thin_box(canvas, xa, ya, xb, yb));
}

PHP_FUNCTION(caca_draw_cp437_box) {
	zval *_zval;
	long xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllll", &_zval, &xa, &ya, &xb, &yb) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_draw_cp437_box(canvas, xa, ya, xb, yb));
}

PHP_FUNCTION(caca_fill_box) {
	zval *_zval;
	char *str;
	long str_len, xa, ya, xb, yb = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlllls", &_zval, &xa, &ya, &xb, &yb, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_fill_box(canvas, xa, ya, xb, yb, str[0]));
}

PHP_FUNCTION(caca_draw_triangle) {
	zval *_zval;
	char *str;
	long str_len, xa, ya, xb, yb, xc, yc = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlllllls", &_zval, &xa, &ya, &xb, &yb, &xc, &yc, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_draw_triangle(canvas, xa, ya, xb, yb, xc, yc, str[0]));
}

PHP_FUNCTION(caca_draw_thin_triangle) {
	zval *_zval;
	long xa, ya, xb, yb, xc, yc = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllllll", &_zval, &xa, &ya, &xb, &yb, &xc, &yc) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_draw_thin_triangle(canvas, xa, ya, xb, yb, xc, yc));
}

PHP_FUNCTION(caca_fill_triangle) {
	zval *_zval;
	char *str;
	long str_len, xa, ya, xb, yb, xc, yc = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlllllls", &_zval, &xa, &ya, &xb, &yb, &xc, &yc, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	if (str_len != 1) {
		RETURN_FALSE;
	}
	RETURN_SUCCESS(caca_fill_triangle(canvas, xa, ya, xb, yb, xc, yc, str[0]));
}

PHP_FUNCTION(caca_get_frame_count) {
	zval *_zval;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &_zval) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_LONG(caca_get_canvas_count(canvas));
}

PHP_FUNCTION(caca_set_frame) {
	zval *_zval;
	long id;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &_zval, &id) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_frame(canvas, id));
}

PHP_FUNCTION(caca_set_frame_name) {
	zval *_zval;
	char *str;
	long str_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &_zval, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_set_frame_name(canvas, str));
}

PHP_FUNCTION(caca_create_frame) {
	zval *_zval;
	long id;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &_zval, &id) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_create_frame(canvas, id));
}

PHP_FUNCTION(caca_free_frame) {
	zval *_zval;
	long id;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &_zval, &id) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);
	RETURN_SUCCESS(caca_free_frame(canvas, id));
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

PHP_FUNCTION(caca_get_font_list) {
}

PHP_FUNCTION(caca_get_font_width) {
}

PHP_FUNCTION(caca_get_font_height) {
}

PHP_FUNCTION(caca_render_canvas) {
}

PHP_FUNCTION(caca_canvas_set_figfont) {
}

PHP_FUNCTION(caca_put_figchar) {
}

PHP_FUNCTION(caca_flush_figlet) {
}

PHP_FUNCTION(caca_file_close) {
}

PHP_FUNCTION(caca_file_tell) {
}

PHP_FUNCTION(caca_file_read) {
}

PHP_FUNCTION(caca_file_write) {
}

PHP_FUNCTION(caca_file_gets) {
}

PHP_FUNCTION(caca_file_eof) {
}

PHP_FUNCTION(caca_import_string) {
	zval *_zval;
	char *src, *type;
	long src_len, type_len = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &_zval, &src, &src_len, &type, &type_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);

	RETURN_LONG(caca_import_memory(canvas, src, src_len, type));
}

PHP_FUNCTION(caca_import_file) {
}

PHP_FUNCTION(caca_get_import_list) {
}

PHP_FUNCTION(caca_export_string) {
	zval *_zval;
	char *type;
	long type_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &_zval, &type, &type_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);

	void *buffer;
	size_t len;
	buffer = caca_export_memory(canvas, type, &len);
	return_value->type = IS_STRING;
	return_value->value.str.len = len;
	return_value->value.str.val = emalloc(len);
	memcpy(return_value->value.str.val, buffer, len);
	free(buffer);
}

PHP_FUNCTION(caca_get_export_list) {
}

PHP_FUNCTION(caca_create_display) {
	caca_canvas_t *canvas;
	FETCH_CANVAS(canvas);

	caca_display_t *display = caca_create_display(canvas);
	ZEND_REGISTER_RESOURCE(return_value, display, le_caca_display);
}

PHP_FUNCTION(caca_create_display_with_driver) {
	zval *_zval;
	char *str;
	long str_len = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &_zval, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_canvas_t *canvas;
	ZEND_FETCH_RESOURCE(canvas, caca_canvas_t*, &_zval, -1, PHP_CACA_CANVAS_RES_NAME, le_caca_canvas);

	caca_display_t *display = caca_create_display_with_driver(canvas, str);
	if (!display) {
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, display, le_caca_display);
}

PHP_FUNCTION(caca_get_display_driver_list) {
	char const * const *list = caca_get_display_driver_list();
	int i;
	array_init(return_value);	
	for(i = 0; list[i]; i += 2)
		add_assoc_string(return_value, (char*) list[i], (char*) list[i + 1], 1);
}

PHP_FUNCTION(caca_get_display_driver) {
	zval *_zval;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &_zval) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);
	RETURN_STRING((char *) caca_get_display_driver(display), 1);
}

PHP_FUNCTION(caca_set_display_driver) {
	zval *_zval;
	char *str;
	long str_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &_zval, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);
	RETURN_SUCESS(caca_set_display_driver(display, str));
}

PHP_FUNCTION(caca_get_canvas) {
	zval *_zval;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &_zval) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);
	caca_canvas_t *canvas = caca_get_canvas(display);
	ZEND_REGISTER_RESOURCE(return_value, canvas, le_caca_canvas);
}

PHP_FUNCTION(caca_refresh_display) {
	caca_display_t *display;
	FETCH_DISPLAY(display);
	RETURN_SUCCESS(caca_refresh_display(display));
}

PHP_FUNCTION(caca_set_display_time) {
	zval *_zval;
	long value = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &_zval, &value) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);
	RETURN_SUCCESS(caca_set_display_time(display, value));
}

PHP_FUNCTION(caca_get_display_time) {
	caca_display_t *display;
	FETCH_DISPLAY(display);
	RETURN_LONG(caca_get_display_time(display)); //TODO: check return value
}

PHP_FUNCTION(caca_get_display_width) {
	caca_display_t *display;
	FETCH_DISPLAY(display);
	RETURN_LONG(caca_get_display_width(display));
}

PHP_FUNCTION(caca_get_display_height) {
	caca_display_t *display;
	FETCH_DISPLAY(display);
	RETURN_LONG(caca_get_display_height(display));
}

PHP_FUNCTION(caca_set_display_title) {
	zval *_zval;
	char *str;
	long str_len = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &_zval, &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);
	RETURN_SUCCESS(caca_set_display(display, str));
}

PHP_FUNCTION(caca_set_mouse) {
	zval *_zval;
	long value = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &_zval, &value) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);
	RETURN_SUCCESS(caca_set_mouse(display, value));
}

PHP_FUNCTION(caca_set_cursor) {
	zval *_zval;
	long value = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &_zval, &value) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);
	RETURN_SUCCESS(caca_set_cursor(display, value));
}

PHP_FUNCTION(caca_get_event) {
	zval *_zval1, *_zval2 = NULL;
	long g, aa = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|rl", &_zval1, &g, &_zval2, &aa) == FAILURE) {
		RETURN_FALSE;
	}
	caca_display_t *display;
	ZEND_FETCH_RESOURCE(display, caca_display_t*, &_zval1, -1, PHP_CACA_DISPLAY_RES_NAME, le_caca_display);

	caca_event_t *event = NULL;
	if (_zval2) {
		ZEND_FETCH_RESOURCE(event, caca_event_t*, &_zval2, -1, PHP_CACA_EVENT_RES_NAME, le_caca_event);
	}
	RETURN_LONG(caca_get_event(display, g, event, aa));
}

PHP_FUNCTION(caca_get_mouse_x) {
	caca_display_t *display;
	FETCH_DISPLAY(display);
	RETURN_LONG(caca_get_mouse_x(display));
}

PHP_FUNCTION(caca_get_mouse_y) {
	caca_display_t *display;
	FETCH_DISPLAY(display);
	RETURN_LONG(caca_get_mouse_y(display));
}

PHP_FUNCTION(caca_get_event_type) {
	caca_event_t *event;
	FETCH_EVENT(event);
	RETURN_LONG(caca_get_event_type(event));
}

PHP_FUNCTION(caca_get_event_key_ch) {
	caca_event_t *event;
	FETCH_EVENT(event);
	RETURN_LONG(caca_get_event_key_ch(event));
}

PHP_FUNCTION(caca_get_event_mouse_button) {
	caca_event_t *event;
	FETCH_EVENT(event);
	RETURN_LONG(caca_get_event_mouse_button(event));
}

PHP_FUNCTION(caca_get_event_mouse_x) {
	caca_event_t *event;
	FETCH_EVENT(event);
	RETURN_LONG(caca_get_event_mouse_x(event));
}

PHP_FUNCTION(caca_get_event_mouse_y) {
	caca_event_t *event;
	FETCH_EVENT(event);
	RETURN_LONG(caca_get_event_mouse_y(event));
}

PHP_FUNCTION(caca_get_event_resize_width) {
	caca_event_t *event;
	FETCH_EVENT(event);
	RETURN_LONG(caca_get_event_resize_width(event));
}

PHP_FUNCTION(caca_get_event_resize_height) {
	caca_event_t *event;
	FETCH_EVENT(event);
	RETURN_LONG(caca_get_event_resize_height(event));
}

