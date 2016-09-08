# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from distutils.core import setup, Extension

cpp_args = ['-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.7']

ext_modules = [
    Extension(
        'make_asym',
        ['least_asymmetry/make_asym.cc'],
        include_dirs=['least_asymmetry/include'],
        language='c++',
        extra_compile_args=cpp_args,
    ),
]

setup(
    name='least_asymmetry',
    version='0.1',
    author='Nate Lust',
    author_email='nlust@astro.princeton.edu',
    description='A module for calculating centers though least asymmetry',
    packages = ['least_asymmetry'],
    ext_modules=ext_modules,
)
