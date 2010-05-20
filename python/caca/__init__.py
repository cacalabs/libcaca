# -*- coding: utf-8 -*-
#
# libcaca       Colour ASCII-Art library
#               Python language bindings
# Copyright (c) 2010 Alex Foulon <alxf@lavabit.com>
#               All Rights Reserved
#
# This library is free software. It comes without any warranty, to
# the extent permitted by applicable law. You can redistribute it
# and/or modify it under the terms of the Do What The Fuck You Want
# To Public License, Version 2, as published by Sam Hocevar. See
# http://sam.zoy.org/wtfpl/COPYING for more details.
#

""" Libcaca Python bindings """

import ctypes
from ctypes.util import find_library

if find_library('caca') is not None:
    _lib = ctypes.cdll.LoadLibrary(find_library('caca'))
else:
    raise ImportError, \
        "Can't find shared library, you need to install libcaca in your path !"

from common import *

