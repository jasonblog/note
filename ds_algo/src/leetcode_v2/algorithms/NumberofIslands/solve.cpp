#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdio>
using namespace std;
struct mypair {
    int first;
    int second;
    mypair(int i, int j): first(i), second(j) {}
};
class Solution
{
public:
    int numIslands(vector<vector<char>>& grid)
    {
        int result = 0;
        int n = grid.size();

        if (n == 0) {
            return 0;
        }

        int m = grid[0].size();

        for (auto i = 0; i < n; ++i) {
            for (auto j = 0; j < m; ++j) {
                if (grid[i][j] == '1') {
                    result++;
                    clear(grid, i, j);
                }
            }
        }

        return result;
    }
private:
    void clear(vector<vector<char>>& grid, int i, int j)
    {
        int n = grid.size();
        int m = grid[0].size();

        if (i < 0 || j < 0 || i >= n || j >= m || grid[i][j] == '0') {
            return;
        }

        grid[i][j] = '0';
        clear(grid, i - 1, j);
        clear(grid, i + 1, j);
        clear(grid, i, j - 1);
        clear(grid, i, j + 1);
    }
    void visited(vector<vector<char>>& grid, size_t i, size_t j)
    {
        queue<mypair> q;
        q.push(mypair(i, j));
        auto n = grid.size();
        auto m = grid[0].size();

        while (!q.empty()) {
            auto p = q.front();
            q.pop();
            auto i = p.first;
            auto j = p.second;
            grid[i][j] = '0';

            if (i - 1 >= 0 && grid[i - 1][j] == '1') {
                q.push(mypair(i - 1, j));
            }

            if (j - 1 >= 0 && grid[i][j - 1] == '1') {
                q.push(mypair(i, j - 1));
            }

            if (i + 1 < n && grid[i + 1][j] == '1') {
                q.push(mypair(i + 1, j));
            }

            if (j + 1 < m && grid[i][j + 1] == '1') {
                q.push(mypair(i, j + 1));
            }
        }
    }
};
void mk_grid(vector<vector<char>>& grid, vector<string> values)
{
    for (auto value : values) {
        vector<char> v;
        for_each(value.begin(), value.end(), [&v](char c) {
            v.push_back(c);
        });
        grid.push_back(v);
    }
}
void print(const vector<vector<char>>& grid)
{
    for (auto g : grid) {
        for_each(g.begin(), g.end(), [](char c) {
            cout << c;
        });
        cout << endl;
    }
}
int main(int argc, char** argv)
{
    Solution solution;
    vector<vector<char>> grid;
    vector<string> s = {"11110", "11010", "11000", "00000"};
    mk_grid(grid, s);
    print(grid);
    cout << solution.numIslands(grid) << endl;
    grid.clear();
    s = {"11000", "11000", "00100", "00011"};
    mk_grid(grid, s);
    print(grid);
    cout << solution.numIslands(grid) << endl;
    grid.clear();
    vector<vector<char>> blank;
    cout << solution.numIslands(blank) << endl;
    return 0;
}
