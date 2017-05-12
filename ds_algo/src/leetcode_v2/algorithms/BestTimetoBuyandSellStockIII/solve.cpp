#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
using namespace std;
static inline void print(const vector<int>& v)
{
    for_each(v.begin(), v.end(), [](int i) {
        cout << i << " ";
    });
    cout << endl;
}
class Solution
{
public:
    int maxProfit(vector<int>& prices)
    {
        int n = prices.size();

        if (n <= 1) {
            return 0;
        }

        vector<int> leftMin(n, 0);
        leftMin[0] = prices[0];

        for (int i = 1; i < n; ++i) {
            leftMin[i] = min(leftMin[i - 1], prices[i]);
        }

        vector<int> rightMax(n, 0);
        rightMax[0] = prices[n - 1];

        for (int i = n - 2, k = 1; i >= 0; --i, k++) {
            rightMax[k] = max(rightMax[k - 1], prices[i]);
        }

        int left = 0, right = 0;
        int profit = 0;

        for (int i = 0; i < n; ++i) {
            left = max(left, prices[i] - leftMin[i]);
            right = rightMax[n - i - 1] - prices[i];
            profit = max(profit, left + right);
        }

        return profit;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> prices;
    int n;

    while (scanf("%d", &n) != EOF) {
        prices.clear();

        for (int i = 0; i < n; ++i) {
            int v;
            scanf("%d", &v);
            prices.push_back(v);
        }

        printf("maxProfit = %d\n", solution.maxProfit(prices));
    }

    return 0;

}
