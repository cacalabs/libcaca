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

from caca import _lib, _PYTHON3, _bytes_to_str

#color constants
COLOR_BLACK = 0x00
COLOR_BLUE = 0x01
COLOR_GREEN = 0x02
COLOR_CYAN = 0x03
COLOR_RED = 0x04
COLOR_MAGENTA = 0x05
COLOR_BROWN = 0x06
COLOR_LIGHTGRAY = 0x07
COLOR_DARKGRAY = 0x08
COLOR_LIGHTBLUE = 0x09
COLOR_LIGHTGREEN = 0x0a
COLOR_LIGHTCYAN = 0x0b
COLOR_LIGHTRED = 0x0c
COLOR_LIGHTMAGENTA = 0x0d
COLOR_YELLOW = 0x0e
COLOR_WHITE = 0x0f
COLOR_DEFAULT = 0x10
COLOR_TRANSPARENT = 0x20

#styles constants
STYLE_BOLD = 0x01
STYLE_ITALICS = 0x02
STYLE_UNDERLINE = 0x04
STYLE_BLINK = 0x08

#key constants
EVENT_NONE = 0x0000
EVENT_KEY_PRESS = 0x0001
EVENT_KEY_RELEASE = 0x0002
EVENT_MOUSE_PRESS = 0x0004
EVENT_MOUSE_RELEASE = 0x0008
EVENT_MOUSE_MOTION = 0x0010
EVENT_RESIZE = 0x0020
EVENT_QUIT = 0x0040
EVENT_ANY = 0xffff

#event constants
KEY_UNKNOWN = 0x00
KEY_CTRL_A = 0x01
KEY_CTRL_B = 0x02
KEY_CTRL_C = 0x03
KEY_CTRL_D = 0x04
KEY_CTRL_E = 0x05
KEY_CTRL_F = 0x06
KEY_CTRL_G = 0x07
KEY_BACKSPACE = 0x08
KEY_TAB = 0x09
KEY_CTRL_J = 0x0a
KEY_CTRL_K = 0x0b
KEY_CTRL_L = 0x0c
KEY_RETURN = 0x0d
KEY_CTRL_N = 0x0e
KEY_CTRL_O = 0x0f
KEY_CTRL_P = 0x10
KEY_CTRL_Q = 0x11
KEY_CTRL_R = 0x12
KEY_PAUSE = 0x13
KEY_CTRL_T = 0x14
KEY_CTRL_U = 0x15
KEY_CTRL_V = 0x16
KEY_CTRL_W = 0x17
KEY_CTRL_X = 0x18
KEY_CTRL_Y = 0x19
KEY_CTRL_Z = 0x1a
KEY_ESCAPE = 0x1b
KEY_DELETE = 0x7f
KEY_UP = 0x111
KEY_DOWN = 0x112
KEY_LEFT = 0x113
KEY_RIGHT = 0x114
KEY_INSERT = 0x115
KEY_HOME = 0x116
KEY_END = 0x117
KEY_PAGEUP = 0x118
KEY_PAGEDOWN = 0x119
KEY_F1 = 0x11a
KEY_F2 = 0x11b
KEY_F3 = 0x11c
KEY_F4 = 0x11d
KEY_F5 = 0x11e
KEY_F6 = 0x11f
KEY_F7 = 0x120
KEY_F8 = 0x121
KEY_F9 = 0x122
KEY_F10 = 0x123
KEY_F11 = 0x124
KEY_F12 = 0x125
KEY_F13 = 0x126
KEY_F14 = 0x127
KEY_F15 = 0x128


def get_version():
    """ Return string with libcaca version information.
    """
    _lib.caca_get_version.restype = ctypes.c_char_p

    if _PYTHON3:
        return _bytes_to_str(_lib.caca_get_version())
    else:
        return _lib.caca_get_version()

def get_display_driver_list():
    """ Return a list of available drivers as tuple (name, description).
    """
    tmplst = []
    retlst = []

    _lib.caca_get_display_driver_list.restype = ctypes.POINTER(ctypes.c_char_p)

    for item in _lib.caca_get_display_driver_list():
        if item is not None and item != "":
            if _PYTHON3:
                tmplst.append(_bytes_to_str(item))
            else:
                tmplst.append(item)
        else:
            #memory error occured otherwise
            break

    for i in range(0, len(tmplst)):
        if i % 2 == 0:
            retlst.append((tmplst[i], tmplst[i+1]))

    del tmplst
    return retlst

def get_export_list():
    """ Return list of available export formats as tuple (name, description).
    """
    tmplst = []
    retlst = []

    _lib.caca_get_export_list.restype = ctypes.POINTER(ctypes.c_char_p)

    for item in _lib.caca_get_export_list():
        if item is not None and item != "":
            if _PYTHON3:
                tmplst.append(_bytes_to_str(item))
            else:
                tmplst.append(item)
        else:
            #memory error occured otherwise
            break

    for i in range(0, len(tmplst)):
        if i % 2 == 0:
            retlst.append((tmplst[i], tmplst[i+1]))

    del tmplst
    return retlst

def get_import_list():
    """ Return list of available import formats as tuple (name, description).
    """
    tmplst = []
    retlst = []

    _lib.caca_get_import_list.restype = ctypes.POINTER(ctypes.c_char_p)

    autodetect = False
    for item in _lib.caca_get_import_list():
        if item is not None:
            if item == "":
                if not autodetect:
                    if _PYTHON3:
                        tmplst.append(_bytes_to_str("\"\""))
                    else:
                        tmplst.append("\"\"")
                    autodetect = True
                else:
                    #memory error occured otherwise
                    break
            else:
                if _PYTHON3:
                    tmplst.append(_bytes_to_str(item))
                else:
                    tmplst.append(item)
        else:
            #memory error occured otherwise
            break

    for i in range(0, len(tmplst)):
        if i % 2 == 0:
            retlst.append((tmplst[i], tmplst[i+1]))

    del tmplst
    return retlst

