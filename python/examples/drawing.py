#!/usr/bin/env python
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
import sys
import time

import caca
from caca.canvas import Canvas
from caca.display import Display, Event

class Drawing(object):
    def __init__(self, canvas):
        self.cv = canvas

    def do_menu(self):
        self.cv.put_str(0, 1, "Drawing Menu")
        self.cv.put_str(0, 2, "------------")
        self.cv.put_str(0, 4, "1) line")
        self.cv.put_str(0, 5, "2) thin line")
        self.cv.put_str(0, 6, "3) polyline")
        self.cv.put_str(0, 7, "4) thin polyline")
        self.cv.put_str(0, 8, "5) circle")
        self.cv.put_str(0, 9, "6) ellipse")
        self.cv.put_str(0, 10, "7) thin ellipse")
        self.cv.put_str(0, 11, "8) box")
        self.cv.put_str(0, 12, "9) thin box")

    def do_line(self, thin=False):
        if thin:
            self.cv.draw_thin_line(0, 0, self.cv.get_width(), 1)
        else:
            self.cv.draw_line(0, 0, self.cv.get_width(), 1, '#')

    def do_polyline(self, thin=False):
        x = [0, self.cv.get_width() - 1, self.cv.get_width() - 1]
        y = [0, self.cv.get_height(), 0]
        array_x = ctypes.c_int * (len(x) + 1)
        array_y = ctypes.c_int * (len(y) + 1)
        array_xy = [ (x, y) for x, y in zip(array_x(*x), array_y(*y))]
        if thin:
            self.cv.draw_thin_polyline(array_xy)
        else:
            self.cv.draw_polyline(array_xy, '#')

    def do_circle(self):
        x = self.cv.get_width() // 2
        y = self.cv.get_height() // 2
        radius = 5
        self.cv.draw_circle(x, y, radius, '@')

    def do_ellipse(self, thin=False):
        x = self.cv.get_width() // 2
        y = self.cv.get_height() // 2
        a = 7
        b = 3
        if thin:
            self.cv.draw_thin_ellipse(x, y, a, b)
        else:
            self.cv.draw_ellipse(x, y, a, b, '@')

    def do_box(self, thin=False):
        if thin:
            self.cv.draw_thin_box(0, 0, self.cv.get_width(), self.cv.get_height())
        else:
            self.cv.draw_box(0, 0, self.cv.get_width(), self.cv.get_height(), '#')

if __name__ == "__main__":
    cv = Canvas()
    dp = Display(cv)
    ev = Event()
    draw = Drawing(cv)

    while True:
        cv.clear()
        draw.do_menu()
        dp.refresh()
        if dp.get_event(caca.EVENT_KEY_PRESS, ev, 0):
            ch = ev.get_key_ch()
            cv.clear()
            if ch == ord('q'):
                sys.exit()
            elif ch == ord('1'):
                draw.do_line()
            elif ch == ord('2'):
                draw.do_line(thin=True)
            elif ch == ord('3'):
                draw.do_polyline()
            elif ch == ord('4'):
                draw.do_polyline(thin=True)
            elif ch == ord('5'):
                draw.do_circle()
            elif ch == ord('6'):
                draw.do_ellipse()
            elif ch == ord('7'):
                draw.do_ellipse(thin=True)
            elif ch == ord('8'):
                draw.do_box()
            elif ch == ord('9'):
                draw.do_box(thin=True)
            dp.refresh()
            time.sleep(2)

