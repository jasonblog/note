%module TestModel
%{
#include "TestModel.h"
%}

class TestModel
{
public:
    TestModel();
    int add(int x,int y);
};
