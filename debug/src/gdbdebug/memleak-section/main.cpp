#include <malloc.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
using namespace std;

void test_malloc_leak(int size)
{
	malloc(1024);
}
void no_leak()
{
	void *p=malloc(1024*1024*10);
	free(p);
}
int main(int argc,char* argv[])
{
	no_leak();
	test_malloc_leak(1024);
	return 0;
}

