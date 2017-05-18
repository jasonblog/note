#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
class Solution
{
public:
    int lengthOfLongestSubstring(string s)
    {
        vector<char> v;
        unsigned int max = 0;

        for (size_t i = 0; i < s.size(); ++i) {
            auto pos = find(v.begin(), v.end(), s[i]);

            if (pos != v.end()) {
                max = max >= v.size() ? max : v.size();
                v.erase(v.begin(), pos + 1);
            }

            v.push_back(s[i]);
        }

        return max >= v.size() ? max : v.size();
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    string s = "abcadef";
    cout << solution.lengthOfLongestSubstring(s) << endl;
    return 0;
}
