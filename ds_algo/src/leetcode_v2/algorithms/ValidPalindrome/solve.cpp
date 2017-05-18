#include <string>
#include <iostream>
#include <cstdio>
#include <cctype>
using namespace std;
class Solution
{
public:
    bool isPalindrome(string s)
    {
        int n = s.size();

        if (n == 0) {
            return true;
        }

        int left = next(s, -1);
        int right = prev(s, n);

        for (int i = next(s, -1), j = prev(s, n); i < j;
             i = next(s, i), j = prev(s, j)) {
            if (!equals(s[i], s[j])) {
                return false;
            }
        }

        return true;
    }
private:
    int next(const string& s, int i)
    {
        i++;

        while (!isalnum(s[i])) {
            i++;
        }

        return i;
    }
    int prev(const string& s, int j)
    {
        j--;

        while (!isalnum(s[j])) {
            j--;
        }

        return j;
    }
    bool equals(char a, char b)
    {
        a = tolower(a);
        b = tolower(b);
        return a == b;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    string s1 = "A man, a plan, a canal: Panama";
    string s2 = "race a car";
    string s3 = "1a2";
    cout << solution.isPalindrome(s1) << endl;
    cout << solution.isPalindrome(s2) << endl;
    cout << solution.isPalindrome(s3) << endl;
    return 0;
}
