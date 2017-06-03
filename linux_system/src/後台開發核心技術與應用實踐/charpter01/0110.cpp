#include<iostream>
using namespace std;
void Mmin1(int a, int b)
{
    int temp;

    if (a > b) {
        temp = a;
        a = b;
        b = temp;
    }
}

void Mmin2(int& a, int& b) //引用作为函数的参数
{
    int temp;

    if (a > b) {
        temp = a;
        a = b;
        b = temp;
    }
}
int main()
{
    int a = 30, b = 20;
    Mmin1(a, b); //
    cout << a << " " << b << endl; //a、b的值保持不变。
    Mmin2(a, b);
    cout << a << " " << b << endl; //a的值是20，b的值是30. a、b的值被修改了
    return 0;
}
