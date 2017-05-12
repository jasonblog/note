#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;
class Solution
{
public:
    int strStr(string s, string p)
    {
        return BF_strStr(s, p);
    }
private:
    /** TLE **/
    int slow_strStr(string s, string p)
    {
        if (p.empty()) {
            return 0;
        }

        if (s.empty()) {
            return -1;
        }

        char first = p[0];
        int n = s.size() - 1;
        int targetCount = p.size();

        for (int i = 0; i <= n; ++i) {
            if (s[i] != first) {
                while (++i <= n && s[i] != first);
            }

            if (i <= n) {
                int j = i + 1;
                int end = j + targetCount - 1;

                for (int k = 1; j < end && s[j] == p[k]; j++, k++);

                if (j == end) {
                    return i;
                }
            }
        }

        return -1;
    }
    int kmp_strStr(string s, string p)
    {
        if (p.empty()) {
            return 0;
        }

        if (s.empty()) {
            return -1;
        }

        int s_len = s.size(), p_len = p.size();
        int next[p_len], i = 0, j = -1;
        next[0] = -1;

        while (i < p_len - 1) {
            if (j == -1 || p[i] == p[j]) {
                i++, j++;

                if (p[i] != p[j]) {
                    next[i] = j;
                } else {
                    next[i] = next[j];
                }
            } else {
                j = next[j];
            }
        }

        i = 0, j = 0;

        while (i < s_len && j < p_len) {
            if (j == -1 || s[i] == p[j]) {
                i++, j++;
            } else {
                j = next[j];
            }
        }

        if (j == p_len) {
            return i - j;
        }

        return -1;
    }
    /* 暴力枚举算法
     *  BF(Brute Force)普通的模式匹配算法
     * */
    int BF_strStr(string s, string p)
    {
        int s_len = s.size();
        int p_len = p.size();

        for (int i = 0; ; ++i) {
            for (int j = 0; ; j++) {
                if (j == p.size()) {
                    return i;
                }

                if (i + j == s.size()) {
                    return -1;
                }

                if (s[i + j] != p[j]) {
                    break;
                }
            }
        }

        return -1;
    }
    int BM_strStr(string s, string p)
    {

    }
};
int main(int argc, char** argv)
{
    Solution solution;
    string s, p;
    cout << solution.strStr(s, p) << endl;
    s = "abcd";
    cout << solution.strStr(s, p) << endl;
    s = string(), p = "abcd";
    cout << solution.strStr(s, p) << endl;

    while (cin >> s >> p) {
        cout << solution.strStr(s, p) << endl;
    }
}
