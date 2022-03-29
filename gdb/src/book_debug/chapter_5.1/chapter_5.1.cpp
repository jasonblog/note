// chapter_5.1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
using namespace std;
/*
#include "../chapter_5.1_dll/chapter_5.1_dll.h"
#pragma comment(lib,"../debug/chapter51dll.lib")

void load_time_test()
{
    double pi = calc_pi();
    Cchapter51dll test;
    int total = test.add(100, 10);
    int res = test.sub(total, 10);
    cout << "pi is " << pi << endl;
    cout << "add is " << total << endl;
    cout << "sub  res is " << res << endl;
}
 */
void run_time_test()
{
    typedef double (*CALC_PI)();
    HINSTANCE hmodule = LoadLibrary("chapter51dll.dll");
    if (!hmodule)
        return;
    CALC_PI calc_pi = (CALC_PI)GetProcAddress(hmodule, "calc_pi");
    if (calc_pi)
    {
        double pi = calc_pi();
        cout << "pi is " << pi << endl;
    }
    FreeLibrary(hmodule);
}

int main()
{
    //load_time_test();
    std::cout << "请选择:\n1:动态调用动态库函数2:退出程序!\n";
    while (true)
    {
        switch (getchar())
        {
        case '1':
            run_time_test();
            break;
        case '2':
            return 0;
        }
    }         
} 


