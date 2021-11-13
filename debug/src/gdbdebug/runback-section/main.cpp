#include <iostream>
#include <cstring>
using namespace std;
int g1=0;
int g2=0;
void test1()
{	
	g1=5;
	g2=6;
	int a=0;
	int b=0;
	int c=0;
	g1=10;
	g2=20;
	a++;
	b++;
	c++;
	
}
void test2()
{
	FILE* file = fopen("test.txt","a");	
	if(file)
	{
		char buffer[]="this is a test\n";
		fwrite(buffer,sizeof(buffer),1,file);
		fclose(file);
	}
}
int main(int argc,char** argv)
{
	cout << "call test1 " << endl;
	test1();
	test2();
	return 0;
}
