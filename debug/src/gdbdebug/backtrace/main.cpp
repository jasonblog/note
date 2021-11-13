#include <iostream>
#include <cstring>
#include "test.h"
using namespace std;

void call_test()
{
	test_child test;
        int zz = test.test_member(10,20);
        cout << "zz is " << zz << endl;
}
int main(int argc,char** argv)
{
	call_test();
	return 0;
}
