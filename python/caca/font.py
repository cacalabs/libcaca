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

from caca import _lib, _PYTHON3, _str_to_bytes

class _Font(object):
    """ Model for Font object.
    """
    def __init__(self):
        self._font = 0

    def from_param(self):
        """ Required by ctypes module to call object as parameter of
            a C function.
        """
        return self._font

    def __del__(self):
        if hasattr(self, "_font"):
            if self._font > 0:
                self._free()

    def __str__(self):
        return "<CacaFont>"

    def _free(self):
        """ Free a libcaca font.
        """
        _lib.caca_free_font.argtypes = [_Font]
        _lib.caca_free_font.restype = ctypes.c_int

        return _lib.caca_free_font(self)

class Font(_Font):
    """ Font object, methods are libcaca functions with caca_font_t as first
        argument.
    """
    def __init__(self, font, size=0):
        """ Font constructor

            font    -- the memory area containing the font or its name
            size    -- the size of the memory area, or 0 if the font name is given
        """
        if size == 0:
            _lib.caca_load_font.argtypes = [ctypes.c_char_p, ctypes.c_int]
        else:
            raise FontError("Unsupported method")

        _lib.caca_load_font.restype = ctypes.c_int

        if _PYTHON3:
            font = _str_to_bytes(font)

        self._font = _lib.caca_load_font(font, size)
        if self._font == 0:
            err = ctypes.c_int.in_dll(_lib, "errno")
            if err.value == errno.ENOENT:
                raise FontError("Requested built-in font does not exist")
            elif err.value == errno.EINVAL:
                raise FontError("Invalid font data in memory area")
            elif err.value == errno.ENOMEM:
                raise FontError("Not enough memory to allocate font structure")

    def get_width(self):
        """ Get a font's standard glyph width.
        """
        _lib.caca_get_font_width.argtypes = [_Font]
        _lib.caca_get_font_width.restype  = ctypes.c_int

        return _lib.caca_get_font_width(self)

    def get_height(self):
        """ Get a font's standard glyph height.
        """
        _lib.caca_get_font_height.argtypes = [_Font]
        _lib.caca_get_font_height.restype  = ctypes.c_int

        return _lib.caca_get_font_height(self)

    def get_blocks(self):
        """ Get a font's list of supported glyphs.
        """
        raise FontError("Not Implemented")

class FontError(Exception):
    pass

