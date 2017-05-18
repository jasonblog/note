#include <string>
#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>

using namespace std;
class Solution
{
public:
    string countAndSay(int n)
    {
        n--;
        string result = "1";

        while (n--) {
            result = next(result);
        }

        return result;
    }
private:
    string next(string s)
    {
        int n = s.size();

        if (n < 1) {
            return "";
        }

        char cur = s[0];
        int count = 1;
        string result;

        for (int i = 1; i < n; ++i) {
            if (cur != s[i]) {
                result += itos(count);
                result.push_back(cur);
                cur = s[i];
                count = 1;
            } else {
                ++count;
            }
        }

        result += itos(count);
        result += cur;
        return result;
    }
    string itos(int i)
    {
        ss.str("");
        ss.clear();
        ss << i;
        return ss.str();
    }
    stringstream ss;
};
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (scanf("%d", &n) != EOF) {
        cout << solution.countAndSay(n) << endl;
    }

    return 0;

}
