from distutils.core import setup
import os
from glob import glob
from pybind11.setup_helpers import Pybind11Extension

print("path: ", os.path.abspath(__file__))

lutnet = Pybind11Extension  (
    'lutnet',
    sources=['lutnet-py.cpp'],
    # libraries=['boost_python37-mt','boost_numpy37-mt'],
    extra_compile_args=['-std=c++11', '-O3', '-DPYTHON_BUILD'],
    include_dirs=['..'],
)

setup(
    name='lutnet',
    version='1.7',
    ext_modules=[lutnet])
