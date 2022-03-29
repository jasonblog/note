
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <cstring>

using namespace std;
void test_core(const char* name,int size)
{
	char test[15]="test string";
	memcpy(test,name,size);
	cout << "name=" << test << endl;
}
int main(int argc,char** argv)
{
	const char *badstr=0;
	const char *goodstr="SimpleSoft";
	if(argc > 1)
	{
		test_core(badstr,11);
	}
	else
	{
		test_core(goodstr,11);
	}
	return 0;
}

