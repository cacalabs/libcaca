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

class _Dither(object):
    """ Model for Dither object.
    """
    def __init__(self):
        self._dither = 0

    def from_param(self):
        """ Required by ctypes module to call object as parameter of
            a C function.
        """
        return self._dither

    def __del__(self):
        if self._dither > 0:
            self._free()

    def __str__(self):
        return "<CacaDither>"

    def _free(self):
        """ Free a libcaca dither.
        """
        _lib.caca_free_font.argtypes = [_Dither]
        _lib.caca_free_font.restype = ctypes.c_int

        return _lib.caca_free_font(self)

class Dither(_Dither):
    """ Dither object, methods are libcaca functions with caca_dither_t as first
        argument.
    """
    def __init__(self, bpp, width, height, pitch, rmask, gmask, bmask, amask):
        """ Dither constructor

            bpp     -- bitmap depth in bits per pixels
            width   -- bitmap width in pixels
            height  -- bitmap height in pixels
            pitch   -- bitmap pitch in bytes
            rmask   -- bitmask for red values
            gmask   -- bitmask for green values
            bmask   -- bitmask for blue values
            amask   -- bitmask for alpha values
        """
        pass

