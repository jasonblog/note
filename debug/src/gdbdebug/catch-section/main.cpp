#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <thread>
using namespace std;
int test_fun(int num)
{
     		
	if(num ==0)
		throw "num is 0";
	int x = 100 / num;
	cout << x << endl;
	return x;
}
void test_file()
{
	int fd = open("main.d",O_RDWR);
	if(fd != 0)
	{
		close(fd);
	}
}

int main(int argc,char** argv)
{
	test_file();
	try
	{
		test_fun(0);
	}
	catch(const char* error_msg)
	{
		cout << error_msg << endl;
	}
	test_file();
	return 0;
}
