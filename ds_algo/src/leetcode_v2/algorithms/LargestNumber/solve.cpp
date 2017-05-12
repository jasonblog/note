#include <vector>
#include <string>
#include <algorithm>
using namespace std;
class Solution
{
public:
    static bool cmp(const string& s1, const string& s2)
    {
        return s1 + s2 > s2 + s1;
    }
    string largestNumber(vector<int>& num)
    {
        vector<string> v;
        for_each(num.begin(), num.end(), [&](int i) {
            v.push_back(to_string(i));
        });
        sort(v.begin(), v.end(), cmp);
        string result;
        for_each(v.begin(), v.end(), [&](string s) {
            result += s;
        });

        if (result[0] == '0') {
            return "0";
        }

        return result;
    }

};
int main(int argc, char** argv)
{

    return 0;
}
