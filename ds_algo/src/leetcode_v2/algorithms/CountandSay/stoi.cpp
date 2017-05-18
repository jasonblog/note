#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
using namespace std;
int main(int argc, char** argv)
{
    /* 使用stoi */
    string s1 = "123";
    cout << stoi(s1) << endl;
    /* 使用sscanf */
    int i;
    sscanf(s1.c_str(), "%d", &i);
    cout << i << endl;
    /* 使用stringstream */
    stringstream ss;
    int j;
    ss << s1;
    ss >> j;
    cout << j << endl;
    return 0;
}
