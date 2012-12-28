#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# figfont       libcaca blit test program
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/blit.c"
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
from caca.canvas import Canvas, CanvasError, NullCanvas
from caca.display import Display, DisplayError, Event

THE_PIG="""\
  ,__         __,
  \\)`\\_..._/`(/
   .'  _   _  '.
  /    o\\ /o   \\
  |    .-.-.    |  _
  |   /() ()\\   | (,`)
 / \\  '-----'  / \\ .'
|   '-..___..-'   |
|                 |
|                 |
;                 ;
 \\      / \\      /
  \\-..-/'-'\\-..-/
jgs\\/\\/     \\/\\/"""

def main():
    """ Main function. """
    try:
        cv = Canvas(0, 0)
        dp = Display(cv)
    except (CanvasError, DisplayError) as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(2)

    sprite = Canvas(0, 0)
    sprite.set_color_ansi(caca.COLOR_LIGHTRED, caca.COLOR_BLACK)
    sprite.import_from_memory(THE_PIG, "text")
    sprite.set_handle(sprite.get_width()//2, sprite.get_height()//2)

    cv.set_color_ansi(caca.COLOR_WHITE, caca.COLOR_BLUE)
    cv.put_str(0, 0, "Centered sprite")
    cv.blit(cv.get_width()//2, cv.get_height()//2, sprite, NullCanvas())

    dp.refresh()
    dp.get_event(caca.EVENT_KEY_PRESS, Event(), -1)

    sys.exit(0)

if __name__ == "__main__":
    main()

