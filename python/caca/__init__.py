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

#standard modules
import locale
import sys
import ctypes
from ctypes.util import find_library

if find_library('caca') is not None:
    _lib = ctypes.cdll.LoadLibrary(find_library('caca'))
else:
    raise ImportError(
        "Can't find shared library, you need to install libcaca in your path !")

#functions to handle string/bytes in python3+
if sys.version_info[0:2] >= (3, 0):
    _PYTHON3 = True
else:
    _PYTHON3 = False

def _str_to_bytes(the_string):
    """ Translate string to bytes type for python 3.
    """
    return bytes(the_string, locale.getlocale()[1])

def _bytes_to_str(the_bytes):
    """ Translate bytes to string type for python 3.
    """
    return the_bytes.decode(locale.getlocale()[1])

from .common import *

