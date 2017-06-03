#include<iostream>
#include<stdlib.h>
using namespace std;
#define max_size 10
//比较两个数的大小
//如果比较函数返回大于0，qsort就认为a>b
//如果比较函数返回等于0，qsort就认为a=b
//如果比较函数返回小于0，qsort就认为a<b
int cmp(const void* a, const void* b)
{
    return *(int*)a - *(int*)b;
}
int main()
{
    int arr[max_size];
    int n = 0;

    //从标准输入设备中读入整数，同时累计输入个数，直到输入的是非整型数据为止
    for (;; n++) {
        cin >> arr[n];
    }

    qsort(arr, n, sizeof(int), cmp);

    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }

    return 0;
}
