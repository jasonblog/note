swig -c++ -python TestModel.i
g++ -fPIC -c TestModel.cpp TestModel_wrap.cxx -I /usr/include/python2.7
g++ -shared TestModel.o TestModel_wrap.o -o _TestModel.so

from TestModel import *
t.add(23,44)
