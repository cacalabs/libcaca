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

from caca import _lib, _PYTHON3, _str_to_bytes
from caca.canvas import _Canvas, Canvas

class _Display(object):
    """ Model for Display objects.
    """
    def from_param(self):
        """ Required by ctypes module to call object as parameter of
            a C function.
        """
        return self._dp

    def __str__(self):
        return "<CacaDisplay>"

    def __del__(self):
        if self._dp > 0:
            self._free()

    def _free(self):
        """ Free a libcaca display.
        """
        _lib.caca_free_display.argtypes = [_Display]
        _lib.caca_free_display.restype  = ctypes.c_int

        return _lib.caca_free_display(self)

class Display(_Display):
    """ Display objects, methods are libcaca functions with display_t as first
        parameter.
    """
    def __init__(self, cv, driver=None):
        """ Display constructor.

            cv      -- canvas to attach.
            driver  -- caca driver to set with display
        """

        if driver is None:
            _lib.caca_create_display.argtypes = [_Canvas]
            self._dp = _lib.caca_create_display(cv)
        else:
            _lib.caca_create_display_with_driver.argtypes = [
                _Canvas, ctypes.c_char_p
            ]
            if _PYTHON3 and isinstance(driver, str):
                driver = _str_to_bytes(driver)

            self._dp = _lib.caca_create_display_with_driver(cv, driver)

        if self._dp == 0:
            raise DisplayError("Failed to create display")

    def get_driver(self):
        """ Return the caca graphical context's current output driver.
        """
        _lib.caca_get_display_driver.argtypes = [_Display]
        _lib.caca_get_display_driver.restype  = ctypes.c_char_p

        return _lib.caca_get_display_driver(self)

    def set_driver(self, driver=None):
        """ Set the output driver.

            driver  -- A string describing the desired output driver or NULL
                       to choose the best driver automatically.
        """
        _lib.caca_set_display_driver.argtypes = [_Display, ctypes.c_char_p]
        _lib.caca_set_display_driver.restype  = ctypes.c_int

        if not driver:
            driver = ctypes.c_char_p(0)
        else:
            if _PYTHON3 and isinstance(driver, str):
                driver = _str_to_bytes(driver)

        return _lib.caca_set_display_driver(self, driver)

    def get_canvas(self):
        """ Get the canvas attached to a caca graphical context.
        """
        _lib.caca_get_canvas.argtypes = [_Display]
        _lib.caca_get_canvas.restype  = ctypes.POINTER(ctypes.c_char_p)

        return Canvas(pointer=_lib.caca_get_canvas(self))

    def refresh(self):
        """ Flush pending changes and redraw the screen.
        """
        _lib.caca_refresh_display.argtypes = [_Display]
        _lib.caca_refresh_display.restype  = ctypes.c_int

        return _lib.caca_refresh_display(self)

    def set_time(self, usec):
        """ Set the refresh delay.

            usec    -- the refresh delay in microseconds
        """
        _lib.caca_set_display_time.argtypes = [_Display, ctypes.c_int]
        _lib.caca_set_display_time.restype  = ctypes.c_int

        return _lib.caca_set_display_time(self, usec)

    def get_time(self):
        """ Get the display's average rendering time.
        """
        _lib.caca_get_display_time.argtypes = [_Display]
        _lib.caca_get_display_time.restype  = ctypes.c_int

        return _lib.caca_get_display_time(self)

    def set_title(self, title):
        """ Set the display title.

            title   -- the desired display title
        """
        _lib.caca_set_display_title.argtypes = [_Display, ctypes.c_char_p]
        _lib.caca_set_display_title.restype  = ctypes.c_int

        if _PYTHON3 and isinstance(title, str):
            title = _str_to_bytes(title)

        return _lib.caca_set_display_title(self, title)

    def set_mouse(self, flag):
        """ Show or hide the mouse pointer. This function works with the
            ncurses, S-Lang and X11 drivers.

            flag -- 0 hides the pointer, 1 shows the system's default pointer
                    (usually an arrow)
        """
        _lib.caca_set_mouse.argtypes = [_Display, ctypes.c_int]
        _lib.caca_set_mouse.restype  = ctypes.c_int

        return _lib.caca_set_mouse(self, flag)

    def set_cursor(self, flag):
        """ Show or hide the cursor, for devices that support such a feature.

            flag -- 0 hides the cursor, 1 shows the system's default cursor
                    (usually a white rectangle).
        """

        _lib.caca_set_cursor.argtypes = [Display, ctypes.c_int]
        _lib.caca_set_cursor.restype  = ctypes.c_int

        return _lib.caca_set_cursor(self, flag)

    def get_event(self, event_mask, event, timeout):
        """ Get the next mouse or keyboard input event.

            event_mask  -- bitmask of requested events
            event       -- a pointer to caca_event structure or NULL
            tiemout     -- a timeout value in microseconds
        """

        _lib.caca_get_event.argtypes = [
                Display, ctypes.c_int, ctypes.POINTER(Event), ctypes.c_int
            ]

        return _lib.caca_get_event(self, event_mask, ctypes.byref(event),
                                         timeout)

    def get_mouse_x(self):
        """ Return the X mouse coordinate.
        """
        _lib.caca_get_mouse_x.argtypes = [Display]
        _lib.caca_get_mouse_x.restype  = ctypes.c_int

        return _lib.caca_get_mouse_x(self)

    def get_mouse_y(self):
        """ Return the Y mouse coordinate.
        """
        _lib.caca_get_mouse_y.argtypes = [Display]
        _lib.caca_get_mouse_y.restype  = ctypes.c_int

        return _lib.caca_get_mouse_y(self)

