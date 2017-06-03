#include<iostream>
using namespace std;
int min(int a, int b) //这里的min就是函数名，a、b是形参，
{
    //返回值是一个int整型
    if (a < b) {
        return a;
    } else {
        return b;
    }
}
int main()
{
    int a = 10, b = 1;
    cout << min(a, b) << endl; //这里的a、b是实参
    return 0;
}
