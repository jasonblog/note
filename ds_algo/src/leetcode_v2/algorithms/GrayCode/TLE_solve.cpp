#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <unordered_set>
using namespace std;
class Solution
{
public:
    vector<int> grayCode(int n)
    {
        vector<int> result;
        vector<int> cur;
        int N = 1;

        for (int i = 0; i < n; ++i) {
            N <<= 1;
        }

        dfs(result, cur, n, 0, N);
        return result;
    }
private:
    void dfs(vector<int>& result, vector<int> cur, int n, int x, int N)
    {
        cur.push_back(x);

        if (N == 1) {
            result = vector<int>(begin(cur), end(cur));
            return;
        }

        vector<int> next = getPossibleNext(n, x);

        for (auto i : next) {
            if (find(begin(cur), end(cur), i) == end(cur)) {
                dfs(result, cur, n, i, N - 1);
            }
        }
    }
    vector<int> getPossibleNext(int n, int x)
    {
        int base = 0x1;
        vector<int> result;

        for (int i = 0; i < n; ++i) {
            result.push_back(base ^ x);
            base <<= 1;
        }

        return result;
    }
};
void print(vector<int> v)
{
    cout << "[";
    for_each(begin(v), end(v), [](int i) {
        cout << i << ' ';
    });
    cout << "]";
    cout << endl;
}
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (cin >> n) {
        print(solution.grayCode(n));
    }

    return 0;
}
