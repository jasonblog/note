#include <string>
#include <algorithm>
#include <iostream>
#include <unordered_map>
using namespace std;
class Solution
{
public:
    vector<string> anagrams(const vector<string>& strs)
    {
        unordered_map<string, int> m;
        vector<string> result;

        for (int i = 0; i < strs.size(); ++i) {
            string s = strs[i];
            sort(begin(s), end(s));

            if (m.find(s) == m.end()) {
                m[s] = i;
            } else {
                if (m[s] >= 0) {
                    result.push_back(strs[m[s]]);
                    m[s] = -1;
                }

                result.push_back(strs[i]);
            }
        }

        return result;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<string> strs = {"tea", "and", "ate", "eat", "den"};
    auto result = solution.anagrams(strs);
    for_each(begin(result), end(result), [](string s) {
        cout << s << " ";
    });
    cout << endl;
    return 0;
}
