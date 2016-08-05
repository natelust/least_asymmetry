# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from distutils.core import setup, Extension

cpp_args = ['-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.7']

ext_modules = [
    Extension(
        'make_asym',
        ['make_asym.cc'],
        include_dirs=['include'],
        language='c++',
        extra_compile_args=cpp_args,
    ),
]

setup(
    name='make_asym',
    version='0.1',
    author='Nate Lust',
    author_email='nlust@astro.princeton.edu',
    description='A module for calculating centers though least asymmetry',
    ext_modules=ext_modules,
)
