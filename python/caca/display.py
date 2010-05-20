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
from caca.canvas import _Canvas

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
            self._dp = _lib.caca_create_display_with_driver(cv, driver)

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

        return _lib.caca_set_display_driver(self, driver)

    def get_canvas(self):
        """ Get the canvas attached to a caca graphical context.
        """
        _lib.caca_get_canvas.argtypes = [_Display]

        return _lib.caca_get_canvas(self)

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

        return _lib.caca_set_display_title(self, title)

    def set_mouse(self, flag):
        """ Show or hide the mouse pointer. This function works with the ncurses,
        S-Lang and X11 drivers.

            flag -- 0 hides the pointer, 1 shows the system's default pointer (usually an arrow).
        """
        _lib.caca_set_mouse.argtypes = [_Display, ctypes.c_int]
        _lib.caca_set_mouse.restype  = ctypes.c_int

        return _lib.caca_set_mouse(self, flag)

    def set_cursor(self, flag):
        """ Show or hide the cursor, for devices that support such a feature.

            flag -- 0 hides the cursor, 1 shows the system's default cursor (usually a white rectangle).
        """

        _lib.caca_set_cursor.argtypes = [Display, ctypes.c_int]
        _lib.caca_set_cursor.restype  = ctypes.c_int

        return _lib.caca_set_cursor(self, flag)

    def get_event(self, event_mask, event, timeout):
        """ Poll the event queue for mouse or keyboard events matching the event mask
        and return the first matching event. Non-matching events are discarded.
        If event_mask is zero, the function returns immediately.
        The timeout value tells how long this function needs to wait for an event.
        A value of zero returns immediately and the function returns zero if no more events
        are pending in the queue. A negative value causes the function to wait indefinitely
        until a matching event is received.
        If not null, ev will be filled with information about the event received. If null,
        the function will return but no information about the event will be sent.

            event_mask  -- bitmask of requested events
            event       -- a pointer to caca_event structure or NULL
            tiemout     -- a timeout value in microseconds
        """

        _lib.caca_get_event.argtypes = [Display, ctypes.c_int, ctypes.POINTER(Event), ctypes.c_int]

        return _lib.caca_get_event(self, event_mask, ctypes.byref(event), timeout)

    def get_mouse_x(self):
        """ Return the X coordinate of the mouse position last time it was detected.
        This function is not reliable if the ncurses or S-Lang drivers are being used,
        because mouse position is only detected when the mouse is clicked.
        Other drivers such as X11 work well.
        """

        _lib.caca_get_mouse_x.argtypes = [Display]
        _lib.caca_get_mouse_x.restype  = ctypes.c_int

        return _lib.caca_get_mouse_x(self)

    def get_mouse_y(self):
        """ Return the Y coordinate of the mouse position last time it was detected.
        This function is not reliable if the ncurses or S-Lang drivers are being used,
        because mouse position is only detected when the mouse is clicked.
        Other drivers such as X11 work well.
        """

        _lib.caca_get_mouse_y.argtypes = [Display]
        _lib.caca_get_mouse_y.restype  = ctypes.c_int

        return _lib.caca_get_mouse_y(self)

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
        """ Return the type of an event. This function may always be called
            on an event after caca_get_event() was called, and its return value
            indicates which other functions may be called.
        """
        _lib.caca_get_event_type.argtypes = [Event]
        _lib.caca_get_event_type.restype = ctypes.c_int

        return _lib.caca_get_event_type(self)

    def get_key_ch(self):
        """ Return either the ASCII value for an event's key, or if the key is not
            an ASCII character, an appropriate KEY_* value
        """
        _lib.caca_get_event_key_ch.argtypes = [Event]
        _lib.caca_get_event_key_ch.restype = ctypes.c_int

        return _lib.caca_get_event_key_ch(self)

    def get_key_utf32(self):
        """ Return the UTF-32/UCS-4 value for an event's key if it resolves
            to a printable character.
        """
        _lib.caca_get_event_key_utf32.argtypes = [Event]
        _lib.caca_get_event_key_utf32.restype = ctypes.c_uint32

        return _lib.caca_get_event_key_utf32(self)

    def get_key_utf8(self):
        """ Write the UTF-8 value for an event's key if it resolves to a
            printable character. Up to 6 UTF-8 bytes and a null termination
            are written.
        """
        # set buffer for writing utf8 value
        buf = ctypes.c_buffer(2)

        _lib.caca_get_event_key_utf8.argtypes = [Event, ctypes.c_char_p]
        _lib.caca_get_event_key_utf8.restype = ctypes.c_int

        _lib.caca_get_event_key_utf8(self, buf)

        return buf

