/*
 *  PypyCaca       libcaca Python bindings
 *  Copyright (c) 2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include <Python.h>
#include <caca.h>
#define SET_INTCONSTANT(dict, value) \
     PyDict_SetItemString(dict, #value, PyInt_FromLong((long) value))


PyMODINIT_FUNC initcaca(void);

/* Basic functions */

static PyObject *
pycaca_init(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_display_time(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_display_time(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_width(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_height(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_size(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_width(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_height(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_display_title(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_display_width(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_display_height(PyObject *self, PyObject *args);
static PyObject *
pycaca_refresh(PyObject *self, PyObject *args);
static PyObject *
pycaca_end(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_feature(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_feature(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_feature_name(PyObject *self, PyObject *args);

/* Event handling */
static PyObject *
pycaca_get_event(PyObject *self, PyObject *args);
static PyObject *
pycaca_wait_event(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_mouse_x(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_mouse_y(PyObject *self, PyObject *args);


/* Primitives drawing */
static PyObject *
pycaca_draw_line(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_polyline(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_thin_polyline(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_thin_line(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_circle(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_ellipse(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_thin_ellipse(PyObject *self, PyObject *args);
static PyObject *
pycaca_fill_ellipse(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_box(PyObject *self, PyObject *args);
static PyObject *
pycaca_fill_box(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_thin_box(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_triangle(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_thin_triangle(PyObject *self, PyObject *args);
static PyObject *
pycaca_fill_triangle(PyObject *self, PyObject *args);

/* Charactere drawing */
static PyObject *
pycaca_set_color(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_fg_color(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_bg_color(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_color_name(PyObject *self, PyObject *args);
static PyObject *
pycaca_putchar(PyObject *self, PyObject *args);
static PyObject *
pycaca_putstr(PyObject *self, PyObject *args);
static PyObject *
pycaca_printf(PyObject *self, PyObject *args);
/*static PyObject *
pycaca_get_screen(PyObject *self, PyObject *args);*/
  static PyObject *
pycaca_clear(PyObject *self, PyObject *args);


/* Sprites functions */
static PyObject *
pycaca_load_sprite(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_sprite(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_sprite_frames(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_sprite_width(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_sprite_height(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_sprite_dx(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_sprite_dy(PyObject *self, PyObject *args);
static PyObject *
pycaca_free_sprite(PyObject *self, PyObject *args);



/* Exporters */
static PyObject *
pycaca_get_html(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_html3(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_irc(PyObject *self, PyObject *args);
static PyObject *
pycaca_get_ansi(PyObject *self, PyObject *args);


/* Bitmap functions */
static PyObject *
pycaca_create_bitmap(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_bitmap_palette(PyObject *self, PyObject *args);
static PyObject *
pycaca_set_bitmap_gamma(PyObject *self, PyObject *args);
static PyObject *
pycaca_draw_bitmap(PyObject *self, PyObject *args);
static PyObject *
pycaca_free_bitmap(PyObject *self, PyObject *args);
