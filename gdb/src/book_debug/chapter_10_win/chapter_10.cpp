// chapter_10.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
int main()
{
    char name[10];
    OutputDebugString("输出调试信息");
    
    //while (true)
    //{
    //    Sleep(1000);
    //}
    bool isdebug = IsDebuggerPresent();
    if (isdebug)
    {
        std::cout << "正在调试器运行\n";
    }
    else
    {
        std::cout << "没有在调试器运行\n";
    }
    std::cout << "你好:" << name << std::endl;
}

