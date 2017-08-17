all: _TestModel.so

_TestModel.so: TestModel_wrap.o
	g++ -shared TestModel.o TestModel_wrap.o -o _TestModel.so

TestModel_wrap.o: TestModel_wrap.cxx
	g++ -fPIC -c TestModel.cpp TestModel_wrap.cxx -I /usr/include/python2.7

TestModel_wrap.cxx: TestModel.i 
	swig -c++ -python TestModel.i

clean:
	rm -f TestModel.py TestModel_wrap.cxx *.o *.so
