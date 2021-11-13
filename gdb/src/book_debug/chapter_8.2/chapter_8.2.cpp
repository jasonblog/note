#include <iostream>
#include <string.h>
using namespace std;

void crash_test()
{
	char *str =0;
	strcpy(str,"test");
}
int main()
{
	cout << "crash test" << endl;
	crash_test();
	return 0;

}
