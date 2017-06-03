#include <map>
#include <string>
#include <iostream>
using namespace std;
int main()
{
    map<string, int, greater<string> > mapStudent;
    mapStudent["LiMin"] = 90;
    mapStudent["ZiLinMi"] = 72;
    mapStudent["BoB"] = 79;
    map<string, int>::iterator iter = mapStudent.begin();

    for (iter = mapStudent.begin(); iter != mapStudent.end(); iter++) {
        cout << iter->first << " " << iter->second << endl;
    }

    return 0;
}
