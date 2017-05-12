#include <vector>
#include <climits>
#include <cstdio>
#include <cstdlib>
using namespace std;
class Solution
{
public:
    int findMin(vector<int>& nums)
    {
        int n = nums.size();

        if (n == 0) {
            return INT_MIN;
        }

        int s = 0, t = n - 1;

        while (s < t) {
            if (nums[s] < nums[t]) {
                return nums[s];
            }

            int mid = (s + t) >> 1;

            if (nums[mid] > nums[t]) {
                s = mid + 1;
            } else if (nums[mid] < nums[t]) {
                t = mid;
            } else {
                t--;
            }
        }

        return nums[s];
    }
};
int main(int argc, char** argv)
{
    int a[20];
    int n;
    Solution solution;

    while (scanf("%d", &n) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", a + i);
        }

        vector<int> v(a, a + n);
        printf("%d\n", solution.findMin(v));
    }

    return 0;
}
