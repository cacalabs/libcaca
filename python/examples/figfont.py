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
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

import codecs
import os
import sys

import caca
from caca.canvas import Canvas, CanvasError

def main():
    """ Main function. """


    if len(sys.argv) < 3:
        sys.stderr.write("Usage: %s <figfont file> <word>\n" \
                            % os.path.basename(sys.argv[0]))
        sys.exit(2)

    try:
        cv = Canvas(0, 0)
    except CanvasError as err:
        sys.stderr.write("%s\n" % err)
        sys.exit(2)

    if cv.set_figfont(sys.argv[1]):
        sys.stderr.write("Could not open font...\n")
        sys.exit(2)

    if sys.version_info[0:2] >= (3,0):
        word = sys.argv[2]
    else:
        word = codecs.decode(sys.argv[2], "utf8")
    for c in word:
        cv.put_figchar(c)

    sys.stderr.write(cv.export_to_memory("utf8"))

if __name__ == "__main__":
    main()

