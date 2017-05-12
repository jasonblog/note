#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
using namespace std;
class Solution
{
public:
    vector<vector<int>> generate(int n)
    {
        vector<vector<int>> result;

        for (int i = 0; i < n; ++i) {
            result.push_back(vector<int>(i + 1, 0));
            result[i][0] = result[i][i] = 1;

            for (int j = 1; j < i; ++j) {
                result[i][j] = result[i - 1][j - 1] + result[i - 1][j];
            }
        }

        return result;
    }
};
void print(vector<vector<int>> v)
{
    for (auto i : v) {
        for_each(begin(i), end(i), [](int i) {
            cout << i << ' ';
        });
        cout << endl;
    }
}
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (scanf("%d", &n) != EOF) {
        print(solution.generate(n));
    }

    return 0;
}
