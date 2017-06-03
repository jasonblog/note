#include<iostream>
using namespace std;
template<typename T>
T min(T a, T b, T c)
{
    if (a > b) {
        a = b;
    }

    if (a > c) {
        a = c;
    }

    return a;
}
int main()
{
    int a = 1, b = 2, c = 3;
    cout << min(a, b, c) << endl;
    long long a1 = 1000000000, b1 = 2000000000, c1 = 3000000000;
    cout << min(a1, b1, c1) << endl;
    return 0;
}
