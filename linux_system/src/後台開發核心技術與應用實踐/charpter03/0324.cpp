#include<map>
#include<string>
#include<iostream>
using namespace std;
int main()
{
    map<int, string> mapStudent;
    mapStudent[1] =  "student_one";
    mapStudent[2] =  "student_two";
    mapStudent[3] =  "student_three";
    int iSize = mapStudent.size();

    for (int i = 1; i <= iSize; i++) {
        cout << i << " " << mapStudent[i] << endl;
    }

    return 0;
}
