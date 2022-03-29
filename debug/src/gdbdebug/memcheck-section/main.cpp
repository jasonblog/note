#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;
void new_test()
{
	int *test = new int[80];
	test[0]=0;
}
void malloc_test()
{
	int *test =(int*) malloc(100);
	test[0]=0;
}
void heap_buffer_overflow_test()
{
	char *test = new char[10];
	const char* str = "this is a test string";
	strcpy(test,str);
	delete []test;

}
void stack_buffer_overflow_test()
{
	int test[10];
	test[1]=0;
	int a = test[13];
	cout << a << endl;

}
int global_data[100] = {0};
void global_buffer_overflow_test()
{
	int data = global_data[102];
	cout << data << endl;

}
void use_after_free_test()
{
	char *test = new char[10];
	strcpy(test,"this test");
	delete []test;
	char c = test[0];
	cout << c << endl;
}
int main()
{
	//new_test();
	//malloc_test();
	
	//heap_buffer_overflow_test();
	
	//stack_buffer_overflow_test();
	//global_buffer_overflow_test();
	use_after_free_test();
	
	return 0;
}
