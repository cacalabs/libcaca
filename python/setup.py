#!/usr/bin/env python
#
# Minimal setup.py script
#

import sys
from setuptools import setup

try:
    import caca
except ImportError as err:
    sys.stderr.write("FATAL: %s\n" % str(err))
    sys.exit(127)

version_string=caca.get_version()

setup(
    name='caca',
    author='Alex Foulon',
    author_email='alxf@lavabit.com',
    version=version_string,
    packages=['caca'],
    package_dir={
        'caca': 'caca',
    },
    test_suite='test.alltests',
)

