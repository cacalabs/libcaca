# -*- coding: utf-8 -*-
#
# libcaca       Colour ASCII-Art library
#               Python language bindings
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#               All Rights Reserved
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What The Fuck You Want
# To Public License, Version 2, as published by Sam Hocevar. See
# http://sam.zoy.org/wtfpl/COPYING for more details.
#

""" Libcaca Python bindings """

import ctypes

from caca import _lib

class _Canvas(object):
    """ Model for Canvas objects.
    """

    def __init__(self):
        self._cv = 0

    def from_param(self):
        """ Required by ctypes module to call object as parameter of
            a C function.
        """
        return self._cv

    def __str__(self):
        return "<CacaCanvas %dx%d>" % (self.get_width(), self.get_height())

    def __del__(self):
        if self._cv > 0:
            self._free()

    def _free(self):
        """ Free a libcaca canvas.
        """
        _lib.caca_free_canvas.argtypes = [_Canvas]
        _lib.caca_free_canvas.restype  = ctypes.c_int

        return _lib.caca_free_canvas(self)

    def get_width(self):
        raise CanvasError, "You can't use model canvas directly"

    def get_height(self):
        raise CanvasError, "You can't use model canvas directly"

class Canvas(_Canvas):
    """ Canvas object, methods are libcaca functions with canvas_t as
        first parameter.
    """
    def __init__(self, width=0, height=0):
        """ Canvas constructor.

            width   -- the desired canvas width
            height  -- the desired canvas height
        """
        _lib.caca_create_canvas.argtypes = [ctypes.c_int, ctypes.c_int]

        self._cv = _lib.caca_create_canvas(width, height)
        if self._cv == 0:
            raise CanvasError, "Failed to create canvas"

    def set_size(self, width, height):
        """ Resize a canvas.

            width   -- the desired canvas width
            height  -- the desired canvas height
        """
        _lib.caca_set_canvas_size.argtypes  = [
            _Canvas, ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_set_canvas_size.restype   = ctypes.c_int

        return _lib.caca_set_canvas_size(self, width, height)

    def get_width(self):
        """ Get the canvas width.
        """
        _lib.caca_get_canvas_width.argtypes = [_Canvas]
        _lib.caca_get_canvas_width.restype  = ctypes.c_int

        return _lib.caca_get_canvas_width(self)

    def get_height(self):
        """ Get the canvas height.
        """
        _lib.caca_get_canvas_height.argtypes = [_Canvas]
        _lib.caca_get_canvas_height.restype  = ctypes.c_int

        return _lib.caca_get_canvas_height(self)

    def get_chars(self):
        """ Get the canvas character array, return python list.
        """
        chlist = []
        #get canvas size
        w, h = self.get_width(), self.get_height()

        _lib.caca_get_canvas_chars.argtypes = [_Canvas]
        _lib.caca_get_canvas_chars.restype  = \
                    ctypes.POINTER(ctypes.c_uint8 * (w * h))

        plist = _lib.caca_get_canvas_chars(self)

        #build character list
        for item in plist.contents:
            if item != 0:
                chlist.append(chr(item))

        return chlist

    def gotoxy(self, x, y):
        """ Set cursor position.

            x   -- X cursor coordinate
            y   -- Y cursor coordinate
        """
        _lib.caca_gotoxy.argtypes = [_Canvas, ctypes.c_int]
        _lib.caca_gotoxy.restyoe  = ctypes.c_int

        return _lib.caca_gotoxy(self, x, y)

    def wherex(self):
        """ Get X cursor position.
        """
        _lib.caca_wherex.argtypes = [_Canvas]
        _lib.caca_wherex.restype  = ctypes.c_int

        return _lib.caca_wherex(self)

    def wherey(self):
        """ Get Y cursor position.
        """
        _lib.caca_wherey.argtypes = [_Canvas]
        _lib.caca_wherey.restype  = ctypes.c_int

        return _lib.caca_wherey(self)

    def put_char(self, x, y, ch):
        """ Print an ASCII or Unicode character.

            x   -- X coordinate
            y   -- Y coordinate
            ch  -- the character to print
        """
        _lib.caca_put_char.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
            ]
        _lib.caca_put_char.restype  = ctypes.c_int

        return _lib.caca_put_char(self, x, y, ord(ch))

    def get_char(self, x, y):
        """ Get the Unicode character at the given coordinates.

            x   -- X coordinate
            y   -- Y coordinate
        """
        _lib.caca_get_char.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_get_char.restype  = ctypes.c_uint32

        return _lib.caca_get_char(self, x, y)

    def put_str(self, x, y, s):
        """ Print a string.

            x   -- X coordinate
            y   -- Y coordinate
            s   -- the string to print
        """
        _lib.caca_put_str.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_char_p
            ]
        _lib.caca_put_str.restype  = ctypes.c_int

        return _lib.caca_put_str(self, x, y, s)

    def printf(self, x, y, fmt, *args):
        """ Print a formated string.

            x       -- X coordinate
            y       -- Y coordinate
            fmt     -- the format string to print
            args    -- Arguments to the format string
        """
        _lib.caca_printf.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_char_p
            ]
        _lib.caca_printf.restype  = ctypes.c_int

        return _lib.caca_printf(self, x, y, fmt, *args)

    def clear(self):
        """ Clear the canvas.
        """
        _lib.caca_clear_canvas.argtypes = [_Canvas]
        _lib.caca_clear_canvas.restype  = ctypes.c_int

        return _lib.caca_clear_canvas(self)

    def set_handle(self, x, y):
        """ Set cursor handle. Blitting method will use the handle value to
            put the canvas at the proper coordinates.

            x   -- X handle coordinate
            y   -- Y handle coordinate
        """
        _lib.caca_set_canvas_handle.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_set_canvas_handle.restype  = ctypes.c_int

        return _lib.caca_set_canvas_handle(self, x, y)

    def get_handle_x(self):
        """ Get X handle position.
        """
        _lib.caca_get_canvas_handle_x.argtypes = [_Canvas]
        _lib.caca_get_canvas_handle_x.restype  = ctypes.c_int

        return _lib.caca_get_canvas_handle_x(self)

    def get_handle_y(self):
        """ Get Y handle position.
        """
        _lib.caca_get_canvas_handle_y.argtypes = [_Canvas]
        _lib.caca_get_canvas_handle_y.restype  = ctypes.c_int

        return _lib.caca_get_canvas_handle_y(self)

    def blit(self, x, y, cv, mask):
        """ Blit canvas onto another one.

            x       -- X coordinate
            y       -- Y coordinate
            cv      -- the source canvas
            mask    -- the mask canvas
        """
        _lib.caca_blit.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, _Canvas, _Canvas
            ]
        _lib.caca_blit.restype  = ctypes.c_int

        return _lib.caca_blit(self, x, y, cv, mask)

    def set_boundaries(self, x, y, width, height):
        """ Set a canvas' new boundaries.

            x       -- X coordinate of the top-left corner
            y       -- Y coordinate of the top-left corner
            width   -- width of the box
            height  -- height of the box
        """
        _lib.caca_set_canvas_boundaries.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_set_canvas_boundaries.restype  = ctypes.c_int

        return _lib.caca_set_canvas_boundaries(self, x, y, width, height)

    def disable_dirty_rect(self):
        """ Disable dirty rectangles.
        """
        _lib.caca_disable_dirty_rect.argtypes = [_Canvas]
        _lib.caca_disable_dirty_rect.restype  = ctypes.c_int

        return _lib.caca_disable_dirty_rect(self)

    def enable_dirty_rect(self):
        """ Enable dirty rectangles.
        """
        _lib.caca_enable_dirty_rect.argtypes = [_Canvas]
        _lib.caca_enable_dirty_rect.restype  = ctypes.c_int

        return _lib.caca_enable_dirty_rect(self)

    def get_dirty_rect_count(self):
        """ Get the number of dirty rectangles in the canvas.
        """
        _lib.caca_get_dirty_rect_count.argtypes = [_Canvas]
        _lib.caca_get_dirty_rect_count.restype  = ctypes.c_int

        return _lib.caca_get_dirty_rect_count(self)

    def get_dirty_rect(self, idx):
        """ Get a canvas's dirty rectangle.

            idx -- the requested rectangle index
        """
        x = ctypes.POINTER(ctypes.c_int)
        y = ctypes.POINTER(ctypes.c_int)
        w = ctypes.POINTER(ctypes.c_int)
        h = ctypes.POINTER(ctypes.c_int)

        _lib.caca_get_dirty_rect.argtypes = [
            _Canvas, ctypes.c_int,
            ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
            ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)
            ]
        _lib.caca_get_dirty_rect.restype  = ctypes.c_int

        _lib.caca_get_dirty_rect(self, idx, x, y, w, h)

        return [x.contents.value, y.contents.value,
                w.contents.value, h.contents.value]

    def add_dirty_rect(self, x, y, width, height):
        """ Add an area to the canvas's dirty rectangle list.

            x       -- the leftmost edge of the additional dirty rectangle
            y       -- the topmost edge of the additional dirty rectangle
            width   -- the width of the additional dirty rectangle
            height  -- the height of the additional dirty rectangle
        """
        _lib.caca_add_dirty_rect.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_add_dirty_rect.restype  = ctypes.c_int

        return _lib.caca_add_dirty_rect(self, x, y, width, height)

    def remove_dirty_rect(self, x, y, width, height):
        """ Remove an area from the dirty rectangle list.

            x       -- the leftmost edge of the additional dirty rectangle
            y       -- the topmost edge of the additional dirty rectangle
            width   -- the width of the additional rectangle
            height  -- the height of the additional dirty rectangle
        """
        _lib.caca_remove_dirty_rect.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_remove_dirty_rect.restype  = ctypes.c_int

        return _lib.caca_remove_dirty_rect(self, x, y, height, width)

    def clear_dirty_rect_list(self):
        """ Clear a canvas's dirty rectangle list.
        """
        _lib.caca_clear_dirty_rect_list.argtypes = [_Canvas]
        _lib.caca_clear_dirty_rect_list.restype  = ctypes.c_int

        return _lib.caca_clear_dirty_rect_list(self)

    def invert(self):
        """ Invert a canvas' colours.
        """
        _lib.caca_invert.argtypes = [_Canvas]
        _lib.caca_invert.restype  = ctypes.c_int

        return _lib.caca_invert(self)

    def flip(self):
        """ Flip a canvas horizontally.
        """
        _lib.caca_flip.argtypes = [_Canvas]
        _lib.caca_flip.restype  = ctypes.c_int

        return _lib.caca_flip(self)

    def flop(self):
        """ Flip a canvas vertically.
        """
        _lib.caca_flop.argtypes = [_Canvas]
        _lib.caca_flop.restype  = ctypes.c_int

        return _lib.caca_flop(self)

    def rotate_180(self):
        """ Rotate a canvas.
        """
        _lib.caca_rotate_180.argtypes = [_Canvas]
        _lib.caca_rotate_180.restype  = ctypes.c_int

        return _lib.caca_rotate_180(self)

    def rotate_left(self):
        """ Rotate a canvas, 90 degrees counterclockwise.
        """
        _lib.caca_rotate_left.argtypes = [_Canvas]
        _lib.caca_rotate_left.restype  = ctypes.c_int

        return _lib.caca_rotate_left(self)

    def rotate_right(self):
        """ Rotate a canvas, 90 degrees clockwise.
        """
        _lib.caca_rotate_right.argtypes = [_Canvas]
        _lib.caca_rotate_right.restype  = ctypes.c_int

        return _lib.caca_rotate_right(self)

    def stretch_left(self):
        """ Rotate and stretch a canvas, 90 degrees counterclockwise.
        """
        _lib.caca_stretch_left.argtypes = [_Canvas]
        _lib.caca_stretch_left.restype  = ctypes.c_int

        return _lib.caca_stretch_left(self)

    def stretch_right(self):
        """ Rotate and stretch a canvas, 90 degrees clockwise.
        """
        _lib.caca_stretch_right.argtypes = [_Canvas]
        _lib.caca_stretch_right.restype  = ctypes.c_int

        return _lib.caca_stretch_right(self)

    def get_attr(self, x, y):
        """ Get the text attribute at the given coordinates.

            x   -- X coordinate
            y   -- Y coordinate
        """
        _lib.caca_get_attr.argtypes = [_Canvas, ctypes.c_int, ctypes.c_int]
        _lib.caca_get_attr.restype  = ctypes.c_uint32
        return _lib.caca_get_attr(self, x, y)

    def set_attr(self, attr):
        """ Set the default character attribute.

            attr    -- the requested attribute value
        """
        _lib.caca_set_attr.argtypes = [_Canvas, ctypes.c_uint32]
        _lib.caca_set_attr.restype  = ctypes.c_int

        return _lib.caca_set_attr(self, attr)

    def put_attr(self, x, y, attr):
        """ Set the character attribute at the given coordinates.

            x       -- X coordinate
            y       -- Y coordinate
            attr    -- the requested attribute value
        """
        _lib.caca_put_attr.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_put_attr.restype  = ctypes.c_int

        return _lib.caca_put_attr(self, x, y, attr)

    def set_color_ansi(self, fg, bg):
        """ Set the default colour pair for text (ANSI version).

            fg  -- the requested ANSI foreground colour.
            bg  -- the requested ANSI background colour.
        """
        _lib.caca_set_color_ansi.argtypes = [_Canvas, ctypes.c_uint8, ctypes.c_uint8]
        _lib.caca_set_color_ansi.restype  = ctypes.c_int

        return _lib.caca_set_color_ansi(self, fg, bg)

    def set_color_argb(self, fg, bg):
        """ Set the default colour pair for text (truecolor version).

            fg  -- the requested ARGB foreground colour.
            bg  -- the requested ARGB background colour.
        """
        _lib.caca_set_color_argb.argtypes = [
            _Canvas, ctypes.c_uint16, ctypes.c_uint16
        ]
        _lib.caca_set_color_argb.restype  = ctypes.c_int

        return _lib.caca_set_color_argb(self, fg, bg)

    def draw_line(self, x1, y1, x2, y2, ch):
        """ Draw a line on the canvas using the given character.

            x1  -- X coordinate of the first point
            y1  -- Y coordinate of the first point
            x2  -- X coordinate of the second point
            y2  -- Y coordinate of the second point
            ch  -- character to be used to draw the line
        """
        _lib.caca_draw_line.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_draw_line.restype  = ctypes.c_int

        return _lib.caca_draw_line(self, x1, y1, x2, y2, ord(ch))

    def draw_polyline(self, array_x, array_y, n, ch):
        """ Draw a polyline.

            array_x -- Array of X coordinates, must have n+1 elements
            array-y -- Array of Y coordinates, must have n+1 elements
            n       -- Number of lines to draw
            ch      -- character to be used to draw the line
        """
        _lib.caca_draw_polyline.argtypes = [
            _Canvas, ctypes.c_int * n, ctypes.c_int * n, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_draw_polyline.restype  = ctypes.c_int

        return _lib.caca_draw_polyline(self, array_x, array_y, n, ord(ch))

    def draw_thin_line(self, x1, y1, x2, y2):
        """ Draw a thin line on the canvas, using ASCII art.

            x1  -- X coordinate of the first point
            y1  -- Y coordinate of the first point
            x2  -- X coordinate of the second point
            y2  -- Y coordinate of the second point
        """
        _lib.caca_draw_thin_line.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_draw_thin_line.restype  = ctypes.c_int

        return _lib.caca_draw_thin_line(self, x1, y1, x2, y2)

    def draw_thin_polyline(self, array_x, array_y, n):
        """ Draw an ASCII art thin polyline.

            array_x -- Array of X coordinates, must have n+1 elements
            array_y -- Array of Y coordinates, must have n+1 elements
            n       -- Number of lines to draw
        """
        _lib.caca_draw_thin_polyline.argtypes = [
            Canvas, ctypes.c_int * n, ctypes.c_int * n, ctypes.c_int
        ]
        _lib.caca_draw_thin_polyline.restype  = ctypes.c_int

        return _lib.caca_draw_thin_polyline(self, array_x, array_y, n)

    def draw_circle(self, x, y, r, ch):
        """ Draw a circle on the canvas using the given character.

            x   -- center X coordinate
            y   -- center Y coordinate
            r   -- circle radius
            ch  -- the UTF-32 character to be used to draw the circle outline
        """
        _lib.caca_draw_circle.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_draw_circle.restype  = ctypes.c_int

        return _lib.caca_draw_circle(self, x, y, r, ord(ch))

    def draw_ellipse(self, xo, yo, a, b, ch):
        """ Draw an ellipse on the canvas using the given character.

            xo  -- center X coordinate
            yo  -- center Y coordinate
            a   -- ellipse x radius
            b   -- ellipse y radius
            ch  -- UTF-32 character to be used to draw the ellipse outline
        """
        _lib.caca_draw_ellipse.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_draw_ellipse.restype  = ctypes.c_int

        return _lib.caca_draw_ellipse(self, xo, yo, a, b, ord(ch))

    def draw_thin_ellipse(self, xo, yo, a, b):
        """ Draw a thin ellipse on the canvas.

            xo  -- center X coordinate
            yo  -- center Y coordinate
            a   -- ellipse X radius
            b   -- ellipse Y radius
        """
        _lib.caca_draw_thin_ellipse.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_draw_thin_ellipse.restype  = ctypes.c_int

        return _lib.caca_draw_thin_ellipse(self, xo, yo, a, b)

    def fill_ellipse(self, xo, yo, a, b, ch):
        """ Fill an ellipse on the canvas using the given character.

            xo  -- center X coordinate
            yo  -- center Y coordinate
            a   -- ellipse X radius
            b   -- ellipse Y radius
            ch  -- UTF-32 character to be used to fill the ellipse
        """
        _lib.caca_fill_ellipse.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_fill_ellipse.restype  = ctypes.c_int

        return _lib.caca_fill_ellipse(self, xo, yo, a, b, ord(ch))

    def draw_box(self, x, y, width, height, ch):
        """ Draw a box on the canvas using the given character.

            x       -- X coordinate of the upper-left corner of the box
            y       -- Y coordinate of the upper-left corner of the box
            width   -- width of the box
            height  -- height of the box
            ch      -- character to be used to draw the box
        """
        _lib.caca_draw_box.argtypes = [
            Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_draw_box.restype  = ctypes.c_int

        return _lib.caca_draw_box(self, x, y, width, height, ord(ch))

    def draw_thin_box(self, x, y, width, height):
        """ Draw a thin box on the canvas.

            x       -- X coordinate of the upper-left corner of the box
            y       -- Y coordinate of the upper-left corner of the box
            width   -- width of the box
            height  -- height of the box
        """
        _lib.caca_draw_thin_box.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_draw_thin_box.restype  = ctypes.c_int

        return _lib.caca_draw_thin_box(self, x, y, width, height)

    def draw_cp437_box(self, x, y, width, height):
        """ Draw a box on the canvas using CP437 characters.

            x       -- X coordinate of the upper-left corner box
            y       -- Y coordinate of the upper-left corner box
            width   -- width of the box
            height  -- height of the box
        """
        _lib.caca_draw_cp437_box.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_draw_cp437_box.restype  = ctypes.c_int

        return _lib.caca_draw_cp437_box(self, x, y, width, height)

    def fill_box(self, x, y, width, height, ch):
        """ Fill a box on the canvas using the given character.

            x       -- X coordinate of the upper-left corner of the box
            y       -- Y coordinate of the upper-left corner of the box
            width   -- width of the box
            height  -- height of the box
            ch      -- UFT-32 character to be used to fill the box
        """
        _lib.caca_fill_box.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_fill_box.restype  = ctypes.c_int

        return _lib.caca_fill_box(self, x, y, width, height, ord(ch))

    def draw_triangle(self, x1, y1, x2, y2, x3, y3, ch):
        """ Draw a triangle on the canvas using the given character.

            x1  -- X coordinate of the first point
            y1  -- Y coordinate of the first point
            x2  -- X coordinate of the second point
            y2  -- Y coordinate of the second point
            x3  -- X coordinate of the third point
            y3  -- Y coordinate of the third point
            ch  -- UTF-32 character to be used to draw the triangle outline
        """
        _lib.caca_draw_triangle.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
        ]
        _lib.caca_draw_triangle.restype  = ctypes.c_int

        return _lib.caca_draw_triangle(self, x1, y1, x2, y2, x3, y3, ord(ch))

    def draw_thin_triangle(self, x1, y1, x2, y2, x3, y3):
        """ Draw a thin triangle on the canvas.
        """
        _lib.caca_draw_thin_triangle.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_draw_thin_triangle.restype  = ctypes.c_int

        return _lib.caca_draw_thin_triangle(self, x1, y1, x2, y2, x3, y3)

    def fill_triangle(self, x1, y1, x2, y2, x3, y3, ch):
        """ Fill a triangle on the canvas using the given character.

            x1  -- X coordinate of the first point
            y1  -- Y coordinate of the first point
            x2  -- X coordinate of the second point
            y2  -- Y coordinate of the second point
            x3  -- X coordinate of the second point
            y3  -- Y coordinate of the second point
            ch  -- UTF-32 character to be used to fill the triangle
        """
        _lib.caca_fill_triangle.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_fill_triangle.restype  = ctypes.c_int

        return _lib.caca_fill_triangle(self, x1, y1, x2, y2, x3, y3, ord(ch))

    def fill_triangle_textured(self, coords, tex, uv):
        """ Fill a triangle on the canvas using an arbitrary-sized texture.

            coords  -- coordinates of the triangle (3{x,y})
            tex     -- the handle of the canvas texture
            uv      -- coordinates of the texture  (3{u,v})
        """
        _lib.caca_fill_triangle_textured.argtypes = [
            _Canvas, ctypes.c_int * 6, _Canvas, ctypes.c_int * 6
        ]
        _lib.caca_fill_triangle_textured.restype  = ctypes.c_int

        return _lib.caca_fill_triangle_textured(self, coords, tex, uv)

    def get_frame_count(self):
        """ Get the number of frames in a canvas.
        """
        _lib.caca_get_frame_count.argtypes = [_Canvas]
        _lib.caca_get_frame_count.restype  = ctypes.c_int

        return _lib.caca_get_frame_count(self)

    def set_frame(self, idx):
        """ Activate a given canvas frame.

            idx -- the canvas frame to activate
        """
        _lib.caca_set_frame.argtypes = [_Canvas, ctypes.c_int]
        _lib.caca_set_frame.restype  = ctypes.c_int

        return _lib.caca_set_frame(self, idx)

    def get_frame_name(self):
        """ Get the current frame's name.
        """
        _lib.caca_get_frame_name.argtypes = [_Canvas]
        _lib.caca_get_frame_name.restype  = ctypes.c_char_p

        return _lib.caca_get_frame_name(self)

    def set_frame_name(self, name):
        """ Set the current frame's name.

            name    -- the name to give to the current frame
        """
        _lib.caca_set_frame_name.argtypes = [_Canvas, ctypes.c_char_p]
        _lib.caca_set_frame_name.restype  = ctypes.c_int

        return _lib.caca_set_frame_name(self, name)

    def create_frame(self, idx):
        """ Add a frame to a canvas.

            idx -- the index where to insert the new frame
        """
        _lib.caca_create_frame.argtypes = [_Canvas, ctypes.c_int]
        _lib.caca_create_frame.restype  = ctypes.c_int

        return _lib.caca_create_frame(self, idx)

    def free_frame(self, idx):
        """ Remove a frame from a canvas.

            idx -- the index of the frame to delete
        """
        _lib.caca_free_frame.argtypes = [_Canvas, ctypes.c_int]
        _lib.caca_free_frame.restype  = ctypes.c_int

        return _lib.caca_free_frame(self, idx)

    def import_from_memory(self, data, length, fmt):
        """ Import a memory buffer into the given libcaca canvas's current frame.
        The current frame is resized accordingly and its contents are replaced
        with the imported data.

            data    -- a memory area containing the data to be loaded into the canvas
            length  -- the size in bytes of the memory area
            fmt     -- a string describing the input format
        """

        _lib.caca_import_canvas_from_memory.argtypes = [
            Canvas, ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p
        ]
        _lib.caca_import_canvas_from_memory.restype  = ctypes.c_int

        return _lib.caca_import_canvas_from_memory(self, data, length, fmt)

class NullCanvas(_Canvas):
    """ Represent a NULL canvas_t, eg to use as canvas mask for blit operations.
    """
    def __str__(self):
        return "<NullCanvas>"

class CanvasError(Exception):
    pass
