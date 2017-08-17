swig -python test.i
gcc -O2 -fPIC -c test.c test_wrap.c -I /usr/include/python2.7
gcc -shared test.o test_wrap.o -o _test.so


Python 2.6.6 (r266:84292, Dec 27 2010, 00:02:40)
[GCC 4.4.5] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> from test import *
>>> add(33,22)
55
