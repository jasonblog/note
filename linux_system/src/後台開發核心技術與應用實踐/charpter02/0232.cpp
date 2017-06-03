#include<iostream>
using namespace std;
class Base
{
public:
    Base()
    {
        std::cout << "Base::Base()" << std::endl;
    }
    virtual ~Base()
    {
        std::cout << "Base::~Base()" << std::endl;
    }
};

class Derive: public Base
{
public:
    Derive()
    {
        std::cout << "Derive::Derive()" << std::endl;
    }
    ~Derive()
    {
        std::cout << "Derive::~Derive()" << std::endl;
    }
};

int main()
{
    Base* pBase = new Derive();
    delete pBase;
    return 0;
}
