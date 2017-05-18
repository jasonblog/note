#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
class Solution
{
public:
    int minimumTotal(vector<vector<int>>& a)
    {
        return compress_minimumTotal(a);
    }
    int positive_minimumTotal(vector<vector<int>>& a)
    {
        int n = a.size();

        if (n < 1) {
            return 0;
        }

        vector<vector<int>> dp(n, vector<int>(n, 0));
        dp[0][0] = a[0][0];

        for (int i = 1; i < n; ++i) {
            dp[i][0] = dp[i - 1][0] + a[i][0];
            dp[i][i] = dp[i - 1][i - 1] + a[i][i];

            for (int j = 1; j < i; ++j) {
                dp[i][j] = min(dp[i - 1][j], dp[i - 1][j - 1]) + a[i][j];
            }
        }

        return *min_element(begin(dp[n - 1]), end(dp[n - 1]));
    }
    int inversed_minimumTotal(const vector<vector<int>>& a)
    {
        int n = a.size();

        if (n < 1) {
            return 0;
        }

        vector<vector<int>> dp(n, vector<int>(n, 0));

        for (int i = 0; i < a[n - 1].size(); ++i) {
            dp[n - 1][i] = a[n - 1][i];
        }

        for (int i = n - 2; i >= 0; --i) {
            for (int j = 0; j <= i; ++j) {
                dp[i][j] = min(dp[i + 1][j], dp[i + 1][j + 1]) + a[i][j];
            }
        }

        return dp[0][0];
    }
    int compress_minimumTotal(const vector<vector<int>>& a)
    {
        int n = a.size();

        if (n < 1) {
            return 0;
        }

        vector<int> dp;
        for_each(begin(a[n - 1]), end(a[n - 1]), [&dp](int i) {
            dp.push_back(i);
        });

        for (int i = n - 2; i >= 0; --i) {
            for (int j = 0; j <= i; ++j) {
                dp[j] = min(dp[j], dp[j + 1]) + a[i][j];
            }
        }

        return dp[0];
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<vector<int>> triangle = {
        {2},
        {3, 4},
        {6, 5, 7},
        {4, 1, 8, 3}
    };
    cout << solution.minimumTotal(triangle) << endl;

}
