from distutils.core import setup, Extension
from numpy import get_include
#from numpy.distutils import intelccompiler
import os
#import re

#make note, may have to run setup.py clean first!

#files = os.listdir('c_code/')
#this will filter the results for just the c files
#files = filter(lambda x: not re.search('.+[.]c[~]$',x),files)

extra = '/opt/local/Library/Frameworks/Python.framework/Versions/Current/lib/'
extra2 = '/usr/include/'
ext_mod = []
inc = [get_include(),extra2,extra]


#for i in range(len(files)):
#    exec('mod'+str(i)+'=Extension("'+files[i].rstrip('.c')+'",sources=["c_code/'+files[i]+'"],include_dirs=inc,extra_compile_args=["-fopenmp"],extra_link_args=["-lgomp"])')

#    exec('ext_mod.append(mod'+str(i)+')')

mod = Extension("make_asym",sources=["make_asym.c"],include_dirs=inc,extra_compile_args=['-undefined dynamic_lookup','-bundle','-L'+extra])#,extra_compile_args=["-fopenmp"],extra_link_args=["-lgomp"])
#mod = Extension("make_asym",sources=["make_asym.c"],include_dirs=inc,extra_compile_args=[],extra_link_args=[])#,extra_compile_args=["-fopenmp"],extra_link_args=["-lgomp"])
ext_mod.append(mod)

setup (name='make_asym',version='1.0',description='make asym pyhon c extension',
       ext_modules = ext_mod)
