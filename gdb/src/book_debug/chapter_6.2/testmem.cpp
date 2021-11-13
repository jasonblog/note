#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;
void new_test()
{
	int *test = new int[80];
}
void malloc_test()
{
	int *test =(int*) malloc(100);
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
	int a = test[12];

}
int global_data[100] = {0};
void global_buffer_overflow_test()
{
	int data = global_data[101];


}
void use_after_free_test()
{
	char *test = new char[10];
	strcpy(test,"this test");
	delete []test;
	char c = test[0];
}
int main()
{
	cout << "memory test" << endl;
	//use_after_free_test();
	//global_buffer_overflow_test();
	//栈溢出测试
	//stack_buffer_overflow_test();
	/*溢出测试*/
	//heap_buffer_overflow_test();
	//*内存泄漏测试
	malloc_test();
	cout << "malloc test end" << endl;
	//new_test();
	cout << "new test end" << endl;
	//*/
	return 0;
}
