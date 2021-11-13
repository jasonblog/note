#include <malloc.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
using namespace std;
struct NODE
{
	int  ID;
	int age;
	char gender;
	NODE *next;
	int test;
	char c;
};
struct NODE2
{
	int ID;
	int age;
	NODE *next;	
	int test;
	char gender;
	char c;
};
class test_1
{
public:
	test_1(){x=10;y=100;}
	virtual ~test_1(){}
	virtual void test_fun()
	{
		printf("test_1 test_fun\n");
	}
private:
	int x;
	int *test;
	int y;
	
};
class test_2:public test_1
{
public:
	typedef unsigned long UL;
	
	test_2(){}
	virtual ~test_2(){}
	virtual void test_fun2()
	{
		printf("test_fun2\n");
	}
	virtual void test_fun()
	{
		printf("test_2 test_fun\n");
	}
private:
	UL member1;

};
int count = 0;

int main(int argc,char* argv[])
{
	test_1 test1;
	test_1 *test2 = new test_2();
	test_2 test3;
	struct NODE node;
	node.age = 100;
	struct NODE2 node2;
	node2.age = 20;
	delete test2;
	cout << "age=" << node.age << node2.age << endl;
	return 0;
}

