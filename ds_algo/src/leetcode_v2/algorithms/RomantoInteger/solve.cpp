#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;
class Solution
{
public:
    int romanToInt(string s)
    {
        int pre = ROMAN_TABLE[s[0]];
        int sum = pre;

        for (int i = 1; i < s.size(); ++i) {
            int cur = ROMAN_TABLE[s[i]];
            sum += cur;

            if (cur > pre) {
                sum -= 2 * pre; // 原来是＋，需要转化为－，因此减去两倍
            }

            pre = cur;
        }

        return sum;
    }
private:
    unordered_map<char, int> ROMAN_TABLE = {
        {'I', 1},
        {'V', 5},
        {'X', 10},
        {'L', 50},
        {'C', 100},
        {'D', 500},
        {'M', 1000}
    };
};
int main(int argc, char** argv)
{
    Solution solution;
    string s;

    while (cin >> s) {
        cout << solution.romanToInt(s) << endl;
    }

    return 0;
}
