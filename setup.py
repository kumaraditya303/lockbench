from setuptools import setup, Extension

setup(name='lockbench', ext_modules=[Extension('lockbench', sources=['lockbench.cpp', 'rwlock.c'])])