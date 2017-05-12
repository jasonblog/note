#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
using namespace std;
class Solution
{
public:
    bool canFinish(int n, vector<pair<int, int>>& request)
    {
        vector<int> visited(n, 0);

        for (int i = 0; i < n; ++i)
            if (!dfs(request, visited, i)) {
                return false;
            }

        return true;
    }
private:
    bool dfs(const vector<pair<int, int>>& request, vector<int>& visited, int i)
    {
        if (visited[i] == -1) {
            return false;
        }

        if (visited[i] == 1) {
            return true;
        }

        visited[i] = -1; // -1 表示正在访问

        for (auto p : request) {
            if (p.second == i) {
                if (!dfs(request, visited, p.first)) {
                    return false;
                }
            }
        }

        visited[i] = 1;
        return true;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<pair<int, int>> request = {make_pair(1, 0), make_pair(2, 1), make_pair(0, 2)};
    cout << solution.canFinish(3, request) << endl;
    return 0;
}
