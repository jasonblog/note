#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;
class Solution
{
public:
    bool containsNearbyDuplicate(vector<int>& nums, int k)
    {
        unordered_map<int, int> map;
        int n = nums.size();

        for (int i = 0; i < n; ++i) {
            if (map.find(nums[i]) != map.end()) {
                int s = map[nums[i]];

                if (i - s <= k) {
                    return true;
                }
            }

            map[nums[i]] = i;
        }

        return false;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> nums = {2, 3, 4, 1};
    cout << solution.containsNearbyDuplicate(nums, 1) << endl;
    return 0;
}
