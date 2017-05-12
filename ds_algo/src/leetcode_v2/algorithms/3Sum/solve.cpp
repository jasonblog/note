#include <vector>
#include <map>
#include <set>
#include <cstdio>
#include <algorithm>
using namespace std;
class Solution
{
public:
    vector<vector<int>> threeSum(vector<int>& num)
    {
        sort(num.begin(), num.end());
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

                if (a + b + c == 0) {
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
                } else if (a + b + c > 0) {
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
    //  vector<int> num1 = {-1, 0,1,2,-1,-4};
    vector<int> num2 = {0, 0, 0};
    Solution solution;
    //  auto result1 = solution.threeSum(num1);
    auto result2 = solution.threeSum(num2);
    //  printf("****************");
    //  for (auto v : result1) {
    //      for_each(v.begin(), v.end(), [=](int i){printf("%d ", i);});
    //      printf("\n");
    //  }
    printf("****************");

    for (auto v : result2) {
        for_each(v.begin(), v.end(), [ = ](int i) {
            printf("%d ", i);
        });
        printf("\n");
    }
}
