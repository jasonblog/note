
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
using namespace std;

mutex _mutex1;
mutex _mutex2;
int data1;
int data2;
int do_work_1()
{
	std::cout << "线程函数do_work_1开始" << std::endl;
	lock_guard<mutex> locker1(_mutex1);
	//模拟做一些事情
	data1++;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	lock_guard<mutex> locker2(_mutex2);
	data2++;
	std::cout << "线程函数do_work_1结束" << std::endl;
	return 0;
}
int do_work_2()
{
	std::cout << "线程函数do_work_2开始" << std::endl;
	lock_guard<mutex> locker2(_mutex2);
	//模拟做一些事情		
	data2++;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	lock_guard<mutex> locker1(_mutex1);

	data1++;
	std::cout << "线程函数do_work_2结束" << std::endl;
	return 0;
}
int main()
{
	thread t1(do_work_1);
	thread t2(do_work_2);
	t1.join();
	t2.join();
	cout << "线程运行结束" << endl;
	return 0;
}

