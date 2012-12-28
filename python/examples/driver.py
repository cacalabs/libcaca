#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# driver        libcaca test drivers program
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/driver.c"
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

    lst = caca.get_display_driver_list()
    cur = 0

    try:
        cv = Canvas(0, 0)
        dp = Display(cv)
    except (CanvasError, DisplayError) as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLACK)

    while True:
        cv.put_str(1, 0, "Available drivers:")
        cur_driver = dp.get_driver()
        n = 0
        for driver, desc in lst:
            if driver == cur_driver:
                cv.put_str(2, n + 2, "%s %s (%s)" % ('*', driver, desc))
            else:
                cv.put_str(2, n + 2, "%s %s (%s)" % (' ', driver, desc))
            n += 1

        cv.put_str(2, n + 3, "Switching driver in 5 seconds")
        dp.refresh()

        if dp.get_event(caca.EVENT_KEY_PRESS, Event(), 5000000):
            break

        cur += 1
        if cur < len(lst) and lst[cur][0] == "raw":
            cur += 1
        if cur >= len(lst):
            cur = 0

        dp.set_driver(lst[cur][0])

if __name__ == "__main__":
    main()

