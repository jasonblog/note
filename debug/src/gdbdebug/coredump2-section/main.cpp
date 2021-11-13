
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <cstring>

using namespace std;
void set_value(char* str,int num)
{
	str[num-1]='\0';
}
string get_file_list(int total_num,int size)
{
	string res;
	for(int i=0;i<total_num;i++)
	{
		char *str = (char*)alloca(size);
		set_value(str,size);
		res = res + string(str);
	}
	return res;
}
int main(int argc,char** argv)
{
	string str = get_file_list(atoi(argv[1]),atoi(argv[2]));
	cout << "get file list completed" << endl;
	return 0;
}

