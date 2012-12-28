#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# frames        canvas frame switching features
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/frames.c"
# which is:
# Copyright (c) 2006-2010 Sam Hocevar <sam@hocevar.net>
#               All Rights Reserverd
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

import sys

import caca
from caca.canvas import Canvas, CanvasError
from caca.display import Display, DisplayError, Event

def main():
    """ Main function. """

    try:
        cv = Canvas(0, 0)
    except CanvasError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(2)

    for idx in range(1, 200):
        cv.create_frame(idx)

    sys.stderr.write("canvas created, size is %dx%d\n" \
                        % (cv.get_width(), cv.get_height()))

    cv.set_size(150, 80)
    sys.stderr.write("canvas expanded, size is %dx%d\n" \
                        % (cv.get_width(), cv.get_height()))

    for idx in range(0, 16):
        cv.set_frame(idx)
        cv.set_color_ansi(caca.COLOR_WHITE, idx)
        cv.fill_box(0, 0, 40, 15, ':')
        cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLUE)
        cv.put_str((idx * 5) // 2, idx, "カカ")
        cv.set_color_ansi(caca.COLOR_DEFAULT, caca.COLOR_TRANSPARENT)

    cv.set_size(41, 16)
    sys.stderr.write("canvas shrinked, size is %dx%d\n" \
                        % (cv.get_width(), cv.get_height()))

    try:
        dp = Display(cv)
    except DisplayError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(2)

    dp.set_time(50000)
    sys.stderr.write("display attached, size is %dx%d\n" \
                        % (cv.get_width(), cv.get_height()))

    n = 0
    while not dp.get_event(caca.EVENT_KEY_PRESS, Event(), 0):
        cv.set_frame(n % 16)
        dp.refresh()
        n += 1

if __name__ == "__main__":
    main()

