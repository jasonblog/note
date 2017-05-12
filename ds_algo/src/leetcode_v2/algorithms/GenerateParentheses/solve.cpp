#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
class Solution
{
public:
    vector<string> generateParenthesis(int n)
    {
        vector<string> result;

        if (n < 1) {
            return result;
        }

        generate(result, "", 0, n);
        return result;
    }
    void generate(vector<string>& result, string value, int open, int n)
    {
        if (n == 0) {
            for (int i = 0; i < open; ++i) {
                value += ')';
            }

            result.push_back(value);
            return;
        }

        if (open == 0) {
            generate(result, value + '(', 1, n - 1);
        } else {
            generate(result, value + ')', open - 1, n);
            generate(result, value + '(', open + 1, n - 1);
        }
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    auto result = solution.generateParenthesis(10);
    for_each(result.begin(), result.end(), [](string s) {
        cout << s << endl;
    });
}
