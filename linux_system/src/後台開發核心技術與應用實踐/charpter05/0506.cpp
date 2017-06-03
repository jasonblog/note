#include<iostream>
using namespace std;
int main()
{
    for (int i = 0; i < 100; i++) {
        cout << "i:" << i << endl;
        sleep(5);
    }

    return 0;
}
