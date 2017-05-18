#include <string>
#include <cstdio>
#include <iostream>
using namespace std;
class Solution
{
public:
    string addBinary(string a, string b)
    {
        int i = a.size() - 1;
        int j = b.size() - 1;
        string result;
        char carray = '0';

        while (i >= 0 && j >= 0) {
            char c = add(a[i], b[j], carray);

            switch (c) {
            case '0':
            case '1':
                carray = '0';
                result.push_back(c);
                break;

            case '2':
                carray = '1';
                result.push_back('0');
                break;

            case '3':
                carray = '1';
                result.push_back('1');
                break;
            }

            i--, j--;
        }

        while (i >= 0) {
            char c = add(a[i], carray);

            if (c == '1' || c == '0') {
                result.push_back(c);
                carray = '0';
            } else {
                result.push_back('0');
                carray = '1';
            }

            i--;
        }

        while (j >= 0) {
            char c = add(b[j], carray);

            if (c == '1' || c == '0') {
                result.push_back(c);
                carray = '0';
            } else {
                result.push_back('0');
                carray = '1';
            }

            j--;
        }

        if (carray == '1') { // 最后一位进位不要丢了
            result.push_back('1');
        }

        return string(result.rbegin(), result.rend());
    }
private:
    char add(char a, char b)
    {
        return a + b - '0';
    }
    char add(char a, char b, char c)
    {
        return a + b + c - '0' - '0';
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    char a[20], b[20];

    while (scanf("%s%s", a, b) != EOF) {
        cout << solution.addBinary(string(a), string(b)) << endl;
    }

    return 0;
}
