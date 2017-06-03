#include<iostream>
using namespace std;
typedef union {
    long i;
    int k[5];
    char c;
} UDATE;
struct data {
    int cat;
    UDATE cow;
    double dog;
} too;
UDATE temp;
int main()
{
    cout << sizeof(struct data) + sizeof(temp) << endl;
    return 0;
}
