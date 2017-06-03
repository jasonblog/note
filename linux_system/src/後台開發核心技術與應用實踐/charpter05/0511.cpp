#include<stdlib.h>
#include<iostream>
using namespace std;
int main()
{
    int len = 4;
    int* pt = (int*)malloc(len * sizeof(int));
    int* p = pt;

    for (int i = 0; i < len; i++) {
        p++;
    }

    *p = 5;
    cout << "the value of p is " << *p << endl;
    return 0;
}
