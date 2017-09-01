swig -c++ -python TestModel.i
g++ -fPIC -c TestModel.cpp TestModel_wrap.cxx -I /usr/include/python2.7
g++ -shared TestModel.o TestModel_wrap.o -o _TestModel.so

from TestModel import *
Quaternion_Quat2Euler(-0.16151874, -0.042242825, -0.97542757, 0.14371127)
