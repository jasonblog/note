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
        //vector<vector<int>> g(n, vector<int>(n, 0));
        vector<int> degree(n, 0);

        for (auto p : request) {
            //g[p.second][p.first] = 1;
            degree[p.first]++;
        }

        return topsort(request, degree);
    }
private:
    /* BFS */
    bool topsort(const vector<pair<int, int>>& request, vector<int>& degree)
    {
        int n = degree.size();
        vector<bool> visited(n, false);
        int sum = 0;

        while (sum < n) { // 还没有访问完
            int cur = findZero(degree);

            if (cur >= 0) { // 访问节点cur
                sum++;
                degree[cur] = -1; // 标记当前节点为已经访问状态

                for (auto p : request) {
                    if (p.second == cur) {
                        degree[p.first]--;    // 去掉已访问节点
                    }
                }
            } else {
                return false;
            }
        }

        return true;
    }
    int findZero(const vector<int>& v)
    {
        int n = v.size();

        for (int i = 0; i < n; ++i) {
            if (v[i] == 0) {
                return i;
            }
        }

        return -1;
    }

};
int main(int argc, char** argv)
{
    Solution solution;
    vector<pair<int, int>> request = {make_pair(1, 0), make_pair(2, 1), make_pair(3, 1)};
    cout << solution.canFinish(4, request) << endl;
    return 0;
}
