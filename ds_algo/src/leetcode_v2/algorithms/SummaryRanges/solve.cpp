#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdio>
using namespace std;
class Solution
{
public:
    vector<string> summaryRanges(vector<int>& nums)
    {
        vector<string> result;
        int n = nums.size();

        if (n == 0) {
            return result;
        }

        int s = nums[0], t;
        bool update = false;
        int i;

        for (i = 1; i < n; ++i) {
            if (nums[i] == nums[i - 1] + 1) {
                update = true;
                t = nums[i];
            } else {
                if (update) {
                    result.push_back(to_string(s) + "->" + to_string(t));
                    s = nums[i];
                    update = false;
                } else {
                    result.push_back(to_string(s));
                    s = nums[i];
                }
            }
        }

        if (update) {
            result.push_back(to_string(s) + "->" + to_string(nums[n - 1]));
        } else {
            result.push_back(to_string(nums[n - 1]));
        }

        return result;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> nums = {0, 5, 9};
    auto result = solution.summaryRanges(nums);
    for_each(begin(result), end(result), [](string s) {
        cout << s << endl;
    });
    return 0;
}
