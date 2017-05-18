#include <string>
#include <iostream>
#include <algorithm>
#include <cstdio>
using namespace std;
class Solution
{
public:
    string multiply(string num1, string num2)
    {
        if (num1 == "0" || num1 == ""
            || num2 == "0" || num2 == "") {
            return "0";
        }

        /*
         * 反而更慢？
        if (num1.size() < num2.size())
            return multiply(num2, num1);
        */
        string result;
        int n = num2.size();

        for (int i = n - 1; i >= 0; --i) {
            if (num2[i] == '0') {
                continue;
            }

            string s = multiply(num1, num2[i] - '0');
            string zeros;

            for (int j = 1; j < n - i; ++j) {
                zeros.push_back('0');
            }

            s = zeros + s;
            result = add(result, s);
        }

        reverse(begin(result), end(result));
        return result == "" ? "0" : result;
    }
private:
    string multiply(string num1, int i)
    {
        if (i == 0) {
            return "0";
        }

        if (i == 1) {
            reverse(begin(num1), end(num1));
            return num1;
        }

        string result;
        int n = num1.size();
        int j = n - 1;
        int carry = 0;

        for (int j = n - 1; j >= 0; --j) {
            int c = (num1[j] - '0') * i + carry;
            carry = c / 10;
            c %= 10;
            result.push_back(c + '0');
        }

        if (carry != 0) {
            result.push_back(carry + '0');
        }

        //reverse(begin(result), end(result));
        return result;
    }
    string add(string num1, string num2)
    {
        string result;
        int i = 0, j = 0;
        int carry = 0;

        while (i < num1.size() && j < num1.size()) {
            int a = num1[i] - '0';
            int b = num2[j] - '0';
            int c = a + b + carry;
            carry = c / 10;
            c %= 10;
            result.push_back(c + '0');
            i++, j++;
        }

        while (i < num1.size()) {
            int c = num1[i] - '0' + carry;
            carry = c / 10;
            c %= 10;
            result.push_back(c + '0');
            i++;
        }

        while (j < num2.size()) {

            int c = num2[j] - '0' + carry;
            carry = c / 10;
            c %= 10;
            result.push_back(c + '0');
            j++;
        }

        if (carry != 0) {
            result.push_back(carry + '0');
        }

        return result;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    string s1, s2;

    while (cin >> s1 >> s2) {
        cout << s1 << " * " << s2 << " = " << solution.multiply(s1, s2) << endl;
    }

    return 0;
}
