#include<iostream>
using namespace std;
int main()
{
    int a[5];
    int i, s = 0;
    a[0] = a[1] = a[3] = a[4] = 0;

    for (i = 0; i < 5; i++) {
        s = s + a[i];
    }

    if (s == 33) {
        cout << "sum is 33" << endl;
    } else {
        cout << "sum is not 33" << endl;
    }

    return 0;
}
