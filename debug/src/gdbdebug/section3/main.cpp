#include <iostream>
#include "student.h"
#include "teacher.h"
using namespace std;
int dummy()
{
	return 10;
}
int sum(int num)
{
	int sum=0;
	for(int i=1;i<=num;i++)
	{
		sum += i;
	}
	return sum;
}
int test_work(int i,int j)
{
	int x = i*j;

	cout << "work test i is " << i <<",j is " << j << ",x is " << x << endl;
	return i+j;
}
int testfun(int i,int j)
{
	cout << "testfun(int,int)" << endl;
	return i+j;
}
float testfun(float i,float j)
{
	cout << "testfun(float,float)" << endl;
	return i+j;
}
int main()
{
	test_work(100,200);
	int total=sum(100);
	cout << "sum(100) is " << total << endl;
	student stu;
	stu.work();
	person *teac = new teacher();
	teac->work();
	delete teac;
	int x=testfun(10,20);
	float y=testfun(10.0f,20.1f);
	cout << "x is " << x << ",y is " << y << endl;

	return 0;
}
