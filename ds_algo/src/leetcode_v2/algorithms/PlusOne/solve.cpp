#include <vector>
#include <cstdio>
#include <iostream>
#include <algorithm>
using namespace std;
class Solution
{
public:
    vector<int> plusOne(vector<int>& digits)
    {
        int i = digits.size() - 1;

        if (i < 0) {
            digits.push_back(1);
            return digits;
        }

        bool carry = true;

        while (i >= 0 && carry) {
            digits[i] += 1;

            if (digits[i] > 9) {
                digits[i] -= 10;
                carry = true;
            } else {
                carry = false;
            }

            i--;
        }

        if (carry) {
            digits.insert(digits.begin(), 1);
        }

        return digits;
    }
};
int main(int argc, char** argv)
{
    int n;
    vector<int> v;
    Solution solution;

    while (cin >> n) {
        v.clear();

        for (int i = 0; i < n; ++i) {
            int value;
            cin >> value;
            v.push_back(value);
        }

        for_each(v.begin(), v.end(), [](int i) {
            cout << i;
        });
        cout << " + 1 = ";
        auto result = solution.plusOne(v);
        for_each(result.begin(), result.end(), [](int i) {
            cout << i;
        });
        cout << endl;
    }

    return 0;
}
