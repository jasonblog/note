#include<iostream>
#include<stdlib.h>
using namespace std;
void func()
{
    int* x = (int*)malloc(10 * sizeof(int)) ;
    x[10] = 0;
}
int main()
{
    func();
    cout << "done" << endl;
    return 0;
}
