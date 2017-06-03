#include <map>
#include <string>
#include <iostream>
using namespace std;
struct CmpByKeyLength {
    bool operator()(const string& k1, const string& k2)
    {
        return k1.length() < k2.length();
    }
};
int main()
{
    map<string, int, CmpByKeyLength > mapStudent;
    mapStudent["LiMin"] = 90;
    mapStudent["ZiLinMi"] = 72;
    mapStudent["BoB"] = 79;
    map<string, int>::iterator iter = mapStudent.begin();

    for (iter = mapStudent.begin(); iter != mapStudent.end(); iter++) {
        cout << iter->first << " " << iter->second << endl;
    }

    return 0;
}
