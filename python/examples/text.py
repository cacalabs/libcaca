#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# text          canvas text import/export
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/text.c"
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

STRING="""\
              |_| 
   _,----._   | | 
  (/ @  @ \\)   __ 
   |  OO  |   |_ 
   \\ `--' /   |__ 
    `----' 
              |_| 
 Hello world!  | 

"""

def main():
    """ Main function. """

    try:
        pig = Canvas(0, 0)
        pig.import_from_memory(STRING, "text")
        cv = Canvas(pig.get_width() * 2, pig.get_height() * 2)
    except CanvasError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(2)

    cv.blit(0, 0, pig, NullCanvas())
    pig.flip()
    cv.blit(pig.get_width(), 0, pig, NullCanvas())
    pig.flip()
    pig.flop()
    cv.blit(0, pig.get_height(), pig, NullCanvas())
    pig.flop()
    pig.rotate_180()
    cv.blit(pig.get_width(), pig.get_height(), pig, NullCanvas())

    for j in range(0, cv.get_height()):
        for i in range(0, cv.get_width(), 2):
            cv.set_color_ansi(caca.COLOR_LIGHTBLUE + (i + j) % 6,
                              caca.COLOR_DEFAULT)

            a = cv.get_attr(-1, -1)
            cv.put_attr(i, j, a)
            cv.put_attr(i+1, j, a)

    print("%s" % cv.export_to_memory('utf8'))
    cv.rotate_left()
    print("%s" % cv.export_to_memory('utf8'))

if __name__ == "__main__":
    main()

