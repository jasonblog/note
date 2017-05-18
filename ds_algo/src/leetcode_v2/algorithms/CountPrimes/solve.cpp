#include <cstdio>
#include <vector>
#include <algorithm>
using namespace std;
bool isPrime(int n)
{
    //asssert(n > 0);
    if (n <= 3) {
        return true;
    }

    for (int i = 2; i <= sqrt(n); ++i) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}
class Solution
{
public:
    int countPrimes(int n)
    {
        vector<bool> flags(n, true);
        flags[0] = false;
        flags[1] = false;
        int sqr = sqrt(n - 1);

        for (int i = 2; i <= sqr; ++i) {
            if (flags[i]) {
                for (int j = i * i; j < n; j += i) {
                    flags[j] = false;
                }
            }
        }

        return count(flags.begin(), flags.end(), true);
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (scanf("%d", &n) != EOF) {
        printf("Pi(%d) = %d\n", n, solution.countPrimes(n));
    }

    return 0;
}
