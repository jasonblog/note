#include <iostream>
#include <cstring>
using namespace std;
struct test_struct
{
	char name[12];
	char gender;
	int age;
};
int test_memory()
{
        const char* test_str="this is a test";
	int itest = 0x12345678;
	struct test_struct test;
	memset(&test,0,sizeof(test));
	test.age = 25;
	strcpy(test.name,"SimpleSoft");
	test.gender = 'm';
	cout << test_str << " " << itest << endl;
	return 0;
}

int main(int argc,char** argv)
{
	test_memory();

	return 0;
}
