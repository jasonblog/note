#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
using namespace std;
int gcd(int a, int b)
{
    if (a < b) {
        return gcd(b, a);
    }

    if (b == 0) {
        return a;
    }

    if ((a & 0x1) == 0) {
        if ((b & 0x1) == 0) {
            return gcd(a >> 1, b >> 1) << 1;
        } else {
            return gcd(a >> 1, b);
        }
    } else {
        if ((b & 0x1) == 0) {
            return gcd(a, b >> 1);
        } else {
            return gcd(b, a - b);
        }
    }
}
class Solution
{
public:
    vector<int> getRow(int n)
    {
        vector<int> result(n + 1, 0);
        result[0] = 1;

        for (int i = 1; i <= n; ++i) { // 必须约分， 否则溢出
            int molecular = (n - i + 1), denominator = i;
            int g = gcd(molecular, denominator);
            molecular /= g;
            denominator /= g;
            int multiplier = result[i - 1];
            g = gcd(multiplier, denominator);
            multiplier /= g;
            denominator /= g;
            int value = multiplier * molecular / denominator;
            result[i] = value;
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
void print(vector<int> v)
{
    for_each(v.begin(), v.end(), [](int i) {
        cout << i << ' ';
    });
    cout << endl;
}
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (scanf("%d", &n) != EOF) {
        print(solution.getRow(n));
    }

    return 0;
}
