#include <string>
#include <map>
#include <iostream>
#define N 256
using namespace std;
class Solution
{
public:
    bool isIsomorphic(string s, string t)
    {
        int map1[N] = {0}, map2[N] = {0};
        int n = s.size();

        if (n != t.size()) {
            return false;
        }

        for (int i = 0; i < n; ++i) {
            char a = s[i], b = t[i];

            if (map1[a] != map2[b]) {
                return false;
            }

            if (map1[a] == 0) {
                map1[a]  = map2[b] = i + 1; // avoid 0
            }
        }

        return true;
    }
};
int main(int argc, char** argv)
{
    string s1, s2;
    Solution solution;

    while (cin >> s1 >> s2) {
        cout << solution.isIsomorphic(s1, s2) << endl;
    }
}
