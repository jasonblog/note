#include <iostream>
#include <cstring>
#include <thread>
using namespace std;
class test
{
public:
	test(){}
	virtual ~test(){}
public:
	static void do_work_1()
	{
		cout << "do work 1" << endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		cout << "thread exited" << endl;
	}
	void do_work_2()
	{
		cout << "do work 2" << endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		cout << "thread exited" << endl;
	}
	void do_work_3(void *arg,int x,int y)
	{
		char *data = (char*)arg;
		cout << "do work 3:" << data << ",x=" << x << ",y=" << y << endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		cout << "thread exited" << endl;
	}
};
void test_thread(void *data)
{
	int *val = (int*)data;
	cout << "thread data:" << *val << endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	cout << "thread exited" << endl;
}
int main(int argc,char** argv)
{
	int data=10;
	thread t1(&test_thread,(void*)&data);

	thread t2(&test::do_work_1);	
	
	test test3;
	thread t3(&test::do_work_2,test3);
	
	test test4;
	thread t4(&test::do_work_3,test4,(void*)"test",10,20);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	cout << "threads exit" << endl;
	return 0;
}
