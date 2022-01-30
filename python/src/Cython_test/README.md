https://clay-atlas.com/blog/2019/12/19/python-chinese-tutorial-package-cython/

time python fibo.py 40

cp fibo.py fibo.pyx

python setup.py build_ext --inplace

time python fibo_cython.py 40
