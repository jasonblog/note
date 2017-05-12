#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
using namespace std;
int main(int argc, char** argv)
{
    /* Method 1 使用sprintf */
    char buf[20];
    sprintf(buf, "%d", 123);
    string s1(buf);
    cout << "s1 = " << s1 << endl;

    /* Method 2 使用stringstream */
    stringstream ss;
    ss << 123;
    string s2 = ss.str();
    s2.push_back('c');
    cout << "s2 = " << s2 << endl;
    return 0;
}
