#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# font          libcaca font test program
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/font.c"
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

import ctypes
import sys

import caca
from caca.canvas import Canvas, CanvasError
from caca.display import Display, DisplayError, Event
from caca.dither import Dither, DitherError
from caca.font import Font, FontError

def main():
    """ Main function. """

    try:
        cv = Canvas(8, 2)
    except CanvasError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLACK)
    cv.put_str(0, 0, "ABcde")
    cv.set_color_ansi(caca.COLOR_LIGHTRED, caca.COLOR_BLACK)
    cv.put_str(5, 0, "\\o/")
    cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLUE)
    cv.put_str(0, 1, "&$âøÿØ?!")

    fonts = caca.get_font_list()
    if not fonts:
        sys.stderr.write("libcaca was compiled without any fonts\n")
        sys.exit(127)

    try:
        f = Font(fonts[0])
    except FontError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    w = cv.get_width() * f.get_width()
    h = cv.get_height() * f.get_height()
    buf = ctypes.c_buffer(4 * w * h)

    cv.render(f, buf, w, h, 4 * w)

    cv.set_size(80, 32)
    try:
        dp = Display(cv)
    except DisplayError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    try:
        if sys.byteorder == 'big':
            dit = Dither(32, w, h, 4 * w, 0xff0000, 0xff00, 0xff, 0xff000000)
        else:
            dit = Dither(32, w, h, 4 * w, 0xff00, 0xff0000, 0xff000000, 0xff)

        dit.bitmap(cv, 0, 0, cv.get_width(), cv.get_height(), buf)
    except DitherError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)
    else:
        dp.refresh()

    dp.get_event(caca.EVENT_KEY_PRESS, Event(), -1)

if __name__ == "__main__":
    main()

