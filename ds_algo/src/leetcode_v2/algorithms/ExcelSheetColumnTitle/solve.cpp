#include <string>
#include <iostream>
#include <algorithm>
#include <cstdio>
using namespace std;
class Solution
{
public:
    string convertToTitle(int n)
    {
        string result;

        while (n) {
            result.push_back(--n % 26 + 'A');
            n /= 26;
        }

        return string(result.rbegin(), result.rend());
    }
};
int main(int argc, char** argv)
{
    int n;
    Solution solution;

    while (scanf("%d", &n) != EOF) {
        cout << solution.convertToTitle(n) << endl;
    }
}
