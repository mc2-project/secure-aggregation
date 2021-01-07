from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext as build_ext_orig

from Cython.Build import cythonize

setup(
    name="server_methods",
    ext_modules=cythonize(Extension(
                          name="server_methods",
                          sources=["server_methods.pyx"],
                          extra_compile_args=["-fopenmp"],
                          extra_link_args=["-fopenmp"],
                          language="c++",
                          libraries=["modelaggregator_host"],
                          library_dirs=['.', '../build/host'],
                          include_dirs=['.']
        ),
    )
)
