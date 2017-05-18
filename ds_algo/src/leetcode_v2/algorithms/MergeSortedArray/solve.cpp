#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
using namespace std;
class Solution
{
public:
    void merge(vector<int>& num1, int m, vector<int>& num2, int n)
    {
        merge_from_end(num1, m, num2, n);
    }
    void merge(vector<int>& num1, vector<int>& num2)
    {
        merge(num1, num1.size(), num2, num2.size());
    }
private:
    void merge_from_start(vector<int>& num1, int m, vector<int>& num2, int n)
    {
        int i = 0, j = 0, k = 0;
        vector<int> result(m + n, 0);

        while (i < m && j < n) {
            if (num1[i] <= num2[j]) {
                result[k++] = num1[i++];
            } else {
                result[k++] = num2[j++];
            }
        }

        while (i < m) {
            result[k++] = num1[i++];
        }

        while (j < n) {
            result[k++] = num2[j++];
        }

        num1.clear();
        for_each(begin(result), end(result), [&num1](int i) {
            num1.push_back(i);
        });
    }
    void merge_from_end(vector<int>& num1, int m, vector<int>& num2, int n)
    {
        int k = m + n - 1;
        int i = m - 1, j = n - 1;

        while (i >= 0 && j >= 0) {
            if (num1[i] >= num2[j]) {
                num1[k--] = num1[i--];
            } else {
                num1[k--] = num2[j--];
            }
        }

        while (j >= 0) {
            num1[k--] = num2[j--];
        }
    }
};
void print(vector<int> v)
{
    if (v.empty()) {
        cout << "empty" << endl;
        return;
    }

    for_each(begin(v), end(v), [](int i) {
        cout << i << " ";
    });
    cout << endl;
}
int main(int argc, char** argv)
{
    Solution solution;
    int a[20];
    int n, m;

    while (scanf("%d%d", &m, &n) != EOF) {
        for (int i = 0; i < m; ++i) {
            scanf("%d", &a[i]);
        }

        vector<int> v1(a, a + m);

        for (int j = 0; j < n; ++j) {
            scanf("%d", &a[j]);
        }

        vector<int> v2(a, a + n);
        cout << "v1: ";
        print(v1);
        cout << "v2: ";
        print(v2);
        v1.resize(m + n);
        solution.merge(v1, m, v2, n);
        cout << "merged: ";
        print(v1);
    }

    return 0;
}
