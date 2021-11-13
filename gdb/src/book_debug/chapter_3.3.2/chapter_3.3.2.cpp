#include <malloc.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
	
void test_throw(int number)
{
	int local_data = number;
	const char* name = "test throw";
	printf("name is %s,%d\n",name,local_data);
	throw "test exception";
}
void test_try_catch(int number)
{
	int local_data = number;
	const char* name = "test_try_catch";
	printf("name is %s,%d\n",name,local_data);
	try
	{
		throw 10;
	}
	catch(...)
	{
		printf("catch ...\n");
	}

}
int main(int argc,char* argv[])
{
	execl("ls","-l");
	test_throw(10);
	//test_try_catch(20);
	return 0;
}
