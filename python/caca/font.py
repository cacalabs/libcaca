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
        pass

