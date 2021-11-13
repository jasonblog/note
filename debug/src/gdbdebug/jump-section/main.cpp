#include <iostream>
#include <cstring>
using namespace std;
int test_work(const char*name,int age)
{
	cout << "name=" << name << ",age=" << age << endl;
	return 0;
}

int main(int argc,char** argv)
{
	int age = 25;
	char name[100]={0};
test_label:
	strcpy(name,"SimpleSoft");
	test_work(name,age);
	memset(name,0,sizeof(name));
	age = 0;
	return 0;
}
