#include<iostream>
using namespace std;
int main()
{
    int p1 = 1; //p1是一个普通的整型变量
    int* p2;    //p2是一个指针，指向一个整型变量
    p2 = &p1; //把p1的地址赋值给p2，p2也就指向了p1
    cout << p1 << " " << *p2 << endl; //*p2就是取p2所指向的地址的内容
    p1 = 2; //那么*p2的值也是2
    cout << p1 << " " << *p2 << endl;
    *p2 = 3; //那么p1的值也是3
    cout << p1 << " " << *p2 << endl;
    return 0;
}
