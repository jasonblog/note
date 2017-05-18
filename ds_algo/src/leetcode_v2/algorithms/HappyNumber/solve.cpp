#include <cstdio>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;
class Solution
{
public:
    bool isHappy(int n)
    {
        return isHappy1(n);
    }
private:
    bool isHappy1(int n)
    {
        unordered_set<int> s;

        if (n == 0) {
            return false;
        }

        while (n != 1) {
            s.insert(n);
            n = next(n);

            if (s.find(n) != s.end()) {
                return false;
            }
        }

        return true;
    }
    bool isHappy2(int n)
    {
        int slow = next(n);
        int fast = next(next(n));

        while (slow != 1 && slow != fast) {
            slow = next(slow);
            fast = next(next(fast));
        }

        return slow == 1;
    }
    int next(int n)
    {
        int ans = 0;

        while (n) {
            int mod = (n % 10);
            ans += (mod * mod);
            n /= 10;
        }

        return ans;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (scanf("%d", &n) != EOF) {
        printf("%d : %d\n", n, solution.isHappy(n));
    }

    return 0;
}
