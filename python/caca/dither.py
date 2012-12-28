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

from caca import _lib
from caca.canvas import _Canvas

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
        _lib.caca_free_dither.argtypes = [_Dither]
        _lib.caca_free_dither.restype = ctypes.c_int

        return _lib.caca_free_dither(self)

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
        _lib.caca_create_dither.argtypes = [
            ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
            ctypes.c_uint, ctypes.c_uint, ctypes.c_uint, ctypes.c_uint,
        ]

        self._dither = _lib.caca_create_dither(bpp, width, height, pitch,
                                               rmask, gmask, bmask, amask)

        if self._dither == 0:
            raise DitherError("Failed to create dither object")

    def set_palette(self, red, green, blue, alpha):
        """ Set the palette of an 8 bits per pixel bitmap. Values should be
        between 0 and 4095 (0xfff).

            red     -- array of 256 red values
            green   -- array of 256 green values
            blue    -- array of 256 blue values
            alpha   -- array of 256 alpha values
        """
        raise DitherError("Not implemented")

    def set_brightness(self, brightness):
        """ Set the brightness of the dither object.

            brightness  -- brightness value
        """
        if isinstance(brightness, int):
            brightness = float(brightness)

        _lib.caca_set_dither_brightness.argtypes = [_Dither, ctypes.c_float]
        _lib.caca_set_dither_brightness.restype  = ctypes.c_int

        return _lib.caca_set_dither_brightness(self, brightness)

    def get_brightness(self):
        """ Get the brightness of the dither object.
        """
        _lib.caca_get_dither_brightness.argtypes = [_Dither]
        _lib.caca_get_dither_brightness.restype  = ctypes.c_float

        return _lib.caca_get_dither_brightness(self)

    def set_gamma(self, gamma):
        """ Set the gamma of the dither object. A negative value causes colour
        inversion.

            gamma -- gamma value
        """
        if isinstance(gamma, int):
            gamma = float(gamma)

        _lib.caca_set_dither_gamma.argtypes = [_Dither, ctypes.c_float]
        _lib.caca_set_dither_gamma.restype  = ctypes.c_int

        return _lib.caca_set_dither_gamma(self, gamma)

    def get_gamma(self):
        """ Get the gamma of the dither object.
        """
        _lib.caca_get_dither_gamma.argtypes = [_Dither]
        _lib.caca_get_dither_gamma.restype  = ctypes.c_float

        return _lib.caca_get_dither_gamma(self)

    def set_contrast(self, contrast):
        """ Set the contrast of dither.

            contrast -- contrast value
        """
        if isinstance(contrast, int):
            contrast = float(contrast)

        _lib.caca_set_dither_contrast.argtypes = [_Dither, ctypes.c_float]
        _lib.caca_set_dither_contrast.restype  = ctypes.c_int

        return _lib.caca_set_dither_contrast(self, contrast)

    def get_contrast(self):
        """ Get the contrast of the dither object.
        """
        _lib.caca_get_dither_contrast.argtypes = [_Dither]
        _lib.caca_get_dither_contrast.restype  = ctypes.c_float

        return _lib.caca_get_dither_contrast(self)

    def set_antialias(self, value):
        """ Set dither antialiasing.

            value -- A string describing the antialiasing method that will
                     be used for the dithering.

                     + "none": no antialiasing
                     + "prefilter" or "default": simple prefilter antialiasing. (default)
        """
        _lib.caca_set_dither_antialias.argtypes = [_Dither, ctypes.c_char_p]
        _lib.caca_set_dither_antialias.restype  = ctypes.c_int

        return _lib.caca_set_dither_antialias(self, value)

    def get_antialias(self):
        """ Return the dither's current antialiasing method.
        """
        _lib.caca_get_dither_antialias.argtypes = [_Dither]
        _lib.caca_get_dither_antialias.restype  = ctypes.c_char_p

        return _lib.caca_get_dither_antialias(self)

    def get_antialias_list(self):
        """ Get available antialiasing methods.
        """
        lst = []

        _lib.caca_get_dither_antialias_list.argtypes = [_Dither]
        _lib.caca_get_dither_antialias_list.restype  = ctypes.POINTER(ctypes.c_char_p)

        for item in _lib.caca_get_dither_antialias_list(self):
            if item is not None and item != "":
                lst.append(item)
            else:
                #memory occurs otherwise
                break

        return lst

    def set_color(self, value):
        """ Choose colours used for dithering.

            value -- A string describing the colour set that will be
                     used for the dithering.

                     + "mono": use light gray on a black background
                     + "gray": use white and two shades of gray on a black background
                     + "8": use the 8 ANSI colours on a black background
                     + "16": use the 16 ANSI colours on a black background
                     + "fullgray": use black, white and two shades of gray
                       for both the characters and the background
                     + "full8": use the 8 ANSI colours for both the characters and
                       the background
                     + "full16" or "default": use the 16 ANSI colours for both the
                       characters and the background (default)
        """
        _lib.caca_set_dither_color.argtypes = [_Dither, ctypes.c_char_p]
        _lib.caca_set_dither_color.restype  = ctypes.c_int

        return _lib.caca_set_dither_color(self, value)

    def get_color(self):
        """ Get current colour mode.
        """
        _lib.caca_get_dither_color.argtypes = [_Dither]
        _lib.caca_get_dither_color.restype  = ctypes.c_char_p

        return _lib.caca_get_dither_color(self)

    def get_color_list(self):
        """ Get available colour modes.
        """
        lst = []

        _lib.caca_get_dither_color_list.argtypes = [_Dither]
        _lib.caca_get_dither_color_list.restype  = ctypes.POINTER(ctypes.c_char_p)

        for item in _lib.caca_get_dither_color_list(self):
            if item is not None and item != "":
                lst.append(item)
            else:
                #memory occurs otherwise
                break

        return lst

    def set_charset(self, value):
        """ Choose characters used for dithering.

            value -- A string describing the characters that need to be
                     used for the dithering.

                     + "ascii" or "default": use only ASCII characters (default).
                     + "shades": use Unicode characters "U+2591 LIGHT SHADE",
                       "U+2592 MEDIUM SHADE" and "U+2593 DARK SHADE". These characters are
                       also present in the CP437 codepage available on DOS and VGA.
                     + "blocks": use Unicode quarter-cell block combinations.
                       These characters are only found in the Unicode set.
        """
        _lib.caca_set_dither_charset.argtypes = [_Dither, ctypes.c_char_p]
        _lib.caca_set_dither_charset.restype  = ctypes.c_int

        return _lib.caca_set_dither_charset(self, value)

    def get_charset(self):
        """ Get current character set.
        """
        _lib.caca_get_dither_charset.argtypes = [_Dither]
        _lib.caca_get_dither_charset.restype  = ctypes.c_char_p

        return _lib.caca_get_dither_charset(self)

    def get_charset_list(self):
        """ Get available dither character sets.
        """
        lst = []

        _lib.caca_get_dither_color_list.argtypes = [_Dither]
        _lib.caca_get_dither_color_list.restype  = ctypes.POINTER(ctypes.c_char_p)

        for item in _lib.caca_get_dither_color_list(self):
            if item is not None and item != "":
                lst.append(item)
            else:
                #memory occurs otherwise
                break

        return lst

    def set_algorithm(self, value):
        """ Set dithering algorithm.

            value -- A string describing the algorithm that needs to be
                     used for the dithering.

                     + "none": no dithering is used, the nearest matching colour is used.
                     + "ordered2": use a 2x2 Bayer matrix for dithering.
                     + "ordered4": use a 4x4 Bayer matrix for dithering.
                     + "ordered8": use a 8x8 Bayer matrix for dithering.
                     + "random": use random dithering.
                     + "fstein": use Floyd-Steinberg dithering (default).
        """
        _lib.caca_set_dither_algorithm.argtypes = [_Dither, ctypes.c_char_p]
        _lib.caca_set_dither_algorithm.restype  = ctypes.c_int

        return _lib.caca_set_dither_algorithm(self, value)

    def get_algorithm(self):
        """ Get dithering algorithms.
        """
        _lib.caca_get_dither_algorithm.argtypes = [_Dither]
        _lib.caca_get_dither_algorithm.restype  = ctypes.c_char_p

        return _lib.caca_get_dither_algorithm(self)

    def get_algorithm_list(self):
        """ Get dithering algorithms.
        """
        lst = []

        _lib.caca_get_dither_color_list.argtypes = [_Dither]
        _lib.caca_get_dither_color_list.restype  = ctypes.POINTER(ctypes.c_char_p)

        for item in _lib.caca_get_dither_color_list(self):
            if item is not None and item != "":
                lst.append(item)
            else:
                #memory occurs otherwise
                break

        return lst

    def bitmap(self, canvas, x, y, width, height, pixels):
        """ Dither a bitmap on the canvas.

            canvas  -- a handle to libcaca canvas
            x       -- X coordinate of the upper-left corner of the drawing area
            y       -- Y coordinate of the upper-left corner of the drawing area
            width   -- width of the drawing area
            height  -- height of the drawing area
            pixels  -- bitmap's pixels
        """
        _lib.caca_dither_bitmap.argtypes = [
            _Canvas, ctypes.c_int, ctypes.c_int,
            ctypes.c_int, ctypes.c_int, _Dither,
            ctypes.c_char_p
        ]
        _lib.caca_dither_bitmap.restype  = ctypes.c_int

        return _lib.caca_dither_bitmap(canvas, x, y, width, height, self, pixels)

class DitherError(Exception):
    pass

