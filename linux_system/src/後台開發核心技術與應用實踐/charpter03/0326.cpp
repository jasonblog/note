#include <map>
#include <string>
#include <iostream>
using namespace std;
int main()
{
    map<int, string> mapStudent;
    mapStudent[1] = "student_one";
    mapStudent[2] = "student_two";
    mapStudent[3] = "student_three";
    mapStudent[4] = "student_four";
    map<int, string>::iterator iter = mapStudent.begin();

    for (; iter != mapStudent.end();) {
        if ((*iter).second == "student_one") {
            mapStudent.erase(iter++);
        } else {
            ++iter;
        }
    }

    for (iter = mapStudent.begin(); iter != mapStudent.end(); iter++) {
        cout << iter->first << " " << iter->second << endl;
    }

    return 0;
}
