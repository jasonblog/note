#include<iostream>
using namespace std;
int func(int n)
{
    int result = 0;

    for (int i = 1; i <= n; i++) {
        result += i;
    }

    return result;
}
int main()
{
    int arr[10];
    arr[0] = 0;
    arr[1] = 1;

    for (int i = 2; i < 10; i++) {
        arr[i] = arr[i - 1] + arr[i - 2];
    }

    cout << "arr[9]" << arr[9] << endl;
    cout << "func(9)" << func(9) << endl;
    return 0;
}
