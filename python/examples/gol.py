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

import random
import sys
import time

import caca

from caca.canvas import Canvas
from caca.display import Display, Event

class CellArray(object):
    def __init__(self, width, height):
        self.array = []
        self.width = width
        self.height = height

        for i in range(0, self.height):
            self.array.append([])
            for j in range(0, self.width):
                self.array[i].append([])

    def get(self, x, y):
        return self.array[x][y]

    def set(self, x, y, value):
        self.array[x][y] = value

    def neighbors(self, x, y):
        n = 0
        h = self.height
        w = self.width

        if self.get((x-1)%h, (y-1)%w):
            n += 1
        if self.get((x-1)%h, y):
            n += 1
        if self.get((x-1)%h, (y+1)%w):
            n += 1
        if self.get(x, (y-1)%w):
            n += 1
        if self.get(x, (y+1)%w):
            n += 1
        if self.get((x+1)%h, (y-1)%w):
            n += 1
        if self.get((x+1)%h, y):
            n += 1
        if self.get((x+1)%h, (y+1)%w):
            n += 1

        return n

    def population(self):
        n = 0

        for i in range(0, self.height):
            for j in range(0, self.width):
                if self.get(i, j):
                    n += 1

        return n

class CellApp(object):
    def __init__(self, width, height):
        self.cycle  = 0
        self.auto   = False
        self.width  = width
        self.height = height

        self.ca   = CellArray(self.width, self.height)
        self.cbuf = CellArray(self.width, self.height)

    def nextCycle(self):
        self.cycle += 1
        for x in range(0, self.ca.height):
            for y in range(0, self.ca.width):
                if self.ca.get(x, y):
                    if self.ca.neighbors(x, y) >= 2 and self.ca.neighbors(x, y) <= 3:
                        self.cbuf.set(x, y, 1)
                    else:
                        self.cbuf.set(x, y, 0)
                elif not self.ca.get(x, y):
                    if self.ca.neighbors(x, y) == 3:
                        self.cbuf.set(x, y, 1)
                    else:
                        self.cbuf.set(x, y, 0)
                else:
                    self.cbuf.set(x, y, 0)
        for x in range(0, self.ca.height):
            for y in range(0, self.ca.width):
                self.ca.set(x, y, self.cbuf.get(x, y))

    def resetCycle(self):
        self.cycle = 0

    def randomCells(self):
        for x in range(0, self.ca.height):
            for y in range(0, self.ca.width):
                self.ca.set(x, y, random.randint(0, 1))

    def renderCells(self, cv):
        cv.clear()
        cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLUE)
        cv.put_str(0, 0, " "*cv.get_width())
        cv.put_str(0, 0, "s: start, p: pause, n: next, r: random cells, z: reset all")
        cv.put_str(0, cv.get_height()-1, " "*cv.get_width())
        cv.put_str(0, cv.get_height()-1, "generation: %d, population: %d" % (self.cycle, self.ca.population()))
        cv.set_color_ansi(caca.COLOR_DEFAULT, caca.COLOR_BLACK)
        posx = (cv.get_height() - self.height) // 2
        posy = (cv.get_width() - self.width) // 2
        for x in range(0, self.ca.height):
            for y in range(0, self.ca.width):
                if self.ca.get(x, y):
                    cv.put_str(posy+y, posx+x, "@")

    def zeroCells(self):
        for x in range(0, self.ca.height):
            for y in range(0, self.ca.width):
                self.ca.set(x, y, 0)

if __name__ == "__main__":
    cv = Canvas()
    dp = Display(cv)
    ev = Event()

    app = CellApp(80, 20)
    app.zeroCells()

    while True:
        if dp.get_event(caca.EVENT_KEY_PRESS, ev, 2):
            ch = ev.get_key_ch()
            if ch == ord('q'):
                break
            elif ch == ord('s'):
                app.auto = True
            elif ch == ord('n'):
                app.nextCycle()
            elif ch == ord('r'):
                app.randomCells()
            elif ch == ord('p'):
                if app.auto:
                    app.auto = False
                else:
                    app.auto = True
            elif ch == ord('z'):
                app.resetCycle()
                app.zeroCells()
                app.auto = False

        if app.auto:
            app.nextCycle()

        app.renderCells(cv)
        dp.refresh()
        time.sleep(0.2)

