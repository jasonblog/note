#include<iostream>
using namespace std;
int Mmin(int x, int y)
{
    if (x < y) {
        return x;
    }

    return y;
}
int Mmax(int x, int y)
{
    if (x > y) {
        return x;
    }

    return y;
}
int main()
{
    int (*f)(int x, int y);
    int a = 10, b = 20;
    f = Mmin; //把Mmin函数的入口地址赋给f
    cout << (*f)(a, b) << endl;
    f = Mmax; //把Mmax函数的入口地址赋给f
    cout << (*f)(a, b) << endl;
    return 0;
}
