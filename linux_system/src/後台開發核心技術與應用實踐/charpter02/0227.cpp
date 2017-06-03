#include<iostream>
using namespace std;
class CBase
{
public:
    CBase()
    {
        std::cout << "CBase::CBase()" << std::endl;
    }
    ~ CBase()
    {
        std::cout << "CBase::~CBase()" << std::endl;
    }
};

class CBase1: public CBase
{
public:
    CBase1()
    {
        std::cout << "CBase::Base1()" << std::endl;
    }
    ~ CBase1()
    {
        std::cout << "CBase::~Base1()" << std::endl;
    }
};

class CDerive
{
public:
    CDerive()
    {
        std::cout << "CDerive::CDerive()" << std::endl;
    }
    ~ CDerive()
    {
        std::cout << "CDerive::~CDerive()" << std::endl;
    }
};

class CDerive1: public CBase1
{
private:
    CDerive m_derive;
public:
    CDerive1()
    {
        std::cout << "CDerive1::CDerive1()" << std::endl;
    }
    ~CDerive1()
    {
        std::cout << "CDerive1::~CDerive1()" << std::endl;
    }
};

int main()
{
    CDerive1 derive;
    return 0;
}
