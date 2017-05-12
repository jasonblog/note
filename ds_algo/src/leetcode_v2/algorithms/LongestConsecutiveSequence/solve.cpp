#include <unordered_map>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <algorithm>
using namespace std;
class Solution
{
public:
    int longestConsecutive(vector<int>& nums)
    {
        unordered_map<int, int> map;
        int res = 0;

        for (int i : nums) {
            if (map.find(i) != map.end()) {
                continue;
            }

            int left = getOrElse(map, i - 1);
            int right = getOrElse(map, i + 1);
            int sum = left + right + 1;
            res = max(res, sum);
            map[i] = sum;
            map[i - left] = sum;
            map[i + right] = sum;
        }

        return res;
    }
private:
    int getOrElse(unordered_map<int, int>& map, int key, int defaultValue)
    {
        if (map.find(key) != map.end()) {
            return map[key];
        } else {
            return defaultValue;
        }
    }
    int getOrElse(unordered_map<int, int>& map, int key)
    {
        return getOrElse(map, key, 0);
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (scanf("%d", &n) != EOF) {
        vector<int> nums;

        for (int i = 0; i < n; ++i) {
            int v;
            scanf("%d", &v);
            nums.push_back(v);
        }

        printf("%d\n", solution.longestConsecutive(nums));
        break;
    }

    return 0;
}
