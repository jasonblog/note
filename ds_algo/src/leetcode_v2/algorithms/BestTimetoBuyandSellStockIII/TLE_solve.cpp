#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
using namespace std;
class Solution
{
public:
    int maxProfit(vector<int>& prices)
    {
        int n = prices.size();

        if (n <= 1) {
            return 0;
        }

        int profit = 0;

        for (int i = 0; i < prices.size(); ++i) {
            int p = maxProfit(prices, 0, i) + maxProfit(prices, i, n - 1);
            profit = max(profit, p);
        }

        return profit;
    }
private:
    int maxProfit(vector<int>& a, int s, int t)
    {
        if (s >= t) {
            return 0;
        }

        int profit = 0;
        int minPrice = a[s];

        for (int i = s + 1; i <= t; ++i) {
            minPrice = min(minPrice, a[i]);
            profit = max(profit, a[i] - minPrice);
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
