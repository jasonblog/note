#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void print(const vector<int>& nums)
{
    for (auto i : nums) {
        cout << i << " ";
    }

    cout << endl;
}
class Solution
{
public:
    int findKthLargest(vector<int>& nums, int k)
    {
        int s = 0, t = nums.size() - 1;

        while (true) {
            int pos = partition(nums, s, t);

            if (pos == k - 1) {
                return nums[pos];
            }

            if (pos > k - 1) {
                t = pos - 1;
            } else {
                s = pos + 1;
            }
        }

        return -1;
    }
private:
    int partition(vector<int>& nums, int s, int t)
    {
        int i = s, j = t;
        int k = nums[s];

        while (i < j) {
            while (nums[j] <= k && i < j) {
                --j;
            }

            nums[i] = nums[j];

            while (nums[i] >= k && i < j) {
                ++i;
            }

            nums[j] = nums[i];
        }

        nums[i] = k;
        return i;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> nums = {1, 2, 1, 3, 5, 5};
    print(nums);
    int result = solution.findKthLargest(nums, 2);
    printf("result = %d\n", result);
    return 0;
}
