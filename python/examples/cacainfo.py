#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# libcaca       Colour ASCII-Art library
#               Python language bindings
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#               All Rights Reserved
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What the Fuck You Want
# to Public License, Version 2, as published by Sam Hocevar. See
# http://www.wtfpl.net/ for more details.
#

""" Libcaca Python bindings """

import caca

if __name__ == '__main__':
    print("libcaca version %s" % caca.get_version())
    print("")
    print("Available drivers:")
    for drv, desc in caca.get_display_driver_list():
        print("  - %s: %s" % (drv, desc))
    print("")
    print("Available fonts:")
    for font in caca.get_font_list():
        print("  - %s" % font)
    print("")
    print("Export formats:")
    for fmt, desc in caca.get_export_list():
        print("  - %s: %s" % (fmt, desc))
    print("")
    print("Import formats:")
    for fmt, desc in caca.get_import_list():
        print("  - %s: %s" % (fmt, desc))
