#include <string>
#include <cstdio>
#include <cstdlib>
using namespace std;
class Solution
{
public:
    string intToRoman(int num)
    {
        string result;

        for (int i = 0; num; i++, num /= 10) {
            if (num % 10) {
                result = ROMAN_TABLE[i][num % 10 - 1] + result;
            }
        }

        return result;
    }
private:
    const string ROMAN_TABLE[4][9] = {
        {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"},
        {"X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"},
        {"C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"},
        {"M", "MM", "MMM",  "", "", "", "", "", ""}
    };
};
int main(int argc, char** argv)
{
    Solution solution;
    int n;

    while (scanf("%d", &n) != EOF) {
        printf("%d -> %s\n", n, solution.intToRoman(n).c_str());
    }

    return 0;
}
