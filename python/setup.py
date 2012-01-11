#!/usr/bin/env python
#
# Minimal setup.py script
#

from setuptools import setup

import caca

setup(
    name='caca',
    author='Alex Foulon',
    author_email='alxf@lavabit.com',
    version=caca.get_version(),
    packages=['caca'],
    package_dir={
        'caca': 'caca',
    },
    test_suite='test.alltests',
)

