from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext as build_ext_orig

from Cython.Build import cythonize

setup(
    ext_modules=cythonize(Extension(
                          name="server_methods",
                          sources=["server_methods.pyx", "host.cpp"],
                          language="c++",
                          libraries=["mbedtls", "mbedcrypto", "oehost", 
                          "sgx_enclave_common", "sgx_dcap_ql", "sgx_urts"],
                          library_dirs=['/opt/openenclave', '/opt/openenclave/lib/openenclave/host'],
                          include_dirs=['/usr/include/mbedtls', '/usr/include/mbedcrypto', '/home/mc2/code/encryption/server', 
                          '/opt/openenclave/include', '.', '/opt/openenclave/lib/openenclave/host']
        ),
    )
)

