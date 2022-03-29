#include <iostream>
#include <cstring>
using namespace std;
struct test_struct
{
	char name[12];
	char gender;
	int age;
};
int test_fun(const char* name,int age,int gender)
{
	test_struct test;
	memset(&test,0,sizeof(test));
	strcpy(test.name,name);
	test.age = age;
	test.gender = gender;
	return 0;
}

int main(int argc,char** argv)
{
	test_fun("SimpleSoft",25,'m');

	return 0;
}
