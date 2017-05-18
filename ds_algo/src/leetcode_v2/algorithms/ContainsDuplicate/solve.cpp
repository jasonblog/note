#include <vector>
#include <unordered_set>
#include <string>
#include <iostream>
using namespace std;
class Solution
{
public:
    bool containsDuplicate(vector<int>& nums)
    {
        unordered_set<int> set;

        for (int i : nums) {
            if (set.find(i) != set.end()) {
                return true;
            } else {
                set.insert(i);
            }
        }

        return false;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> nums = {2, 3, 4, 1};
    cout << solution.containsDuplicate(nums) << endl;
    return 0;
}
