#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# colors        display all possible libcaca colour pairs
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/colors.c"
# which is:
# Copyright (c) 2003-2010 Sam Hocevar <sam@hocevar.net>
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
        cv = Canvas(80, 24)
        dp = Display(cv)
    except (CanvasError, DisplayError) as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    cv.set_color_ansi(caca.COLOR_LIGHTGRAY, caca.COLOR_BLACK)
    cv.clear()

    for i in range(0, 16):
        if i >= 8:
            y = i + 3
        else:
            y = i + 2

        cv.set_color_ansi(caca.COLOR_LIGHTGRAY, caca.COLOR_BLACK)
        cv.printf(3, y, "ANSI %i", i)

        for j in range(0, 16):
            if j >= 8:
                x = 13 + (j * 4)
            else:
                x = 12 + (j * 4)
            if i >= 8:
                y = i + 3
            else:
                y = i + 2

            cv.set_color_ansi(i, j)
            cv.put_str(x, y, "Aaホ")

    cv.set_color_ansi(caca.COLOR_LIGHTGRAY, caca.COLOR_BLACK)
    cv.put_str(3, 20, "This is bold    This is blink    This is italics    This is underline")
    cv.set_attr(caca.STYLE_BOLD)
    cv.put_str(3 + 8, 20, "bold")
    cv.set_attr(caca.STYLE_BLINK)
    cv.put_str(3 + 24, 20, "blink")
    cv.set_attr(caca.STYLE_ITALICS)
    cv.put_str(3 + 41, 20, "italics")
    cv.set_attr(caca.STYLE_UNDERLINE)
    cv.put_str(3 + 60, 20, "underline")

    dp.refresh()
    dp.get_event(caca.EVENT_KEY_PRESS, Event(), -1)

if __name__ == "__main__":
    main()

