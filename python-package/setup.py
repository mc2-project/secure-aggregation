from setuptools import setup, find_packages
import sys

sys.path.insert(0, '.')

setup(name='secagg',
      version="0.0.1",
      description="Secure Aggregation Python Package",
      install_requires=[
          'numpy',
      ],
      zip_safe=False,
      packages=find_packages(),
      include_package_data=True,
      python_requires='>=3.4')
