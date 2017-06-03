#include <map>
#include <string>
#include <iostream>
using namespace std;
int main()
{
    map<int, string> mapStudent;
    pair<map<int, string>::iterator, bool> insert_pair;
    insert_pair = mapStudent.insert(pair<int, string>(1, "student_one"));

    if (insert_pair.second == true) {
        cout << "Insert Successfully" << endl;
    } else {
        cout << "Insert Failure" << endl;
    }

    insert_pair = mapStudent.insert(pair<int, string>(1, "student_two"));

    if (insert_pair.second == true) {
        cout << "Insert Successfully" << endl;
    } else {
        cout << "Insert Failure" << endl;
    }

    map<int, string>::iterator iter;

    for (iter = mapStudent.begin(); iter != mapStudent.end(); iter++) {
        cout << iter->first << " " << iter->second << endl;
    }

    return 0;
}
