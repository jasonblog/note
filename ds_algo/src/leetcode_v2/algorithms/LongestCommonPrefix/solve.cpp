#include <string>
#include <iostream>
#include <cstdio>
#include <vector>
using namespace std;
class Solution
{
public:
    string longestCommonPrefix(vector<string>& strs)
    {
        int n = strs.size();

        if (n < 1) {
            return "";
        }

        int m = strs[0].size();
        int i, j;

        for (i = 0; i < m; ++i) {
            for (auto s : strs) {
                if (i >= s.size() || s[i] != strs[0][i]) {
                    return strs[0].substr(0, i);
                }
            }
        }

        return strs[0];
    }

};
int main(int argc, char** argv)
{
    Solution solution;
    vector<string> s = {"abcd", "abc", "abedefg", "abcdefgh"};
    cout << solution.longestCommonPrefix(s) << endl;
}
