# -*- coding: utf8 -*-
#
# libcaca            Colour ASCII Art library
#                    Python language bindings
# Copyright (c) 2012 Alex Foulon <alxf@lavabit.com>
#                    All Rights Reserved
#
# This program is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

""" Test module """

#standard module
import unittest

#test modules
from . import canvas

#create modules test suite
canvas_t = unittest.TestSuite()

#define tests for canvas_t test suite
canvas_t.addTest(canvas.CanvasTestCase('test_create'))
canvas_t.addTest(canvas.CanvasTestCase('test_get_width'))
canvas_t.addTest(canvas.CanvasTestCase('test_get_height'))
canvas_t.addTest(canvas.CanvasTestCase('test_set_size'))
canvas_t.addTest(canvas.CanvasTestCase('test_get_char'))
canvas_t.addTest(canvas.CanvasTestCase('test_put_char'))
canvas_t.addTest(canvas.CanvasTestCase('test_put_str'))
canvas_t.addTest(canvas.CanvasTestCase('test_printf'))
canvas_t.addTest(canvas.CanvasTestCase('test_wherex'))
canvas_t.addTest(canvas.CanvasTestCase('test_wherey'))
canvas_t.addTest(canvas.CanvasTestCase('test_gotoxy'))
canvas_t.addTest(canvas.CanvasTestCase('test_clear'))
canvas_t.addTest(canvas.CanvasTestCase('test_get_handle_x'))
canvas_t.addTest(canvas.CanvasTestCase('test_get_handle_y'))
canvas_t.addTest(canvas.CanvasTestCase('test_set_handle'))
canvas_t.addTest(canvas.CanvasTestCase('test_blit'))
canvas_t.addTest(canvas.CanvasTestCase('test_blit_mask'))
canvas_t.addTest(canvas.CanvasTestCase('test_set_boundaries'))
canvas_t.addTest(canvas.CanvasTestCase('test_enable_dirty_rect'))
canvas_t.addTest(canvas.CanvasTestCase('test_add_dirty_rect'))
canvas_t.addTest(canvas.CanvasTestCase('test_remove_dirty_rect'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_line'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_thin_line'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_polyline'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_thin_polyline'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_circle'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_ellipse'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_thin_ellipse'))
canvas_t.addTest(canvas.CanvasTestCase('test_fill_ellipse'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_box'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_thin_box'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_cp437_box'))
canvas_t.addTest(canvas.CanvasTestCase('test_fill_box'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_triangle'))
canvas_t.addTest(canvas.CanvasTestCase('test_draw_thin_triangle'))
canvas_t.addTest(canvas.CanvasTestCase('test_fill_triangle'))

#configure all tests in a single suite
alltests = unittest.TestSuite([canvas_t])

