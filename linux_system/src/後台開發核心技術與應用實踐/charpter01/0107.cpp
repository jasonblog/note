#include<iostream>
#include<string>
using namespace std;
int main()
{
    char str[] = "I am a programmer." ;//str 是一个字符数组
    char* str1 =
        "abc"; //str1是一个字符指针变量，可以指向一个字符串
    char* str2[] = {"hello world", "good bye"}; //str2是一个字符指针数组，可以存多个字符串
    string str3 = "I am a programmer, too.";  // str3是一个字符串变量
    cout << "str: " << str << endl;
    cout << "str1: " << str1 << endl;
    cout << "str2[0]: " << str2[0] << endl;
    cout << "str3: " << str3 << endl;
    return 0;
}
