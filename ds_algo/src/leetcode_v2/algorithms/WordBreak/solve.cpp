#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
using namespace std;
class Solution
{
public:
    bool wordBreak(string s, const unordered_set<string>& dict)
    {
        int n = s.size();
        vector<bool> canBreak(n + 1, false);
        canBreak[0] = true;

        for (int i = 1; i <= n; ++i) {
            for (int j = 0; j < i; ++j) {
                if (canBreak[j] && dict.find(s.substr(j, i - j)) != dict.end()) {
                    canBreak[i] = true;
                    break;
                }
            }
        }

        return canBreak[n];
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    string s = "leetcode";
    unordered_set<string> dict = {"leet", "code"};
    cout << solution.wordBreak(s, dict) << endl;
    return 0;
}
