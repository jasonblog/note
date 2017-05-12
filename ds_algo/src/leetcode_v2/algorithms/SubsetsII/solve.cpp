#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
class Solution
{
public:
    vector<vector<int>> subsets(vector<int>& nums)
    {
        return subsets_iterative(nums);
    }
private:
    vector<vector<int>> subsets_iterative(vector<int>& nums)
    {
        vector<vector<int>> result(1, vector<int>());
        sort(begin(nums), end(nums));
        int n = nums.size();
        int m = 0;

        for (int i = 0; i < n; ++i) {
            int j = i > 0 && nums[i] == nums[i - 1] ? m : 0;
            m = result.size();

            for (; j < m; ++j) {
                result.push_back(result[j]);
                result.back().push_back(nums[i]);
            }
        }

        return result;
    }
};
void print(vector<vector<int>> nums)
{
    for (auto v : nums) {
        cout << "[";
        for_each(begin(v), end(v), [](int i) {
            cout << i << ",";
        });
        cout << "]";
        cout << endl;
    }
}
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> nums = {1, 2, 2};
    print(solution.subsets(nums));
    return 0;
}
