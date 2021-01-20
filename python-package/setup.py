from setuptools import setup, find_packages
import sys

sys.path.insert(0, '.')

setup(name='secagg',
      version="0.0.1",
      description="Secure Aggregation Python Package",
      install_requires=[
          'numpy',
      ],
      maintainer='Chester Leung',
      maintainer_email='chestercleung@gmail.com',
      zip_safe=False,
      packages=find_packages("secagg"),
      include_package_data=True,
      #  data_files=[("secagg", '__init__.py')],
      python_requires='>=3.4')
