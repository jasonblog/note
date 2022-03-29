#include <iostream>
#include <cstring>
using namespace std;
int test(int x,int y)
{	
	cout << "x=" << x << "y=" << y << endl;
	return x + y;
}
void fun(int x)
{
	cout << "x=" << x << endl;
}
int test_work(const char*name,int age)
{
	cout << "name=" << name << ",age=" << age << endl;
	return 0;
}

int main(int argc,char** argv)
{
	int age = 25;
	char name[100]={0};
	strcpy(name,"SimpleSoft");
	test_work(name,age);
	memset(name,0,sizeof(name));
	age = test(10,20);
	return 0;
}
