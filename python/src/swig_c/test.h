//test.h
typedef int (*FUNC) (int, int);
int fn(int a, int b, int (*op)(int,int));
int add(int a, int b); 
int mul(int a, int b); 
int sub(int a, int b);
