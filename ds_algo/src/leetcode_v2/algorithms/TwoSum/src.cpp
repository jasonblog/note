#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
using namespace std;
class Solution
{
public:
    vector<int> twoSum(vector<int>& numbers, int target)
    {
        map<int, int> m;
        vector<int> result;

        for (size_t i = 0; i < numbers.size(); ++i) {
            if (m.find(numbers[i]) == m.end()) {
                m[target - numbers[i]] = i;
            } else {
                result.push_back(m[numbers[i]] + 1); // index from 1, not 0
                result.push_back(i + 1); // index from 1
            }
        }

        return result;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> numbers = {2, 7, 11, 15};
    auto result = solution.twoSum(numbers, 9);
    printf("%d\n%d\n", result[0], result[1]);
    return 0;
}