def get_font_list():
    """ Return a list of available fonts.
    """
    fl = []

    _lib.caca_get_font_list.restype = ctypes.POINTER(ctypes.c_char_p)

    for item in _lib.caca_get_font_list():
        if item is not None and item != "":
            if _PYTHON3:
                fl.append(_bytes_to_str(item))
            else:
                fl.append(item)
        else:
            #memory error occured otherwise
            break

    return fl

def rand(range_min, range_max):
    """ Generate a random integer within a range.

        range_min   -- the lower bound of the integer range
        range_max   __ the upper bound of the integer range
    """
    _lib.caca_rand.argtypes = [ctypes.c_int, ctypes.c_int]
    _lib.caca_rand.restype  = ctypes.c_int

    return _lib.caca_rand(range_min, range_max)

def attr_to_ansi(attr):
    """ Get DOS ANSI information from attribute.

        attr    -- the requested attribute value
    """
    _lib.caca_attr_to_ansi.argtypes = [ctypes.c_uint32]
    _lib.caca_attr_to_ansi.restype  = ctypes.c_uint8

    return _lib.caca_attr_to_ansi(attr)

def attr_to_ansi_fg(attr):
    """ Get ANSI foreground information from attribute.

        attr    -- the requested attribute value
    """
    _lib.caca_attr_to_ansi_fg.argtypes = [ctypes.c_uint32]
    _lib.caca_attr_to_ansi_fg.restype  = ctypes.c_uint8

    return _lib.caca_attr_to_ansi_fg(attr)

def attr_to_ansi_bg(attr):
    """ Get ANSI background information from attribute.

        attr    -- the requested attribute value
    """
    _lib.caca_attr_to_ansi_bg.argtypes = [ctypes.c_uint32]
    _lib.caca_attr_to_ansi_bg.restype  = ctypes.c_uint8

    return _lib.caca_attr_to_ansi_bg(attr)

def attr_to_rgb12_fg(attr):
    """ Get 12-bit RGB foreground information from attribute.

        attr    -- the requested attribute value
    """
    _lib.caca_attr_to_rgb12_fg.argtypes = [ctypes.c_uint32]
    _lib.caca_attr_to_rgb12_fg.restype  = ctypes.c_uint16

    return _lib.caca_attr_to_rgb12_fg(attr)

def attr_to_rgb12_bg(attr):
    """ Get 12-bit RGB background information from attribute.

        attr    -- the requested attribute value
    """
    _lib.caca_attr_to_rgb12_bg.argtypes = [ctypes.c_uint32]
    _lib.caca_attr_to_rgb12_bg.restype  = ctypes.c_uint16

    return _lib.caca_attr_to_rgb12_bg(attr)

def utf8_to_utf32(ch):
    """ Convert a UTF-8 character to UTF-32.

        ch  -- the character to convert
    """
    _lib.caca_utf8_to_utf32.argtypes = [ctypes.c_char_p,
            ctypes.POINTER(ctypes.c_size_t)
        ]
    _lib.caca_utf8_to_utf32.restype  = ctypes.c_uint32

    return _lib.caca_utf8_to_utf32(ch, ctypes.c_ulong(0))

def utf32_to_utf8(ch):
    """ Convert a UTF-32 character to UTF-8.

        ch  -- the character to convert
    """
    _lib.caca_utf32_to_utf8.argtypes = [ctypes.c_char_p, ctypes.c_uint32]
    _lib.caca_utf32_to_utf8.restype  = ctypes.c_int

    buf = ctypes.c_buffer(7)
    _lib.caca_utf32_to_utf8(buf, ch)

    if _PYTHON3:
        return _bytes_to_str(buf.raw).replace('\x00', '')
    else:
        return buf.raw.replace('\x00', '')

def utf32_to_cp437(ch):
    """ Convert a UTF-32 character to CP437.

        ch  -- the character to convert
    """
    _lib.caca_utf32_to_cp437.argtypes = [ctypes.c_uint32]
    _lib.caca_utf32_to_cp437.restype  = ctypes.c_uint8

    return _lib.caca_utf32_to_cp437(ch)

def cp437_to_utf32(ch):
    """ Convert a CP437 character to UTF-32.

        ch  -- the character to convert
    """
    _lib.caca_cp437_to_utf8.argtypes = [ctypes.c_uint8]
    _lib.caca_cp437_to_utf8.restype  = ctypes.c_uint32

    return _lib.caca_cp437_to_utf8(ch)

def utf32_to_ascii(ch):
    """ Convert a UTF-32 character to ASCII.

        ch  -- the character to convert
    """
    _lib.caca_utf32_to_ascii.argtypes = [ctypes.c_uint32]
    _lib.caca_utf32_to_ascii.restype  = ctypes.c_uint8

    return _lib.caca_utf32_to_ascii(ch)

def utf32_is_fullwidth(ch):
    """ Tell whether a UTF-32 character is fullwidth.

        ch  -- the UTF-32 character
    """
    _lib.caca_utf32_is_fullwidth.argtypes = [ctypes.c_uint32]
    _lib.caca_utf32_is_fullwidth.restype  = ctypes.c_int

    return _lib.caca_utf32_is_fullwidth(ch)

