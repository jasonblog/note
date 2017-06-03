#include <vector>
#include <iostream>
using namespace std;
int main()
{
    int a[7] = {1, 2, 3, 4, 5, 6, 7};
    vector<int> ivector(a, a +
                        7);/*vector的赋值并不可以像数组一样方便的用花括号方便  的完成赋值，这里借用了数组来初始化这个vector
   初始化方式vector<elementType> intvec(begin,end);这样可以用起来看上去还是比较习惯的。*/
    vector<int>::iterator iter;

    for (iter = ivector.begin(); iter != ivector.end(); iter++) {
        cout << *iter << " ";
    }

    cout << endl;
    ivector[5] = 1;
    /*单个vector的赋值，这个方式看上去还是和数组一样的
    也可以这么写ivector.at(5)=1;但是就是不习惯  */
    cout << ivector[5] << endl << ivector.size() << endl;

    for (iter = ivector.begin(); iter != ivector.end(); iter++) {
        cout << *iter << " ";
    }

    cout << endl;

    for (int i = 0; i < 5; i++) {
        cout << ivector[i] << " ";
    }

    cout << endl;
    return 0;
}
