#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# img2txt       image to text converter
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#
# This file is a Python port of "src/img2txt.c"
# which is:
# Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
#               2007 Jean-Yves Lamoureux <jylam@lnxscene.org>
#               All Rights Reserverd
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

import getopt
import os
import sys

import caca
from caca.canvas import Canvas, CanvasError
from caca.dither import Dither, DitherError

RMASK = 0x00ff0000
GMASK = 0x0000ff00
BMASK = 0x000000ff
AMASK = 0xff000000
BPP = 32
DEPTH = 4

HELP_MSG = """\
Usage: %s [OPTIONS]... <IMAGE>
Convert IMAGE to any text based available format.
Example: %s -w 80 -f ansi ./caca.png

Options:
  -h, --help                      This help
  -v, --version                   Version of the program
  -W, --width=WIDTH               Width of resulting image
  -H, --height=HEIGHT             Height of resulting image
  -x, --font-width=WIDTH          Width of output font
  -y, --font-height=HEIGHT        Height of output font
  -b, --brightness=BRIGHTNESS     Brightness of resulting image
  -c, --contrast=CONTRAST         Contrast of resulting image
  -g, --gamma=GAMMA               Gamma of resulting image
  -d, --dither=DITHER             Dithering algorithm to use
  -f, --format=FORMAT             Format of the resulting image
  -C, --charset=CHARSET           Charset of the resulting image

DITHER list:
  - none: no dithering
  - ordered2: 2x2 ordered dithering
  - ordered4: 4x4 ordered dithering
  - ordered8: 8x8 orederd dithering
  - random: random dithering
  - fstein: Floyd-Steinberg dithering

FORMAT list:
  - caca: native libcaca format
  - ansi: ANSI
  - utf8: UTF-8 with ANSI escape codes
  - utf8cr: UTF-8 with ANSI escape codes and MS-DOS \\r
  - html: HTML
  - html3: backwards-compatible HTML
  - bbfr: BBCode (French)
  - irc: IRC with mIRC colours
  - ps: PostScript document
  - svg: SVG vector image
  - tga: TGA image
  - troff: troff source

CHARSET list:
  - ascii: use only ascii character
  - shades: use unicode character
  - blocks: use unicode quarter-cell combinations
""" % (os.path.basename(sys.argv[0]), os.path.basename(sys.argv[0]))

VERSION_MSG="""\
img2txt Copyright 2006-2007 Sam Hocevar and Jean-Yves Lamoureux
        Copyright 2010      Alex Foulon
Internet: <sam@hocevar.net> <jylam@lnxscene.org>
          <alxf@lavabit.com> version: %s

img2txt, along with its documentation, may be freely copied and distributed.

The latest version of img2txt is available from the web site,
        http://caca.zoy.org/wiki/libcaca in the libcaca package.
""" % caca.get_version()

def main():
    """ Main script function.
    """

    #init options vars
    width = None
    height = None
    font_width = 6
    font_height = 10
    brightness = None
    contrast = None
    gamma = None
    ditalgo = None
    exformat = "ansi"
    charset = None

    #define long and short options
    shortopts = "hvW:H:x:y:b:c:g:d:f:C:"
    longopts = [
        "help",
        "version",
        "width=",
        "height=",
        "font-width=",
        "font-height=",
        "brightness=",
        "contrast=",
        "gamma=",
        "dither=",
        "format=",
        "charset=",
    ]

    #parse command line
    try:
        opts, args = getopt.getopt(sys.argv[1:], shortopts, longopts)
    except getopt.GetoptError, err:
        sys.stderr.write("%s\n" % str(err))
        sys.exit(127)

    for o, a in opts:
        if o in ('-h', '--help'):
            sys.stdout.write(HELP_MSG)
            sys.exit(0)
        elif o in ('-v', '--version'):
            sys.stdout.write("%s\n" % VERSION_MSG)
            sys.exit(0)
        elif o in ('-W', '--width'):
            width = int(a)
        elif o in ('-H', '--height'):
            height = int(a)
        elif o in ('-x', '--font-width'):
            font_width = int(a)
        elif o in ('-y', '--font-height'):
            font_height = int(a)
        elif o in ('-b', '--brightness'):
            brightness = float(a)
        elif o in ('-c', '--contrast'):
            contrast = float(a)
        elif o in ('-g', '--gamma'):
            gamma = float(a)
        elif o in ('-d', '--dither'):
            ditalgo = a
        elif o in ('-f', '--format'):
            exformat = a
        elif o in ('-C', '--charset'):
            charset = a

    if not args:
        sys.stderr.write("%s: Missing argument...\n" % os.path.basename(sys.argv[0]))
        sys.exit(127)
    else:
        try:
            img = Image.open(args[0])
            img.load()
        except IOError, err:
            sys.stderr.write("%s\n" % err)
            sys.exit(127)

    if not width and not height:
        width = 60
        height = width * img.size[1] * font_width / img.size[0] / font_height
    elif width and not height:
        height = width * img.size[1] * font_width / img.size[0] / font_height
    elif not width and height:
        width = height * img.size[0] * font_height / img.size[1] / font_width

    #init canvas
    try:
        cv = Canvas(width, height)
    except CanvasError, err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    cv.set_color_ansi(caca.COLOR_DEFAULT, caca.COLOR_TRANSPARENT)

    #init dither
    try:
        #convert rgb to rgba
        if img.mode == 'RGB':
            img = img.convert('RGBA')
        #reorder rgba
        if img.mode == 'RGBA':
            r, g, b, a = img.split()
            img = Image.merge("RGBA", (b, g, r, a))

        dit = Dither(BPP, img.size[0], img.size[1], DEPTH * img.size[0],
                        RMASK, GMASK, BMASK, AMASK)
    except DitherError, err:
        sys.stderr.write("%s\n" % err)
        sys.exit(127)

    #set dither algorithm
    if ditalgo:
        dit.set_algorithm(ditalgo)

    #set brightness
    if brightness:
        dit.set_brightness(brightness)

    #set gamma
    if gamma:
        dit.set_gamma(gamma)

    #set contrast
    if contrast:
        dit.set_contrast(contrast)

    #set charset
    if charset:
        dit.set_charset(charset)

    #create dither
    dit.bitmap(cv, 0, 0, width, height, img.tostring())

    #print export to screen
    sys.stdout.write("%s" % cv.export_to_memory(exformat))

if __name__ == "__main__":
    #Import PIL package
    try:
        from PIL import Image
    except ImportError, err:
        sys.stderr.write("You need to install PIL module !\n")
        sys.exit(2)

    main()

