#include <string>
#include <iostream>
using namespace std;
//void foo(char *s) {
//  cout << s << endl;
//}
void foo(const char* s)
{
    cout << s << endl;
}
void foo(string s)
{
    foo(s.c_str());
}
int main(int argc, char** argv)
{
    string s = "hello";
    foo(s);
    return 0;
}
