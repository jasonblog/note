#include <iostream>
#include <cstring>
#include <thread>
#include <math.h>
using namespace std;

void draw()
{
	std::cout<<"\033[31m" << endl;
	char heart='$';
	for (float y = 1.5f; y > -1.5f; y -= 0.1f) 
 	{ 
		for (float x = -1.5f; x < 1.5f; x += 0.05f) 
		{	 
			float a = x * x + y * y - 1; 
			putchar(a * a * a - x * x * y * y * y <= 0.0f ? heart : ' '); 
		} 
		putchar('\n'); 
 	} 
	std::cout<<"\033[39m" << endl;
}
int main(int argc,char** argv)
{
	cout << "Please input the password" << endl;
	char input[100]={0};
	cin >> input;

	const char* pwd="I_cfb[Ie\\j";
	int len=strlen(input);
	//do some convert
	for(int i=0;i<len;i++)
	{
		input[i]=input[i] - 10;
	}
	if(strcmp(input,pwd) == 0)
	{
		cout << "Congratulate,your password is right!" << endl;
		draw();
	}
	else
	{
		cout << "Password is wrong,can't run!" << endl;
	}
	return 0;
}
