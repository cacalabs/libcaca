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

#ifndef PHP_CACA_H
#define PHP_CACA_H

#include <caca.h>

#define PHP_CACA_VERSION "0.0"
#define PHP_CACA_EXTNAME "caca"

#define PHP_CACA_CANVAS_RES_NAME "caca_canvas"
#define PHP_CACA_DITHER_RES_NAME "caca_dither"
#define PHP_CACA_FONT_RES_NAME "caca_font"
#define PHP_CACA_FILE_RES_NAME "caca_file"
#define PHP_CACA_DISPLAY_RES_NAME "caca_display"
#define PHP_CACA_EVENT_RES_NAME "caca_event"

int le_caca_canvas;
int le_caca_dither;
int le_caca_font;
int le_caca_file;
int le_caca_display;
int le_caca_event;

PHP_MINIT_FUNCTION(caca);
PHP_MINFO_FUNCTION(caca);

PHP_FUNCTION(caca_create_canvas);
PHP_FUNCTION(caca_manage_canvas);
PHP_FUNCTION(caca_unmanage_canvas);
PHP_FUNCTION(caca_set_canvas_size);
PHP_FUNCTION(caca_get_canvas_width);
PHP_FUNCTION(caca_get_canvas_height);
PHP_FUNCTION(caca_get_canvas_chars);
PHP_FUNCTION(caca_get_canvas_attrs);
PHP_FUNCTION(caca_rand);
PHP_FUNCTION(caca_get_version);
PHP_FUNCTION(caca_gotoxy);
PHP_FUNCTION(caca_get_cursor_x);
PHP_FUNCTION(caca_get_cursor_y);
PHP_FUNCTION(caca_put_char);
PHP_FUNCTION(caca_get_char);
PHP_FUNCTION(caca_put_str);
PHP_FUNCTION(caca_printf);
PHP_FUNCTION(caca_clear_canvas);
PHP_FUNCTION(caca_set_canvas_handle);
PHP_FUNCTION(caca_get_canvas_handle_x);
PHP_FUNCTION(caca_get_canvas_handle_y);
PHP_FUNCTION(caca_blit);
PHP_FUNCTION(caca_set_canvas_boundaries);
PHP_FUNCTION(caca_invert);
PHP_FUNCTION(caca_flip);
PHP_FUNCTION(caca_flop);
PHP_FUNCTION(caca_rotate_180);
PHP_FUNCTION(caca_rotate_left);
PHP_FUNCTION(caca_rotate_right);
PHP_FUNCTION(caca_stretch_left);
PHP_FUNCTION(caca_stretch_right);
PHP_FUNCTION(caca_get_attr);
PHP_FUNCTION(caca_set_attr);
PHP_FUNCTION(caca_put_attr);
PHP_FUNCTION(caca_set_color_ansi);
PHP_FUNCTION(caca_set_color_argb);
PHP_FUNCTION(caca_attr_to_argb64);
PHP_FUNCTION(caca_utf32_to_ascii);
PHP_FUNCTION(caca_utf32_is_fullwidth);
PHP_FUNCTION(caca_draw_line);
PHP_FUNCTION(caca_draw_polyline);
PHP_FUNCTION(caca_draw_thin_line);
PHP_FUNCTION(caca_draw_thin_polyline);
PHP_FUNCTION(caca_draw_circle);
PHP_FUNCTION(caca_draw_ellipse);
PHP_FUNCTION(caca_draw_thin_ellipse);
PHP_FUNCTION(caca_fill_ellipse);
PHP_FUNCTION(caca_draw_box);
PHP_FUNCTION(caca_draw_thin_box);
PHP_FUNCTION(caca_draw_cp437_box);
PHP_FUNCTION(caca_fill_box);
PHP_FUNCTION(caca_draw_triangle);
PHP_FUNCTION(caca_draw_thin_triangle);
PHP_FUNCTION(caca_fill_triangle);
PHP_FUNCTION(caca_get_frame_count);
PHP_FUNCTION(caca_set_frame);
PHP_FUNCTION(caca_get_frame_name);
PHP_FUNCTION(caca_set_frame_name);
PHP_FUNCTION(caca_create_frame);
PHP_FUNCTION(caca_free_frame);
PHP_FUNCTION(caca_set_dither_palette);
PHP_FUNCTION(caca_set_dither_brightness);
PHP_FUNCTION(caca_get_dither_brightness);
PHP_FUNCTION(caca_set_dither_gamma);
PHP_FUNCTION(caca_get_dither_gamma);
PHP_FUNCTION(caca_set_dither_contrast);
PHP_FUNCTION(caca_get_dither_contrast);
PHP_FUNCTION(caca_set_dither_antialias);
PHP_FUNCTION(caca_get_dither_antialias_list);
PHP_FUNCTION(caca_get_dither_antialias);
PHP_FUNCTION(caca_set_dither_color);
PHP_FUNCTION(caca_get_dither_color_list);
PHP_FUNCTION(caca_get_dither_color);
PHP_FUNCTION(caca_set_dither_charset);
PHP_FUNCTION(caca_get_dither_charset_list);
PHP_FUNCTION(caca_get_dither_charset);
PHP_FUNCTION(caca_set_dither_algorithm);
PHP_FUNCTION(caca_get_dither_algorithm_list);
PHP_FUNCTION(caca_get_dither_algorithm);
PHP_FUNCTION(caca_dither_bitmap);
PHP_FUNCTION(caca_get_font_list);
PHP_FUNCTION(caca_get_font_width);
PHP_FUNCTION(caca_get_font_height);
PHP_FUNCTION(caca_render_canvas);
PHP_FUNCTION(caca_canvas_set_figfont);
PHP_FUNCTION(caca_put_figchar);
PHP_FUNCTION(caca_flush_figlet);
PHP_FUNCTION(caca_file_close);
PHP_FUNCTION(caca_file_gets);
PHP_FUNCTION(caca_file_eof);
PHP_FUNCTION(caca_get_import_list);
PHP_FUNCTION(caca_export_memory);
PHP_FUNCTION(caca_get_export_list);
PHP_FUNCTION(caca_get_display_driver_list);
PHP_FUNCTION(caca_get_display_driver);
PHP_FUNCTION(caca_set_display_driver);
PHP_FUNCTION(caca_refresh_display);
PHP_FUNCTION(caca_set_display_time);
PHP_FUNCTION(caca_get_display_time);
PHP_FUNCTION(caca_get_display_width);
PHP_FUNCTION(caca_get_display_height);
PHP_FUNCTION(caca_set_display_title);
PHP_FUNCTION(caca_set_mouse);
PHP_FUNCTION(caca_set_cursor);
PHP_FUNCTION(caca_get_event);
PHP_FUNCTION(caca_get_mouse_x);
PHP_FUNCTION(caca_get_mouse_y);
PHP_FUNCTION(caca_get_event_type);
PHP_FUNCTION(caca_get_event_key_ch);
PHP_FUNCTION(caca_get_event_key_utf8);
PHP_FUNCTION(caca_get_event_mouse_button);
PHP_FUNCTION(caca_get_event_mouse_x);
PHP_FUNCTION(caca_get_event_mouse_y);
PHP_FUNCTION(caca_get_event_resize_width);
PHP_FUNCTION(caca_get_event_resize_height);

extern zend_module_entry caca_module_entry;
#define phpext_caca_ptr &caca_module_entry

#endif
