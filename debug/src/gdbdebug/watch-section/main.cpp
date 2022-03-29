#include <iostream>
#include <cstring>
#include <thread>
using namespace std;
int gdata = 0;
int gdata2 = 0;
void test_thread(void *data)
{
	int *temp = (int*)data;
	std::this_thread::sleep_for(std::chrono::seconds(*temp));
	gdata = *temp;
	gdata2 = 2 * (*temp);
	cout << "thread data:" << gdata << endl;
	cout << "test thread exited" << endl;
}
int main(int argc,char** argv)
{
	int data=3;
	thread t1(&test_thread,(void*)&data);
	int data2=5;
	thread t2(&test_thread,(void*)&data2);
	t1.join();
	t2.join();
	cout << "threads exit" << endl;
	return 0;
}
