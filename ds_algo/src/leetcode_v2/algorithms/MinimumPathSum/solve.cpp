#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
class Solution
{
public:
    int minPathSum(vector<vector<int>>& grid)
    {
        return minPathSum1(grid);
    }
private:
    int minPathSum1(vector<vector<int>>& grid)
    {
        if (grid.size() < 1) {
            return 0;
        }

        int n = grid.size();
        int m = grid[0].size();
        vector<vector<int>> dp(n, vector<int>(m, 0));
        dp[0][0] = grid[0][0];

        for (int i = 1; i < n; ++i) {
            dp[i][0] = dp[i - 1][0] + grid[i][0];
        }

        for (int j = 1; j < m; ++j) {
            dp[0][j] = dp[0][j - 1] + grid[0][j];
        }

        for (int i = 1; i < n; ++i)
            for (int j = 1; j < m; ++j) {
                dp[i][j] = min(dp[i - 1][j], dp[i][j - 1]) + grid[i][j];
            }

        return dp[n - 1][m - 1];
    }
    int minPathSum2(vector<vector<int>>& grid)
    {
        if (grid.size() < 1) {
            return 0;
        }

        int n = grid.size();
        int m = grid[0].size();
        vector<int> dp(n, 0);
        dp[0] = grid[0][0];

        for (int i = 1; i < n; ++i) {
            dp[i] = dp[i - 1] + grid[i][0];
        }

        for (int i = 1; i < n; ++i)
            for (int j = 1; j < n; ++j) {
                dp[j] = min(dp[j], dp[j - 1]) + grid[i][j];
            }

        return dp[n - 1];
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    int n, m;

    while (scanf("%d%d", &n, &m) != EOF) {
        vector<vector<int>> v(n, vector<int>(m, 0));

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j) {
                cin >> v[i][j];
            }

        cout << solution.minPathSum(v) << endl;
    }

    return 0;
}
