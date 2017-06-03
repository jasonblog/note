#include <map>
#include <string>
#include <iostream>
using namespace std;
int main()
{
    map<int, string> mapStudent;
    mapStudent[1] =  "student_one";
    mapStudent[2] =  "student_two";
    mapStudent[3] =  "student_three";
    map<int, string>::reverse_iterator   iter;

    for (iter = mapStudent.rbegin(); iter != mapStudent.rend(); iter++) {
        cout << iter->first << " " << iter->second << endl;
    }

    return 0;
}
