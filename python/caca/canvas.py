# -*- coding: utf-8 -*-
#
# libcaca       Colour ASCII-Art library
#               Python language bindings
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#               All Rights Reserved
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

""" Libcaca Python bindings """

import ctypes
import errno

from caca import _lib, utf8_to_utf32, utf32_to_utf8
from caca import _PYTHON3, _str_to_bytes, _bytes_to_str
from caca.font import _Font

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

class Canvas(_Canvas):
    """ Canvas object, methods are libcaca functions with canvas_t as
        first parameter.
    """
    def __init__(self, width=0, height=0, pointer=None):
        """ Canvas constructor.

            width   -- the desired canvas width
            height  -- the desired canvas height
            pointer -- pointer to libcaca canvas
        """
        _lib.caca_create_canvas.argtypes = [ctypes.c_int, ctypes.c_int]

        if pointer is None:
            try:
                self._cv = _lib.caca_create_canvas(width, height)
            except ctypes.ArgumentError:
                self._cv = 0
                raise CanvasError("Specified width or height is invalid")
            else:
                if self._cv == 0:
                    err = ctypes.c_int.in_dll(_lib, "errno")
                    if err.value == errno.EINVAL:
                        raise CanvasError("Specified width or height is"
                                          " invalid")
                    elif err.value == errno.ENOMEM:
                        raise CanvasError("Not enough memory for the requested"
                                          " canvas size")
                    else:
                        raise CanvasError("Unknown error: failed to create"
                                          " canvas")
        else:
            self._cv = pointer

    def manage(self, *args, **kw):
        """ Not implemented.
        """
        raise CanvasError("Not implemented")

    def unmanage(self, *args, **kw):
        """ Not implemented.
        """
        raise CanvasError("Not implemented")

    def set_size(self, width, height):
        """ Resize a canvas.

            width   -- the desired canvas width
            height  -- the desired canvas height
        """
        _lib.caca_set_canvas_size.argtypes  = [
                _Canvas, ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_set_canvas_size.restype   = ctypes.c_int

        try:
            ret = _lib.caca_set_canvas_size(self, width, height)
        except ctypes.ArgumentError:
            raise CanvasError("Specified width or height is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Specified width or height is invalid")
                elif err.value == errno.EBUSY:
                    raise CanvasError("The canvas is in use by a display driver"
                                      " and cannot be resized")
                elif err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory for the requested"
                                      " canvas size")
            else:
                return ret

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

    def get_chars(self, *args, **kw):
        """ Not implemented.
        """
        raise CanvasError("Not implemented")

    def get_attrs(self, *args, **kw):
        """ Not implemented.
        """
        raise CanvasError("Not implemented")

    def gotoxy(self, x, y):
        """ Set cursor position. Setting the cursor position outside the canvas
            is legal but the cursor will not be shown.

            x   -- X cursor coordinate
            y   -- Y cursor coordinate
        """
        _lib.caca_gotoxy.argtypes = [_Canvas, ctypes.c_int, ctypes.c_int]
        _lib.caca_gotoxy.restyoe  = ctypes.c_int

        try:
            ret = _lib.caca_gotoxy(self, x, y)
        except ctypes.ArgumentError:
            raise CanvasError("specified coordinate X or Y is invalid")
        else:
            return ret

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
        """ Print an ASCII or Unicode character. Return the width of the
            printed character: 2 for a fullwidth character, 1 otherwise.

            x   -- X coordinate
            y   -- Y coordinate
            ch  -- the character to print
        """
        _lib.caca_put_char.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
            ]
        _lib.caca_put_char.restype  = ctypes.c_int

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

            try:
                ret =  _lib.caca_put_char(self, x, y, ch)
            except ctypes.ArgumentError:
                raise CanvasError("specified coordinate X or Y is invalid")
            else:
                return ret

    def get_char(self, x, y):
        """ Get the Unicode character at the given coordinates.

            x   -- X coordinate
            y   -- Y coordinate
        """
        _lib.caca_get_char.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_get_char.restype  = ctypes.c_uint32

        try:
            ch = _lib.caca_get_char(self, x, y)
        except ctypes.ArgumentError:
            raise CanvasError("specified coordinate X or Y is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf32_to_utf8(ch)

            return ch

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

        if _PYTHON3 and isinstance(s, str):
            s = _str_to_bytes(s)

        try:
            ret = _lib.caca_put_str(self, x, y, s)
        except ctypes.ArgumentError:
            raise CanvasError("Invalid argument")
        else:
            return ret

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

        if _PYTHON3 and isinstance(fmt, str):
            fmt = _str_to_bytes(fmt)

        if _PYTHON3:
            nargs = []
            for arg in args[:]:
                if isinstance(arg, str):
                    nargs.append(_str_to_bytes(arg))
                else:
                    nargs.append(arg)
        else:
            nargs = args

        try:
            ret = _lib.caca_printf(self, x, y, fmt, *nargs)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate X or Y is invalid")
        else:
            return ret

    def vprintf(self, *args, **kw):
        """ Not implemented.
        """
        raise CanvasError("Not implemented")

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

        try:
            ret = _lib.caca_set_canvas_handle(self, x, y)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate X or Y is invalid")
        else:
            return ret

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

    def blit(self, x, y, cv, mask=None):
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

        if not isinstance(cv, Canvas):
            raise CanvasError("Specified mask canvas is invalid")
        else:
            if mask is None:
                mask = NullCanvas()
            else:
                if not isinstance(mask, _Canvas):
                    raise CanvasError("Specified mask canvas is invalid")

        try:
            ret = _lib.caca_blit(self, x, y, cv, mask)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate X or Y is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("A mask was specified but the mask size"
                                      " and source canvas size do not match")
            else:
                return ret

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

        try:
            ret = _lib.caca_set_canvas_boundaries(self, x, y, width, height)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate or size is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Specified width or height is invalid")
                elif err.value == errno.EBUSY:
                    raise CanvasError("The canvas is in use by a display driver"
                                      " and cannot be resized")
                elif err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory for the requested"
                                      " canvas size")
            else:
                return ret

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

        ret = _lib.caca_enable_dirty_rect(self)
        if ret == -1:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.EINVAL:
                raise CanvasError("Dirty rectangles were not disabled")
        else:
            return ret

    def get_dirty_rect_count(self):
        """ Get the number of dirty rectangles in the canvas.
        """
        _lib.caca_get_dirty_rect_count.argtypes = [_Canvas]
        _lib.caca_get_dirty_rect_count.restype  = ctypes.c_int

        return _lib.caca_get_dirty_rect_count(self)

    def get_dirty_rect(self, idx):
        """ Get a canvas's dirty rectangle. Return python dictionnary with
            coords as keys: x, y, width, height.

            idx -- the requested rectangle index
        """
        dct = None
        x = ctypes.c_int()
        y = ctypes.c_int()
        width  = ctypes.c_int()
        height = ctypes.c_int()

        _lib.caca_get_dirty_rect.argtypes = [
                _Canvas, ctypes.c_int,
                ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
                ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)
            ]
        _lib.caca_get_dirty_rect.restype  = ctypes.c_int

        try:
            ret = _lib.caca_get_dirty_rect(self, idx, x, y, width, height)
        except ctypes.ArgumentError:
            raise CanvasError("Specified rectangle index is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Specified rectangle index is out of"
                                      " bounds")
            else:
                dct = {
                    'x': x.value, 'y': y.value,
                    'width': width.value, 'height': height.value,
                }
                return dct

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

        try:
            ret =_lib.caca_add_dirty_rect(self, x, y, width, height)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate or size is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Specified rectangle coordinates are out"
                                      " of bounds")
            else:
                return ret

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

        try:
            ret = _lib.caca_remove_dirty_rect(self, x, y, width, height)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate or size is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Specified rectangle coordinates are out"
                                      " of bounds")
            else:
                return ret

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

        ret = _lib.caca_rotate_left(self)
        if ret == -1:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.EBUSY:
                raise CanvasError("The canvas is in use by a display driver"
                                  " and cannot be rotated")
            elif err.value == errno.ENOMEM:
                raise CanvasError("Not enough memory to allocate the new"
                                  " canvas size")
        else:
            return ret

    def rotate_right(self):
        """ Rotate a canvas, 90 degrees clockwise.
        """
        _lib.caca_rotate_right.argtypes = [_Canvas]
        _lib.caca_rotate_right.restype  = ctypes.c_int

        ret = _lib.caca_rotate_right(self)
        if ret == -1:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.EBUSY:
                raise CanvasError("The canvas is in use by a display driver"
                                  " and cannot be rotated")
            elif err.value == errno.ENOMEM:
                raise CanvasError("Not enough memory to allocate the new"
                                  " canvas size")
        else:
            return ret

    def stretch_left(self):
        """ Rotate and stretch a canvas, 90 degrees counterclockwise.
        """
        _lib.caca_stretch_left.argtypes = [_Canvas]
        _lib.caca_stretch_left.restype  = ctypes.c_int

        ret = _lib.caca_stretch_left(self)
        if ret == -1:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.EBUSY:
                raise CanvasError("The canvas is in use by a display driver"
                                  " and cannot be rotated")
            elif err.value == errno.ENOMEM:
                raise CanvasError("Not enough memory to allocate the new"
                                  " canvas size")
        else:
            return ret

    def stretch_right(self):
        """ Rotate and stretch a canvas, 90 degrees clockwise.
        """
        _lib.caca_stretch_right.argtypes = [_Canvas]
        _lib.caca_stretch_right.restype  = ctypes.c_int

        ret = _lib.caca_stretch_right(self)
        if ret == -1:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.EBUSY:
                raise CanvasError("The canvas is in use by a display driver"
                                  " and cannot be rotated")
            elif err.value == errno.ENOMEM:
                raise CanvasError("Not enough memory to allocate the new"
                                  " canvas size")
        else:
            return ret

    def get_attr(self, x, y):
        """ Get the text attribute at the given coordinates.

            x   -- X coordinate
            y   -- Y coordinate
        """
        _lib.caca_get_attr.argtypes = [_Canvas, ctypes.c_int, ctypes.c_int]
        _lib.caca_get_attr.restype  = ctypes.c_uint32

        try:
            ret = _lib.caca_get_attr(self, x, y)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate X or Y is invalid")
        else:
            return ret

    def set_attr(self, attr):
        """ Set the default character attribute.

            attr    -- the requested attribute value
        """
        _lib.caca_set_attr.argtypes = [_Canvas, ctypes.c_uint32]
        _lib.caca_set_attr.restype  = ctypes.c_int

        try:
            ret = _lib.caca_set_attr(self, attr)
        except ctypes.ArgumentError:
            raise CanvasError("Specified attribute is invalid")
        else:
            return ret

    def unset_attr(self, attr):
        """ Unset the default character attribute.

            attr    -- the requested attribute value
        """
        _lib.caca_unset_attr.argtypes = [_Canvas, ctypes.c_uint32]
        _lib.caca_unset_attr.restype  = ctypes.c_int

        try:
            ret = _lib.caca_unset_attr(self, attr)
        except ctypes.ArgumentError:
            raise CanvasError("Specified attribute is invalid")
        else:
            return ret

    def toggle_attr(self, attr):
        """ Toggle the default character attribute.

            attr -- the requested attribute value
        """
        _lib.caca_toggle_attr.argtypes = [_Canvas, ctypes.c_uint32]
        _lib.caca_toggle_attr.restype  = ctypes.c_int

        try:
            ret = _lib.caca_toggle_attr(self, attr)
        except ctypes.ArgumentError:
            raise CanvasError("Specified attribute is invalid")
        else:
            return ret

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

        try:
            ret = _lib.caca_put_attr(self, x, y, attr)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate or attribute is invalid")
        else:
            return ret

    def set_color_ansi(self, fg, bg):
        """ Set the default colour pair for text (ANSI version).

            fg  -- the requested ANSI foreground colour.
            bg  -- the requested ANSI background colour.
        """
        _lib.caca_set_color_ansi.argtypes = [
                _Canvas, ctypes.c_uint8, ctypes.c_uint8
            ]
        _lib.caca_set_color_ansi.restype  = ctypes.c_int

        try:
            ret = _lib.caca_set_color_ansi(self, fg, bg)
        except ctypes.ArgumentError:
            raise CanvasError("At least one of the colour values is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("At least one of the colour values"
                                      " is invalid")
            else:
                return ret

    def set_color_argb(self, fg, bg):
        """ Set the default colour pair for text (truecolor version).

            fg  -- the requested ARGB foreground colour.
            bg  -- the requested ARGB background colour.
        """
        _lib.caca_set_color_argb.argtypes = [
            _Canvas, ctypes.c_uint16, ctypes.c_uint16
        ]
        _lib.caca_set_color_argb.restype  = ctypes.c_int

        try:
            ret = _lib.caca_set_color_argb(self, fg, bg)
        except ctypes.ArgumentError:
            raise CanvasError("At least one of the colour values is invalid")
        else:
            return ret

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

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

            try:
                ret = _lib.caca_draw_line(self, x1, y1, x2, y2, ch)
            except ctypes.ArgumentError:
                raise CanvasError("specified coordinate is invalid")
            else:
                return ret

    def draw_polyline(self, array_xy, ch):
        """ Draw a polyline.

            array_xy -- List of (X, Y) coordinates
            ch       -- character to be used to draw the line
        """
        if not isinstance(array_xy, list) or len(array_xy) < 2:
            raise CanvasError("Specified array of coordinates is invalid")
        else:
            for item in array_xy:
                if not isinstance(item, list) and \
                   not isinstance(item, tuple):
                    raise CanvasError("Specified array of coordinates"
                                      " is invalid")

        ax = ctypes.c_int * len(array_xy)
        ay = ctypes.c_int * len(array_xy)

        _lib.caca_draw_polyline.argtypes = [
                _Canvas, ax, ay, ctypes.c_int, ctypes.c_uint32
            ]
        _lib.caca_draw_polyline.restype  = ctypes.c_int

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

            try:
                ax = ax(*[x[0] for x in array_xy])
                ay = ay(*[y[1] for y in array_xy])
            except IndexError:
                raise CanvasError("Specified array coordinates is invalid")

            try:
                ret = _lib.caca_draw_polyline(self, ax, ay,
                                              len(array_xy) - 1, ch)
            except ctypes.ArgumentError:
                raise CanvasError("specified array of coordinates is invalid")
            else:
                return ret

    def draw_thin_line(self, x1, y1, x2, y2):
        """ Draw a thin line on the canvas, using ASCII art.

            x1  -- X coordinate of the first point
            y1  -- Y coordinate of the first point
            x2  -- X coordinate of the second point
            y2  -- Y coordinate of the second point
        """
        _lib.caca_draw_thin_line.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int,
                ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_draw_thin_line.restype  = ctypes.c_int

        try:
            ret = _lib.caca_draw_thin_line(self, x1, y1, x2, y2)
        except ctypes.ArgumentError:
            raise CanvasError("specified coordinate is invalid")
        else:
            return ret

    def draw_thin_polyline(self, array_xy):
        """ Draw an ASCII art thin polyline.

            array_xy -- Array of (X, Y) coordinates
        """
        if not isinstance(array_xy, list) or len(array_xy) < 2:
            raise CanvasError("Specified array of coordinates is invalid")
        else:
            for item in array_xy:
                if not isinstance(item, list) and \
                   not isinstance(item, tuple):
                    raise CanvasError("Specified array of coordinates"
                                      " is invalid")

        ax = ctypes.c_int * len(array_xy)
        ay = ctypes.c_int * len(array_xy)

        _lib.caca_draw_thin_polyline.argtypes = [
               Canvas, ax, ay, ctypes.c_int
            ]
        _lib.caca_draw_thin_polyline.restype  = ctypes.c_int

        try:
            ax = ax(*[x[0] for x in array_xy])
            ay = ay(*[y[1] for y in array_xy])
        except IndexError:
            raise CanvasError("Specified array coordinates is invalid")

        try:
            ret = _lib.caca_draw_thin_polyline(self, ax, ay, len(array_xy) - 1)
        except ctypes.ArgumentError:
            raise CanvasError("specified array of coordinates is invalid")
        else:
            return ret

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

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

        try:
            ret = _lib.caca_draw_circle(self, x, y, r, ch)
        except ctypes.ArgumentError:
            raise CanvasError("Specified circle coordinate or radius is"
                              " invalid")
        else:
            return ret

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

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

        try:
            ret = _lib.caca_draw_ellipse(self, xo, yo, a, b, ch)
        except ctypes.ArgumentError:
            raise CanvasError("Specified ellipse coordinate or radius is"
                              " invalid")
        else:
            return ret

    def draw_thin_ellipse(self, xo, yo, a, b):
        """ Draw a thin ellipse on the canvas.

            xo  -- center X coordinate
            yo  -- center Y coordinate
            a   -- ellipse X radius
            b   -- ellipse Y radius
        """
        _lib.caca_draw_thin_ellipse.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int,
                ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_draw_thin_ellipse.restype  = ctypes.c_int

        try:
            ret = _lib.caca_draw_thin_ellipse(self, xo, yo, a, b)
        except ctypes.ArgumentError:
            raise CanvasError("Specified ellipse coordinate or radius is"
                              " invalid")
        else:
            return ret

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

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

        try:
            ret = _lib.caca_fill_ellipse(self, xo, yo, a, b, ch)
        except ctypes.ArgumentError:
            raise CanvasError("Specified ellipse coordinate or radius is"
                              " invalid")
        else:
            return ret

    def draw_box(self, x, y, width, height, ch):
        """ Draw a box on the canvas using the given character.

            x       -- X coordinate of the upper-left corner of the box
            y       -- Y coordinate of the upper-left corner of the box
            width   -- width of the box
            height  -- height of the box
            ch      -- character to be used to draw the box
        """
        _lib.caca_draw_box.argtypes = [
                Canvas, ctypes.c_int, ctypes.c_int,
                ctypes.c_int, ctypes.c_int, ctypes.c_uint32
            ]
        _lib.caca_draw_box.restype  = ctypes.c_int

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

        try:
            ret = _lib.caca_draw_box(self, x, y, width, height, ch)
        except ctypes.ArgumentError:
            raise CanvasError("specified box coordinate is invalid")
        else:
            return ret

    def draw_thin_box(self, x, y, width, height):
        """ Draw a thin box on the canvas.

            x       -- X coordinate of the upper-left corner of the box
            y       -- Y coordinate of the upper-left corner of the box
            width   -- width of the box
            height  -- height of the box
        """
        _lib.caca_draw_thin_box.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int,
                ctypes.c_int, ctypes.c_int
            ]
        _lib.caca_draw_thin_box.restype  = ctypes.c_int

        try:
            ret = _lib.caca_draw_thin_box(self, x, y, width, height)
        except ctypes.ArgumentError:
            raise CanvasError("specified box coordinate is invalid")
        else:
            return ret

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

        try:
            ret = _lib.caca_draw_cp437_box(self, x, y, width, height)
        except ctypes.ArgumentError:
            raise CanvasError("specified box coordinate is invalid")
        else:
            return ret

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

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

        try:
            ret = _lib.caca_fill_box(self, x, y, width, height, ch)
        except ctypes.ArgumentError:
            raise CanvasError("specified box coordinate is invalid")
        else:
            return ret

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

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

        try:
            ret = _lib.caca_draw_triangle(self, x1, y1, x2, y2, x3, y3, ch)
        except ctypes.ArgumentError:
            raise CanvasError("specified triangle coordinate is invalid")
        else:
            return ret

    def draw_thin_triangle(self, x1, y1, x2, y2, x3, y3):
        """ Draw a thin triangle on the canvas.

            x1  -- X coordinate of the first point
            y1  -- Y coordinate of the first point
            x2  -- X coordinate of the second point
            y2  -- Y coordinate of the second point
            x3  -- X coordinate of the third point
            y3  -- Y coordinate of the third point
        """
        _lib.caca_draw_thin_triangle.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_draw_thin_triangle.restype  = ctypes.c_int

        try:
            ret = _lib.caca_draw_thin_triangle(self, x1, y1, x2, y2, x3, y3)
        except ctypes.ArgumentError:
            raise CanvasError("specified triangle coordinate is invalid")
        else:
            return ret

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

        if not isinstance(ch, str):
            raise CanvasError("Specified character is invalid")
        else:
            try:
                ch = ord(ch)
            except TypeError:
                ch = utf8_to_utf32(ch)

        try:
            ret = _lib.caca_fill_triangle(self, x1, y1, x2, y2, x3, y3, ch)
        except ctypes.ArgumentError:
            raise CanvasError("specified triangle coordinate is invalid")
        else:
            return ret

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

        try:
            ret = _lib.caca_set_frame(self, idx)
        except ctypes.ArgumentError:
            raise CanvasError("specified index is invalid")
        else:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.EINVAL:
                raise CanvasError("Requested frame is out of range")
            else:
                return ret

    def get_frame_name(self):
        """ Get the current frame's name.
        """
        _lib.caca_get_frame_name.argtypes = [_Canvas]
        _lib.caca_get_frame_name.restype  = ctypes.c_char_p

        if _PYTHON3:
            return _bytes_to_str(_lib.caca_get_frame_name(self))
        else:
            return _lib.caca_get_frame_name(self)

    def set_frame_name(self, name):
        """ Set the current frame's name.

            name    -- the name to give to the current frame
        """
        _lib.caca_set_frame_name.argtypes = [_Canvas, ctypes.c_char_p]
        _lib.caca_set_frame_name.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(name, str):
            name = _str_to_bytes(name)

        try:
            ret = _lib.caca_set_frame_name(self, name)
        except ctypes.ArgumentError:
            raise CanvasError("Specified name is invalid")
        else:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.ENOMEM:
                raise CanvasError("Not enough memory to allocate new frame")
            else:
                return ret

    def create_frame(self, idx):
        """ Add a frame to a canvas.

            idx -- the index where to insert the new frame
        """
        _lib.caca_create_frame.argtypes = [_Canvas, ctypes.c_int]
        _lib.caca_create_frame.restype  = ctypes.c_int

        try:
            ret = _lib.caca_create_frame(self, idx)
        except ctypes.ArgumentError:
            raise CanvasError("specified index is invalid")
        else:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.ENOMEM:
                raise CanvasError("Not enough memory to allocate new frame")
            else:
                return ret

    def free_frame(self, idx):
        """ Remove a frame from a canvas.

            idx -- the index of the frame to delete
        """
        _lib.caca_free_frame.argtypes = [_Canvas, ctypes.c_int]
        _lib.caca_free_frame.restype  = ctypes.c_int

        try:
            ret = _lib.caca_free_frame(self, idx)
        except ctypes.ArgumentError:
            raise CanvasError("specified index is invalid")
        else:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.EINVAL:
                raise CanvasError("Requested frame is out of range, or attempt"
                                  " to delete the last frame of the canvas")
            else:
                return ret

    def import_from_memory(self, data, fmt):
        """ Import a memory buffer into a canvas.

            data -- a memory area containing the data to be loaded into
                    the canvas
            fmt  -- a string describing the input format

            Valid values for format are:
              - "": attempt to autodetect the file format.
              - caca: import native libcaca files.
              - text: import ASCII text files.
              - ansi: import ANSI files.
              - utf8: import UTF-8 files with ANSI colour codes.
        """

        _lib.caca_import_canvas_from_memory.argtypes = [
                Canvas, ctypes.c_char_p,
                ctypes.c_size_t, ctypes.c_char_p
            ]
        _lib.caca_import_canvas_from_memory.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(data, str):
            data = _str_to_bytes(data)
        if _PYTHON3 and isinstance(fmt, str):
            fmt = _str_to_bytes(fmt)

        length = ctypes.c_size_t(len(data))

        try:
            ret = _lib.caca_import_canvas_from_memory(self, data, length, fmt)
        except ctypes.ArgumentError:
            raise CanvasError("Given data are invalid")
        else:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if ret == -1:
                if err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory to allocate canvas")
                elif err.value == errno.EINVAL:
                    raise CanvasError("Invalid format requested")
            else:
                return ret

    def import_from_file(self, filename, fmt):
        """ Import a file into a canvas.

            filename -- the name of the file to load
            fmt      -- a string describing the input format

            Valid values for format are:
              - "": attempt to autodetect the file format.
              - caca: import native libcaca files.
              - text: import ASCII text files.
              - ansi: import ANSI files.
              - utf8: import UTF-8 files with ANSI colour codes.
        """
        _lib.caca_import_canvas_from_file.argtypes = [
            _Canvas, ctypes.c_char_p, ctypes.c_char_p
        ]
        _lib.caca_import_canvas_from_file.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(filename, str):
            filename = _str_to_bytes(filename)
        if _PYTHON3 and isinstance(fmt, str):
            fmt = _str_to_bytes(fmt)

        try:
            ret = _lib.caca_import_canvas_from_file(self, filename, fmt)
        except ctypes.ArgumentError:
            raise CanvasError("Specified filename is invalid")
        else:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if ret == -1:
                if err.value == errno.ENOSYS:
                    raise CanvasError("File access is not implemented on this"
                                      " system")
                elif err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory to allocate canvas")
                elif err.value == errno.EINVAL:
                    raise CanvasError("Invalid format requested")
            else:
                return ret

    def import_area_from_memory(self, x, y, data, fmt):
        """ Import a memory buffer into a canvas area.

            x    -- the leftmost coordinate of the area to import to
            y    -- the topmost coordinate of the area to import to
            data -- a memory area containing the data to be loaded into
                    the canvas
            fmt  -- a string describing the input format

            Valid values for format are:
              - "": attempt to autodetect the file format.
              - caca: import native libcaca files.
              - text: import ASCII text files.
              - ansi: import ANSI files.
              - utf8: import UTF-8 files with ANSI colour codes.
        """
        length = ctypes.c_size_t(len(data))

        _lib.caca_import_area_from_memory.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int,
                ctypes.c_char_p, ctypes.c_size_t, ctypes.c_char_p
            ]
        _lib.caca_import_area_from_memory.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(data, str):
            data = _str_to_bytes(data)
        if _PYTHON3 and isinstance(fmt, str):
            fmt = _str_to_bytes(fmt)

        try:
            ret = _lib.caca_import_area_from_memory(self, x, y,
                                                    data, length, fmt)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate X or Y is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Unsupported format requested or"
                                      " invalid coordinates")
                elif err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory to allocate canvas")
            else:
                return ret

    def import_area_from_file(self, x, y, filename, fmt):
        """ Import a file into a canvas area.

            x        -- the leftmost coordinate of the area to import to
            y        -- the topmost coordinate of the area to import to
            filename -- the name of the file to be load
            fmt      -- a string describing the input format

            Valid values for format are:
              - "": attempt to autodetect the file format.
              - caca: import native libcaca files.
              - text: import ASCII text files.
              - ansi: import ANSI files.
              - utf8: import UTF-8 files with ANSI colour codes.
        """
        _lib.caca_import_area_from_file.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int,
                ctypes.c_char_p, ctypes.c_char_p
            ]
        _lib.caca_import_area_from_file.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(filename, str):
            filename = _str_to_bytes(filename)
        if _PYTHON3 and isinstance(fmt, str):
            fmt = _str_to_bytes(fmt)

        try:
            ret = _lib.caca_import_area_from_file(self, x, y, filename, fmt)
        except ctypes.ArgumentError:
            raise CanvasError("Specified coordinate X or Y is invalid")
        else:
            if ret == -1:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.ENOSYS:
                    raise CanvasError("File access is not implemented on this"
                                      " system")
                elif err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory to allocate canvas")
                elif err.value == errno.EINVAL:
                    raise CanvasError("Unsupported format requested or"
                                      " invalid coordinates")
            else:
                return ret

    def export_to_memory(self, fmt):
        """ Export a canvas into a foreign format.

            fmt -- a string describing the output format

            Valid values for format are:
              - caca: export native libcaca files.
              - ansi: export ANSI art (CP437 charset with ANSI colour codes).
              - html: export an HTML page with CSS information.
              - html3: export an HTML table that should be compatible with
                       most navigators, including textmode ones.
              - irc: export UTF-8 text with mIRC colour codes.
              - ps: export a PostScript document.
              - svg: export an SVG vector image.
              - tga: export a TGA image.
        """
        p_size_t = ctypes.POINTER(ctypes.c_size_t)
        _lib.caca_export_canvas_to_memory.argtypes = [
                _Canvas, ctypes.c_char_p, p_size_t
            ]
        _lib.caca_export_canvas_to_memory.restype  = ctypes.POINTER(
                                                        ctypes.c_char_p)

        p = ctypes.c_size_t()

        if _PYTHON3 and isinstance(fmt, str):
            fmt = _str_to_bytes(fmt)

        try:
            ret = _lib.caca_export_canvas_to_memory(self, fmt, p)
        except ctypes.ArgumentError:
            raise CanvasError("Invalid format requested")
        else:
            if not ret:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Invalid format requested")
                elif err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory to allocate output"
                                      " buffer")
            else:
                if _PYTHON3:
                    return _bytes_to_str(ctypes.string_at(ret, p.value))
                else:
                    return ctypes.string_at(ret, p.value)

    def export_area_to_memory(self, x, y, width, height, fmt):
        """ Export a canvas portion into a foreign format.

            x       -- the leftmost coordinate of the area to export
            y       -- the topmost coordinate of the area to export
            width   -- the width of the area to export
            height  -- the height of the area to export
            fmt     -- a string describing the output format

            Valid values for format are:
              - caca: export native libcaca files.
              - ansi: export ANSI art (CP437 charset with ANSI colour codes).
              - html: export an HTML page with CSS information.
              - html3: export an HTML table that should be compatible with
                       most navigators, including textmode ones.
              - irc: export UTF-8 text with mIRC colour codes.
              - ps: export a PostScript document.
              - svg: export an SVG vector image.
              - tga: export a TGA image.
        """
        p_size_t = ctypes.POINTER(ctypes.c_size_t)

        _lib.caca_export_area_to_memory.argtypes = [
                _Canvas, ctypes.c_int, ctypes.c_int, ctypes.c_int,
                ctypes.c_int, ctypes.c_char_p, p_size_t
            ]
        _lib.caca_export_area_to_memory.restype  = ctypes.POINTER(ctypes.c_char_p)

        p = ctypes.c_size_t()

        if _PYTHON3 and isinstance(fmt, str):
            fmt = _str_to_bytes(fmt)

        try:
            ret = _lib.caca_export_area_to_memory(self, x, y, width, height,
                                                  fmt, p)
        except ctypes.ArgumentError:
            raise CanvasError("Requested area coordinate is invalid")
        else:
            if not ret:
                err = ctypes.c_int.in_dll(_lib, "errno")
                if err.value == errno.EINVAL:
                    raise CanvasError("Invalid format requested")
                elif err.value == errno.ENOMEM:
                    raise CanvasError("Not enough memory to allocate output"
                                      " buffer")
            else:
                if _PYTHON3:
                    return _bytes_to_str(ctypes.string_at(ret, p.value))
                else:
                    return ctypes.string_at(ret, p.value)

    def set_figfont(self, filename):
        """ Load a figfont and attach it to a canvas.

            filename    -- the figfont file to load.
        """
        _lib.caca_canvas_set_figfont.argtypes = [_Canvas, ctypes.c_char_p]
        _lib.caca_canvas_set_figfont.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(filename, str):
            filename = _str_to_bytes(filename)

        return _lib.caca_canvas_set_figfont(self, filename)

    def put_figchar(self, ch):
        """ Paste a character using the current figfont.

            ch  -- the character to paste
        """
        _lib.caca_put_figchar.argtypes = [_Canvas, ctypes.c_uint32]
        _lib.caca_put_figchar.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(ch, str):
            ch = _str_to_bytes(ch)

        try:
            ch = ord(ch)
        except TypeError:
            ch = utf8_to_utf32(ch)

        return _lib.caca_put_figchar(self, ch)

    def flush_figlet(self):
        """ Flush the figlet context
        """
        _lib.caca_flush_figlet.argtypes = [_Canvas]
        _lib.caca_flush_figlet.restype  = ctypes.c_int

        return _lib.caca_flush_figlet(self)

    def render(self, font, buf, width, height, pitch):
        """ Render the canvas onto an image buffer.

            font    -- a Font() object
            buf     -- the image buffer
            width   -- the width (in pixels) of the image
            heigth  -- the height (in pixels) of the image
            pitch   -- the pitch (in bytes) of the image
        """
        _lib.caca_render_canvas.argtypes = [
            _Canvas, _Font, ctypes.c_char_p,
            ctypes.c_int, ctypes.c_int, ctypes.c_int
        ]
        _lib.caca_render_canvas.restype  = ctypes.c_int

        return _lib.caca_render_canvas(self, font, buf, width, height, pitch)

class NullCanvas(_Canvas):
    """ Represent a NULL canvas_t, eg to use as canvas mask for blit operations.
    """
    def __str__(self):
        return "<NullCanvas>"

class CanvasError(Exception):
    pass

