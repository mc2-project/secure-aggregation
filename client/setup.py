from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext as build_ext_orig

from Cython.Build import cythonize

setup(
    ext_modules=cythonize(Extension(
                          name="client_methods",
                          sources=["client_methods.pyx", "../common/encryption/encrypt.cpp", "../common/encryption/serialization.cpp"],
                          language="c++",
                          libraries=["mbedtls", "mbedcrypto"],
                          include_dirs=['/usr/include/mbedtls', '/usr/include/mbedcrypto']
        ),
    )
)

