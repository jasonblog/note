#include <vector>
#include <iostream>
#include <cstdio>
#include <algorithm>
using namespace std;
class Solution
{
public:
    int findPeakElement(vector<int>& a)
    {
        return fastFindPeakElement(a);
    }
private:
    int slowFindPeakElement(vector<int>& a)
    {
        int n = a.size();

        if (n < 2 || a[0] > a[1]) {
            return 0;
        }

        if (a[n - 1] > a[n - 2]) {
            return n - 1;
        }

        for (int i = 1; i < n - 1; ++i)
            if (a[i] > a[i - 1] && a[i] > a[i + 1]) {
                return i;
            }
    }
    int midFindPeakElement(vector<int>& a)
    {
        int n = a.size();

        for (int i = 0; i < n - 1; ++i)
            if (a[i] > a[i + 1]) {
                return i;
            }

        return n - 1;
    }
    int fastFindPeakElement(vector<int>& a)
    {
        int n = a.size();
        int left = 0, right = n - 1;

        while (left < right) {
            int mid = (left + right) >> 1;
            printf("left = %d, mid = %d, right = %d\n", left, mid, right);

            if (a[mid] > a[mid - 1] && a[mid] > a[mid + 1]) {
                return mid;
            }

            if (a[mid] > a[mid + 1]) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }

        return left;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    int a[20], n;

    while (scanf("%d", &n) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", a + i);
        }

        vector<int> v(a, a + n);
        cout << solution.findPeakElement(v) << endl;
    }

    return 0;
}
