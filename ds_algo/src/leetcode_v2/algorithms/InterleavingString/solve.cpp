#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
using namespace std;
class Solution
{
public:
    bool isInterleave(string s1, string s2, string s3)
    {
        return recursive_isInterleave(s1, s2, s3);
    }
private:
    bool dp_isInterleave(string s1, string s2, string s3)
    {
        int len1 = s1.size(), len2 = s2.size(), len3 = s3.size();

        if (len1 + len2 != len3) {
            return false;
        }

        vector<vector<int>> dp(len1 + 1, vector<int>(len2 + 1, false));
        dp[0][0] = true;

        for (int i = 1; i <= len1; ++i)
            if (s1[i - 1] == s3[i - 1]) {
                dp[i][0] = true;
            } else {
                break;
            }

        for (int j = 1; j <= len2; ++j)
            if (s2[j - 1] == s3[j - 1]) {
                dp[0][j] = true;
            } else {
                break;
            }

        for (int i = 1; i <= len1; ++i)
            for (int j = 1; j <= len2; ++j) {
                if (s1[i - 1] == s3[i + j - 1]) {
                    dp[i][j] |= dp[i - 1][j];
                }

                if (s2[j - 1] == s3[i + j - 1]) {
                    dp[i][j] |= dp[i][j - 1];
                }
            }

        return dp[len1][len2];
    }
    bool recursive_isInterleave(string s1, string s2, string s3)
    {
        const char* p1 = s1.c_str(), *p2 = s2.c_str(), *p3 = s3.c_str();
        return recursive_isInterleave(p1, p2, p3);
    }
    bool recursive_isInterleave(const char* s1, const char* s2, const char* s3)
    {
        if (*s1 == 0 && *s2 == 0 && *s3 == 0) {
            return true;
        }

        if (*s1 == *s3 && recursive_isInterleave(s1 + 1, s2, s3 + 1)) {
            return true;
        }

        if (*s2 == *s3 && recursive_isInterleave(s1, s2 + 1, s3 + 1)) {
            return true;
        }

        return false;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    char s1[20], s2[20], s3[40];
    cout << solution.isInterleave(string(), "b", "b") << endl;

    while (scanf("%s%s%s", s1, s2, s3) != EOF) {
        cout << solution.isInterleave(string(s1), string(s2), string(s3)) << endl;
    }

    return 0;
}
