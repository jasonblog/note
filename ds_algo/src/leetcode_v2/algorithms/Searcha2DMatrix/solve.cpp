#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
class Solution
{
public:
    bool searchMatrix(vector<vector<int>>& matrix, int target)
    {
        return nestBinary_searchMatrix(matrix, target);
    }
private:
    /**
     * O(n + m)
     */
    bool line_searchMatrix(const vector<vector<int>>& matrix, int target)
    {
        if (matrix.empty() || matrix[0].empty()) {
            return false;
        }

        int n = matrix.size(), m = matrix[0].size();
        int i = 0, j = m - 1;

        while (i < n && j >= 0) {
            if (matrix[i][j] == target) {
                return true;
            }

            if (matrix[i][j] > target) {
                j--;
            } else {
                i++;
            }
        }

        return false;
    }
    /**
     * O(Log(n * m))
     */
    bool binary_searchMatrix(const vector<vector<int>>& matrix, int target)
    {
        if (matrix.empty() || matrix[0].empty()) {
            return false;
        }

        int n = matrix.size(), m = matrix[0].size();
        int s = 0, t = n * m - 1;

        while (s <= t) {
            int mid = s + ((t - s) >> 1);
            int i = mid / m, j = mid % m;

            if (matrix[i][j] == target) {
                return true;
            }

            if (matrix[i][j] > target) {
                t = mid - 1;
            } else {
                s = mid + 1;
            }
        }

        return false;
    }
    bool binarySearch(const vector<int>& a, int target)
    {
        int s = 0, t = a.size() - 1;

        while (s <= t) {
            int mid = s + ((t - s) >> 1);

            if (a[mid] == target) {
                return true;
            }

            if (a[mid] > target) {
                t = mid - 1;
            } else {
                s = mid + 1;
            }
        }

        return false;
    }
    /**
     * O(Log(n) + Log(m))
     */
    bool nestBinary_searchMatrix(const vector<vector<int>>& matrix, int target)
    {
        if (matrix.empty()) {
            return false;
        }

        int s = 0, t = matrix.size() - 1, n = matrix[0].size();

        while (s <= t) {
            int mid = s + ((t - s) >> 1);

            if (target < matrix[mid][0]) {
                t = mid - 1;
            } else if (target > matrix[mid][n - 1]) {
                s = mid + 1;
            } else {
                return binarySearch(matrix[mid], target);
            }
        }

        return false;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    int n, m, k;

    while (cin >> n >> m >> k) {
        vector<vector<int>> matrix(n, vector<int>(m, 0));

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j) {
                cin >> matrix[i][j];
            }

        cout << solution.searchMatrix(matrix, k) << endl;
    }

    return 0;
}
