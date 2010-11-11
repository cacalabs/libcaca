#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# figfont       libcaca FIGfont test program
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "examples/figfont.c"
# which is:
# Copyright (c) 2007-2010 Sam Hocevar <sam@hocevar.net>
#               All Rights Reserverd
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What The Fuck You Want
# To Public License, Version 2, as published by Sam Hocevar. See
# http://sam.zoy.org/wtfpl/COPYING for more details.
#

import os
import sys

import caca
from caca.canvas import Canvas, CanvasError

def main():
    """ Main function. """

    color = 0

    if len(sys.argv) < 3:
        sys.stderr.write("Usage: %s <figfont file> <word>\n" \
                            % os.path.basename(sys.argv[0]))
        sys.exit(2)

    try:
        cv = Canvas(0, 0)
    except CanvasError, err:
        sys.stderr.write("%s\n" % err)
        sys.exit(2)

    if cv.set_figfont(sys.argv[1]):
        sys.stderr.write("Could not open font...\n")
        sys.exit(2)

    for c in sys.argv[2]:
        color += 4
        cv.set_color_ansi(1+(color % 15), caca.COLOR_TRANSPARENT)
        cv.put_figchar(c)

    sys.stderr.write(cv.export_to_memory("utf8"))

if __name__ == "__main__":
    main()

