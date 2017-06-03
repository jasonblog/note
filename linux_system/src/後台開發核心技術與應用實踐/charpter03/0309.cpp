#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

void print(int n)
{
    cout << n << " ";
}

int main()
{
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    vector<int> ivector(a, a + 7);
    vector<int>::iterator iter;
    for_each(ivector.begin(), ivector.end(), print); //用for_each进行遍历
    cout << endl;
    ivector[5] = 1;
    cout << ivector[5] << endl << ivector.size() << endl;
    for_each(ivector.begin(), ivector.end(), print); //用for_each进行遍历
    return 0;
}
