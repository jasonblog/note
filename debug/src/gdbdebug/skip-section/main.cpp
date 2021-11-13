#include <iostream>
#include <cstring>
#include "test.h"
#include "common/test.h"
using namespace std;
void test_str(const std::string &str)
{
	cout << "string is " << str << endl;

}
void test_number(int number)
{
	cout << "the number is " << number << endl;
}
int main(int argc,char** argv)
{
	test_c test;
	common_c com;	
	test_str(test.get_str());
	test_number(test.get_number());
	com.get_number();
	return 0;
}
