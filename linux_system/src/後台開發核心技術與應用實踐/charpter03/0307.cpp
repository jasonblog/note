#include<iostream>
#include<stdlib.h>
#include<string>
using namespace std;
int main()
{
    char* endptr;
    char nptr[] = "123abc";
    int ret = strtol(nptr, &endptr, 10);
    cout << "ret:" << ret << endl;
    cout << "endptr:" << endptr << endl;

    char* endptr2;
    char nptr2[] = " \n\t    abc";
    ret = strtol(nptr2, &endptr2, 10);
    cout << "ret:" << ret << endl;
    cout << "endptr2:" << endptr2 << endl;

    char* endptr8;
    char nptr8[] = "0123";
    ret = strtol(nptr8, &endptr8, 0);
    cout << "ret:" << ret << endl;
    cout << "endptr8:" << endptr8 << endl;

    char* endptr16;
    char nptr16[] = "0x123";
    ret = strtol(nptr16, &endptr16, 0);
    cout << "ret:" << ret << endl;
    cout << "endptr16:" << endptr16 << endl;

    return 0;
}
