#include<iostream>
#include<string>
#include<string.h>
using namespace std;
int main()
{
    char* cstr = new char[20];
    string str = "Hello world.";
    strncpy(cstr, str.c_str(), str.size());
    cout << cstr << endl;
    str = "Abcd.";
    cout << cstr << endl;
    return 0;
}
