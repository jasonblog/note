#include <vector>
#include <climits>
#include <cstdlib>
#include <cstdio>
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))
using namespace std;
class Solution
{
public:
    int maxProduct(vector<int>& nums)
    {
        const size_t size = nums.size();

        if (size < 1) {
            return 0;
        }

        if (size == 1) {
            return nums[0];
        }

        int neg = 0, pos = 0, max = 0;

        for (auto i = 0; i < size; ++i) {
            if (nums[i] == 0) {
                pos = neg = 0;
            } else if (nums[i] > 0) {
                pos = MAX(pos * nums[i], nums[i]);
                neg = MIN(neg * nums[i], neg);
            } else {
                int old_pos = pos;
                int old_neg = neg;
                pos = MAX(old_neg * nums[i], nums[i]);
                neg = MIN(old_pos * nums[i], nums[i]);
            }

            max = MAX(max, pos);
        }

        return max;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    int a[20];
    int n;

    while (scanf("%d", &n) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", a + i);
        }

        vector<int> v(a, a + n);
        printf("%d\n", solution.maxProduct(v));
    }
}
