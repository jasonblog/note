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
    vector<int> findOrder(int n, vector<pair<int, int>>& request)
    {
        return topsort(n, request);
    }
private:
    /* BFS */
    vector<int> topsort(int n, const vector<pair<int, int>>& request)
    {
        vector<int> result;
        vector<int> degree(n, 0);

        for (auto p : request) {
            degree[p.first]++;
        }

        while (result.size() < n) { // 还没有访问完
            int cur = findZero(degree);

            if (cur >= 0) { // 访问节点cur
                result.push_back(cur);
                degree[cur] = -1; // 标记当前节点为已经访问状态

                for (auto p : request) {
                    if (p.second == cur) {
                        degree[p.first]--;    // 去掉已访问节点
                    }
                }
            } else {
                return vector<int>();
            }
        }

        return result;
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
void print(vector<int> v)
{
    for_each(begin(v), end(v), [](int i) {
        cout << i << ' ';
    });
    cout << endl;
}
int main(int argc, char** argv)
{
    Solution solution;
    vector<pair<int, int>> request = {make_pair(1, 0), make_pair(2, 0), make_pair(3, 1), make_pair(3, 2)};
    print(solution.findOrder(4, request));
    return 0;
}
