//test.i
/*
%module test
%{
    int binary_op(int a, int b, int (*op)(int,int));
    int add(int,int);
    int sub(int,int);
    int mul(int,int);
%}

extern int binary_op(int, int , int (*op)(int,int));


%constant int add(int,int);
%constant int sub(int,int);
%constant int mul(int,int);
*/

%module test
%{
    #include "test.h"
%}

typedef int (*FUNC) (int, int);
extern int fn(int, int, FUNC);

%callback("%s_cb");
    extern int add(int a, int b);
    extern int mul(int a, int b);
    extern int sub(int a, int b);
%nocallback;
