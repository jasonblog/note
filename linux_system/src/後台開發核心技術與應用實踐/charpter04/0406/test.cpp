#include<iostream>
using namespace std;
class Test
{
public:
    int Hello()
    {
        cout << "Hello world!" << endl;
        return 0;
    }
};

int main()
{
    Test test;
    int iRet = test.Hello();
    cout << "iRet=" << iRet << endl;
    return 0;
}
