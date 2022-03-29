// chapter_9.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#pragma optimize("",off)
int release_test(int i,int j)
{
    int x = i + j;
    return x;
}       
#pragma optimize("", on)

int main(int argc,char**argv)
{
    int x = release_test(10, 20);
    std::cout << "x is " << x << std::endl;
}
