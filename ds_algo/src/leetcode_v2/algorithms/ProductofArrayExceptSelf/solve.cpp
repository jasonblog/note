#include <vector>
#include <iostream>
#include <cstdio>
#include <algorithm>
using namespace std;
static inline void print(const vector<int>& a)
{
    for_each(begin(a), end(a), [](int i) {
        cout << i << ' ';
    });
    cout << endl;
}
class Solution
{
public:
    /*
    vector<int> productExceptSelf(vector<int>& nums) {
        if (nums.empty()) // 空
            return vector<int>();
        if (nums.size() == 1) { //只有一个元素，返回{0}
            return vector<int>({0});
        }
        auto n = nums.size();
        vector<int> a(n, 1), b(n, 1);
        for (auto i = 1; i < n; ++i) {
            a[i] = a[i - 1] * nums[i - 1];
            b[i] = b[i - 1] * nums[n - i];
        }
        for (auto i = 0; i < n; ++i) {
            a[i] *= b[n - i - 1];
        }
        return a;
    }
    */
    vector<int> productExceptSelf(vector<int>& nums)
    {
        if (nums.empty()) { // 空
            return vector<int>();
        }

        if (nums.size() == 1) { //只有一个元素，返回{0}
            return vector<int>({0});
        }

        int n = nums.size();
        vector<int> result(n, 1);

        for (int i = 1; i < n; ++i) {
            result[i] = result[i - 1] * nums[i - 1];
        }

        int product = 1;

        for (int i = n - 2; i >= 0; --i) {
            product *= nums[i + 1];
            result[i] *= product;
        }

        return result;

    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> v = {1, 2, 3, 4};
    auto result = solution.productExceptSelf(v);
    print(result);
    return 0;
}
