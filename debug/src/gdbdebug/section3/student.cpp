#include "student.h"
#include <iostream>
using namespace std;
student::student()
{
}
student::~student()
{
}
void student::work()
{
	int test = 100;
	const char* work="study";
	cout << "do work in student ,test is " << test << ",work is " << work << endl;
}
