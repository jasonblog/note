#include <iostream>
#include <cstring>
using namespace std;
int check_some()
{	
	int x=100;
	return x;
}

int main(int argc,char** argv)
{
	if(check_some() == 100)
	{
		cout << "check failed!" << endl;
		return 1;
	}
	else
	{
		cout << "check successfully!" << endl;
	}
	//do somethings

	return 0;
}
