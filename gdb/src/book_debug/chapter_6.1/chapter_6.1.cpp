#include <iostream>
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define   malloc(s)  _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif
class test_c
{
public:
	test_c()
	{
		data = new char[10];
	}
	virtual ~test_c()
	{
		delete[]data;
	}
private:
	char* data;
};
void new_test()
{
	int* test1 = new int[100];
	int* test2 = new int[16];
	test_c* test = new test_c();
	memset(test2, 0, 16);
}
void malloc_test()
{
	test_c* test = new test_c();
	char* test1 = (char*)malloc(100);
	char* test2 = (char*)malloc(6);
	const char* str = "this is a test";
	memcpy(test2, str, strlen(str));
	char* test3 = new char[10];
	delete[]test3;
}
int main()
{
	 //启用调试对管理
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
	std::cout << "Memory leak test" << std::endl;
	////_CrtSetBreakAlloc(168);
	malloc_test();
	//new_test();
	//_ASSERT(_CrtCheckMemory());
	//_CrtDumpMemoryLeaks();	 //显示内存泄漏报告

	return 0;
}
