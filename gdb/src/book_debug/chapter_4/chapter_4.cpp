
#include <thread>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
class test_c
{
public:
	test_c()
	{
	}
	virtual ~test_c()
	{
	}

public:
    static void do_work_1()
    {
	    cout << "do work 1" <<endl;

    }
    void do_work_2()
    {
	    cout << "do work 2" <<endl;
    }
    void do_work_3(void *arg)
    {
	    cout << "do work 3" << endl;
    }
    int do_work_4(void * arg,int i)
    {
	    cout << "do work 4,i is" << i <<endl;
	    return 0;
    }
};

void do_work_5(void *arg)
{
	cout << "do work 5" << endl;
}
int main()
{

	thread t1(&test_c::do_work_1);
	test_c test2;
	
	thread t2(&test_c::do_work_2,test2);
	
	test_c test3;
	thread t3(&test_c::do_work_3,test3,(void*)"test3");
	
	test_c test4;
	thread t4(&test_c::do_work_4,test4,(void*)"test4",4);
	thread t5(&do_work_5,(void*)"test5");
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	return 0;
}
