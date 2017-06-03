#include<iostream>
using namespace std;
int main()
{
    int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; //数组a,类型为int整型，有10个元素，是一个一维数组
    int i, j;

    for (i = 0; i < 10; i++) { //从a[0]~a[9]
        cout << a[i] << " ";
    }

    cout << endl;
    int b[2][2] = {1, 2, 3, 4}; //数组b，类型为int，有4 元素，是一个二维数组

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            cout << b[i][j] << " ";
        }

        cout << endl;
    }

    return 0;
}
