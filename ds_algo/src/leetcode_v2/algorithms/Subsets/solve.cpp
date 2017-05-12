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
    vector<vector<int>> subsets_recurisive(vector<int>& nums)
    {
        vector<vector<int>> result;
        vector<int> cur;
        sort(nums.begin(), nums.end());
        subsets_recurisive(result, cur, nums, 0);
        return result;
    }
    void subsets_recurisive(vector<vector<int>>& result, vector<int> cur,
                            const vector<int>& nums, int i)
    {
        if (i == nums.size()) {
            result.push_back(cur);
            return;
        }

        subsets_recurisive(result, cur, nums, i + 1);
        cur.push_back(nums[i]);
        subsets_recurisive(result, cur, nums, i + 1);
    }
    vector<vector<int>> subsets_iterative(vector<int>& nums)
    {
        vector<vector<int>> result(1, vector<int>());
        sort(begin(nums), end(nums));
        int n = nums.size();

        for (int i = 0; i < n; ++i) {
            int m = result.size();

            for (int j = 0; j < m; ++j) {
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
    vector<int> nums = {4, 1, 0};
    print(solution.subsets(nums));
    return 0;
}
