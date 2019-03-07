from distutils.core import setup, Extension
import numpy

harpy_module = Extension('harpy',
    sources = ['src/harpy.c'],
    extra_compile_args = ['-fopenmp'],
    extra_link_args = ['-lgomp'],
    include_dirs = [numpy.get_include()]
)

setup(
    name = 'harpy',
    version = '1.0',
    description = 'Parallel Image processing package',
    ext_modules = [harpy_module]
)