#include <vector>
#include <map>
#include <set>
#include <cstdio>
#include <algorithm>
using namespace std;
class Solution
{
public:
    vector<vector<int>> fourSum(vector<int>& num, int target)
    {
        vector<vector<int>> result;
        int n = num.size();

        if (n < 4) {
            return result;
        }

        sort(num.begin(), num.end());

        for (int i = 0; i < n - 3; ++i) {
            if (i > 0 && num[i] == num[i - 1]) {
                continue;
            }

            vector<int> n(num.begin() + i + 1, num.end());
            vector<vector<int>> ret = threeSum(n, target - num[i]);

            for (auto v : ret) {
                v.insert(v.begin(), num[i]);
                result.push_back(v);
            }
        }

        return result;
    }
    vector<vector<int>> threeSum(vector<int>& num, int target)
    {
        //  sort(num.begin(), num.end());
        vector<vector<int>> result;
        auto n = num.size();
        size_t i;

        for (i = 0; i < n; ++i) {
            if (i > 0 && num[i] == num[i - 1]) {
                continue;
            }

            int a = num[i];
            int low = i + 1, high = n - 1;

            while (low < high) {
                int b = num[low];
                int c = num[high];

                if (a + b + c == target) {
                    vector<int> v = {a, b, c};
                    result.push_back(v);

                    while (low + 1 < n && num[low] == num[low + 1]) {
                        low++;
                    }

                    while (high - 1 >= 0 && num[high] == num[high - 1]) {
                        high--;
                    }

                    low++;
                    high--;
                } else if (a + b + c > target) {
                    while (high - 1 >= 0 && num[high] == num[high - 1]) {
                        high--;
                    }

                    high--;
                } else {
                    while (low + 1 < n && num[low] == num[low + 1]) {
                        low++;
                    }

                    low++;
                }
            }
        }

        return result;
    }
};
int main(int argc, char** argv)
{
    vector<int> S = {1, 0, -1, 0, -2, 2};
    Solution solution;
    auto result = solution.fourSum(S, 0);

    for (auto v : result) {
        for_each(v.begin(), v.end(), [ = ](int i) {
            printf("%d ", i);
        });
        printf("\n");
    }
}
