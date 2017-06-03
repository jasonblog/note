#include<iostream>
using namespace std;
#define _DEBUG_
int main()
{
    int x = 10;
#ifdef _DEBUG_
    cout << "File:" << __FILE__ << ",Line:" << __LINE__ << ",x:" << x << endl;
#else
    printf("x = %d\n", x);
    cout << x << endl;
#endif
    return 0;
}
