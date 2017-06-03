#include<iostream>
using namespace std;
class CSingleton
{
private:
    CSingleton()    //构造函数是私有的
    {
    }
    static CSingleton* m_pInstance;
public:
    static CSingleton* GetInstance()
    {
        if (m_pInstance == NULL) { //判断是否第一次调用
            m_pInstance = new CSingleton();
        }

        return m_pInstance;
    }
};
CSingleton* CSingleton::m_pInstance = NULL; //初始化静态数据成员
int main()
{
    CSingleton* s1 = CSingleton::GetInstance();
    CSingleton* s2 = CSingleton::GetInstance();

    if (s1 == s2) {
        cout << "s1=s2" << endl; //程序的执行结果是输出了s1=s2
    }

    return 0;
}
