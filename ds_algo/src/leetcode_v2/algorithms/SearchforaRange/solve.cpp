#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdio>
using namespace std;
class Solution
{
public:
    vector<int> searchRange(int a[], int n, int target)
    {
        vector<int> result(2, -1);

        if (a == nullptr || n == 0) {
            return result;
        }

        int left = 0, right = n - 1;
        bool found = false;
        int mid = 0;

        while (left <= right) {
            mid = left + ((right - left) >> 1);

            if (a[mid] == target) {
                found = true;
                break;
            } else if (a[mid] < target) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        if (found) {
            result[0] = leftSearch(a, left, mid, target);
            result [1] = rightSearch(a, mid, right, target);
        }

        return result;
    }
private:
    int leftSearch(int a[], int start, int end, int target)
    {
        int left = start, right = end;

        while (left < right && a[right - 1] == target) {
            if (a[left] == target) {
                return left;
            }

            int mid = left + ((right - left) >> 1);

            if (a[mid] == target) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }

        return right;
    }
    int rightSearch(int a[], int start, int end, int target)
    {
        int left = start, right = end;

        while (left < right && a[left + 1] == target) {
            if (a[right] == target) {
                return right;
            }

            int mid = left + ((right - left) >> 1);

            if (a[mid] == target) {
                left = mid;
            } else {
                right = mid - 1;
            }
        }

        return left;
    }
};
int main(int argc, char** argv)
{
    int a[100], n, key;
    Solution solution;

    while (scanf("%d%d", &n, &key) != EOF) {
        for (int i = 0; i < n; ++i) {
            scanf("%d", a + i);
        }

        auto result = solution.searchRange(a, n, key);
        printf("[%d, %d]\n", result[0], result[1]);
    }

    return 0;
}
