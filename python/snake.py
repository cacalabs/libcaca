#!/usr/bin/env python
#
# snake.py
# Playing with ctypes and libcaca
# http://mornie.org/blog/2007/03/25/Playng-with-ctypes-and-libcaca/
#
# Copyright (C) 2007  Daniele Tricoli aka Eriol <eriol@mornie.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# -- Changelog
# * 23/03/07
#   Initial release
# * 20/10/07
#   Applied patch by Sam Hocevar: check for caca_get_event's return value
#                                 and added caca_event's missing first member
# * 25/10/07
#   Updated for newer libcaca API (Sam Hocevar)

import ctypes as C
import random
import sys
import time

CANVAS_WIDTH = 80
CANVAS_HEIGHT = 40

CENTER_X = CANVAS_WIDTH / 2
CENTER_Y = CANVAS_HEIGHT / 2

UP = 273
DOWN = 274
LEFT = 275
RIGHT = 276

class ev(C.Structure):
    _fields_ = [('opaque_structure', C.c_char_p * 32)]

class Snake(object):

    def __init__(self, center_point, length):

        self.head = center_point
        self.body = []

        for y in xrange(self.head[1] + 1, self.head[1] + length + 1):
            self.body.append((self.head[0], y))

    def move(self, direction):

        phead = tuple(self.head)

        if direction == 'UP':
            self.head[1] -=1
        elif direction == 'DOWN':
            self.head[1] +=1
        elif direction == 'LEFT':
            self.head[0] -=1
        elif direction == 'RIGHT':
            self.head[0] +=1

        self.body = [phead] + self.body[:-1]

    def grow(self):
        self.body += [tuple(self.head)] * 2

    def draw(self):
        global cv
        lcaca.caca_set_color_ansi(cv, 0x05, 0x00)

        for p in self.body:
            lcaca.caca_put_char(cv, p[0], p[1], ord('o'))
        lcaca.caca_set_color_ansi(cv, 0x02, 0x00)
        lcaca.caca_put_char(cv, self.head[0], self.head[1], ord('@'))
        lcaca.caca_refresh_display(dp)

class Target(object):

    def __init__(self):
        self.total = 0

    def random(self, width, height):
        self.x = int(random.uniform(1, width))
        self.y = int(random.uniform(1, height))
        self.value = random.choice(range(1,10))

    def sum(self):
        self.total += self.value

    def draw(self):
        global cv
        lcaca.caca_set_color_ansi(cv, 0x03, 0x00)
        lcaca.caca_put_char(cv, self.x, self.y, ord(str(self.value)))
        lcaca.caca_refresh_display(dp)

def draw_border():
    lcaca.caca_set_color_ansi(cv, 0x04, 0x00)
    lcaca.caca_draw_box(cv,
                         0,
                         0,
                         CANVAS_WIDTH - 1,
                         CANVAS_HEIGHT - 1,
                         ord('#'))

event = ev()
lcaca = C.cdll.LoadLibrary('libcaca.so.0')
cv = lcaca.caca_create_canvas(CANVAS_WIDTH, CANVAS_HEIGHT)
dp = lcaca.caca_create_display(cv)
lcaca.caca_set_display_title(dp, "snake.py - playing with ctypes and libcaca")

s = Snake([CENTER_X, CENTER_Y], 5)
t = Target()
t.random(CANVAS_WIDTH - 2, CANVAS_HEIGHT - 2)

draw_border()
s.draw()
t.draw()

lcaca.caca_get_event(dp, 0x0001, C.byref(event), -1)

while True:
    while lcaca.caca_get_event(dp, 0x0001, C.byref(event), 0):
        ch = lcaca.caca_get_event_key_ch(C.byref(event))
        if ch == 113: # 'q' pressed
            sys.exit()
        elif ch == UP:
            d = 'UP'
        elif ch == DOWN:
            d = 'DOWN'
        elif ch == LEFT:
            d = 'LEFT'
        elif ch == RIGHT:
            d = 'RIGHT'

    try:
        s.move(d)
    except NameError:
        pass

    if (tuple(s.head) in s.body[1:] or
        not 0 < s.head[0] < CANVAS_WIDTH - 1 or
        not 0 < s.head[1] < CANVAS_HEIGHT - 1):
        print 'Game Over!'
        print 'Total score:', t.total
        sys.exit()
    elif tuple(s.head) == (t.x, t.y):
        t.sum()
        t.random(CANVAS_WIDTH - 2, CANVAS_HEIGHT - 2)
        s.grow()

    lcaca.caca_clear_canvas(cv)
    draw_border()
    s.draw()
    t.draw()
    time.sleep(0.1)
