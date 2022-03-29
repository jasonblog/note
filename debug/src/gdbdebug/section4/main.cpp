#include <iostream>
#include "student.h"
#include "teacher.h"
#include <cstring>
using namespace std;
struct test_struct
{
	char name[100];
	int age;
	int gender;
};
int test_work(const char*name,int age)
{
       if(age <=20)
       {
	       cout << "Yong guy:" << name << endl;
       } 
       else if(age >=50)
       {
	       cout << "Old guy:" << name << endl;
       }
       else
       {
	       cout << "Middle guy:" << name << endl;
       }
	cout << "age is " << age << endl;
        return 0;
}

int main(int argc,char** argv)
{
	int i=0;
	int j=i++;
	int k=++i;
	
	cout << i << j << k << endl;
	char name[100]={0};
	strcpy(name,"SimpleSoft");
	test_work(name,25);

	struct test_struct test;
	strcpy(test.name,name);
	test.age = 20;
	test.gender = 1;

	std::string str[]={"this","is","a","test"};

	int iarr[10]={0};
	for(int i=0;i<10;i++)
	{
		iarr[i]=i;
	}
	for(int i=0;i<10;i++)
	{
		cout << iarr[i] << endl;
	}
	return 0;
}
