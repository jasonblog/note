#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>
using namespace std;
class Solution
{
public:
    Solution()
    {
        map['2'] = "abc";
        map['3'] = "def";
        map['4'] = "ghi";
        map['5'] = "jkl";
        map['6'] = "mno";
        map['7'] = "pqrs";
        map['8'] = "tuv";
        map['9'] = "wxyz";
    }
    vector<string> letterCombinations(string digits)
    {
        vector<string> result;

        if (digits.size() == 0) {
            return result;
        }

        for (char c : digits) {
            result = generate(result, c);
        }

        return result;
    }
    vector<string> generate(vector<string> current, char digit)
    {
        vector<string> result;

        if (current.size() > 0) {
            for (char c : map[digit]) {
                for_each(current.begin(), current.end(), [&result, c](string s) {
                    result.push_back(s + c);
                });
            }
        } else {
            for (char c : map[digit]) {
                result.push_back(string() + c);
            }
        }

        return result;
    }
private:
    unordered_map<char, string> map;
};
int main(int argc, char** argv)
{
    Solution solution;
    auto result = solution.letterCombinations("23");
    for_each(result.begin(), result.end(), [](string s) {
        cout << s << endl;
    });
    return 0;
}
