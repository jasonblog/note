#include<map>
#include<string>
#include<iostream>
using namespace std;
int main()
{
    map<int, string> mapStudent;
    mapStudent[1] = "student_one";
    mapStudent[2] = "student_two";
    mapStudent[3] = "student_three";
    map<int, string>::iterator iter = mapStudent.find(1);

    if (iter != mapStudent.end()) {
        cout << "Found, the value is " << iter->second << endl;
    } else {
        cout << "Do not found" << endl;
    }

    return 0;
}
