#include<iostream>
using namespace std;
int main()
{
    int a = 2;
    int& r = a;
    a = a + 4;
    cout << a << " " << r << endl; //因为a和r的值会同时变化，所以a和r的值都是6。
    r = 10;
    cout << a << " " << r << endl; //r变了，a也会变，所以a和r的值都是10。
    return 0;
}
