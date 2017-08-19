all: _test.so

_test.so: test_wrap.o
	gcc -shared test.o test_wrap.o -o _test.so

test_wrap.o: test_wrap.c
	gcc -fPIC -c test.c test_wrap.c -I /usr/include/python2.7

test_wrap.c: test.i 
	swig -python test.i

clean:
	rm -f test.py test_wrap.c *.o *.so *.pyc