class DisplayError(Exception):
    pass

class Event(ctypes.Structure):
    """ Object to store libcaca event.
    """
    _fields_ = (
        ('opaque_structure', ctypes.c_char_p * 32),
    )

    def from_param(self):
        """ Required method to pass object as parameter of a C function.
        """
        return ctypes.byref(self)

    def get_type(self):
        """ Return an event's type.
        """
        _lib.caca_get_event_type.argtypes = [Event]
        _lib.caca_get_event_type.restype  = ctypes.c_int

        return _lib.caca_get_event_type(self)

    def get_key_ch(self):
        """ Return a key press or key release event's value.
        """
        _lib.caca_get_event_key_ch.argtypes = [Event]
        _lib.caca_get_event_key_ch.restype  = ctypes.c_int

        return _lib.caca_get_event_key_ch(self)

    def get_key_utf32(self):
        """ Not implemented.
        """
        raise DisplayError("Not implemented")

    def get_key_utf8(self):
        """ Return a key press or key release event's UTF-8 value
            as python string.
        """
        # set buffer for writing utf8 value
        buf = ctypes.c_buffer(7)

        _lib.caca_get_event_key_utf8.argtypes = [Event, ctypes.c_char_p]
        _lib.caca_get_event_key_utf8.restype  = ctypes.c_int

        _lib.caca_get_event_key_utf8(self, buf)

        raw = []
        for item in list(buf.raw):
            if item == '\x00':
                break
            else:
                raw.append(item)

        return "".join(raw)

    def get_mouse_button(self):
        """ Return a mouse press or mouse release event's button.
        """
        _lib.caca_get_event_mouse_button.argtypes = [Event]
        _lib.caca_get_event_mouse_button.restype  = ctypes.c_int

        return _lib.caca_get_event_mouse_button(self)

    def get_mouse_x(self):
        """ Return a mouse motion event's X coordinate.
        """
        _lib.caca_get_event_mouse_x.argtypes = [Event]
        _lib.caca_get_event_mouse_x.restype  = ctypes.c_int

        return _lib.caca_get_event_mouse_x(self)

    def get_mouse_y(self):
        """ Return a mouse motion event's Y coordinate.
        """
        _lib.caca_get_event_mouse_y.argtypes = [Event]
        _lib.caca_get_event_mouse_y.restype  = ctypes.c_int

        return _lib.caca_get_event_mouse_y(self)

    def get_resize_width(self):
        """ Return a resize event's display width value.
        """
        _lib.caca_get_event_resize_width.argtypes = [Event]
        _lib.caca_get_event_resize_width.restype  = ctypes.c_int

        return _lib.caca_get_event_resize_width(self)

    def get_resize_height(self):
        """ Return a resize event's display height value.
        """
        _lib.caca_get_event_resize_height.argtypes = [Event]
        _lib.caca_get_event_resize_height.restype  = ctypes.c_int

        return _lib.caca_get_event_resize_height(self)

