#include<iostream>
using namespace std;
class Base
{
public:
    static int var;
};
int Base::var = 10;
class Derived: public Base
{
};
int main()
{
    Base base1;
    base1.var++;//通过对象名引用
    cout << base1.var << endl; //输出11
    Base base2;
    base2.var++;
    cout << base2.var << endl; //输出12
    Derived derived1;
    derived1.var++;
    cout << derived1.var << endl; //输出13
    Base::var++;//通过类名引用
    cout << derived1.var << endl; //输出14
    return 0;
}
