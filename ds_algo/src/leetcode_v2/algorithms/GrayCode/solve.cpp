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
        return grayCode2(n);
    }
private:
    vector<int> grayCode1(int n)
    {
        vector<int> result;

        for (int i = 0; i < (1 << n); ++i) {
            result.push_back(i ^ (i >> 1));
        }

        return result;
    }
    vector<int> grayCode2(int n)
    {
        vector<int> result;
        result.push_back(0);

        for (int i = 0; i < n; ++i) {
            int high = 1 << i;

            for (int j = result.size() - 1; j >= 0; --j) {
                result.push_back(result[j] + high);
            }
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
