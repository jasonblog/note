#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
using namespace std;
void print(vector<int> v)
{
    for (auto i : v) {
        printf("%d ", i);
    }

    printf("\n");
}
class Solution
{
public:
    vector<vector<int> > permute(vector<int>& num)
    {
        vector<vector<int>> ans;
        sort(num.begin(), num.end());

        do {
            vector<int> v(num.begin(), num.end());
            ans.push_back(v);
        } while (next(num));

        return ans;
    }
private:
    bool next(vector<int>& v)
    {
        size_t n = v.size();
        int i;

        for (i = n - 2; i >= 0 && v[i] >= v[i + 1]; --i);

        if (i < 0) {
            reverse(v.begin(), v.end());
            return false;
        }

        int j;

        for (j = n - 1; j > i && v[j] <= v[i]; --j);

        swap(v[i], v[j]);
        reverse(v.begin() + i + 1, v.end());
        return true;
    }

};
int main(int argc, char** argv)
{
    Solution solution;
    vector<int> num = {1, 2, 3, 4, 5};
    vector<vector<int>> ans = solution.permute(num);

    for (auto i : ans) {
        print(i);
    }

    return 0;
}
