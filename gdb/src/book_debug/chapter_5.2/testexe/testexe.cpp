#include <iostream>
#include "test.h"
using namespace std;
int main()
{
	printf("1:call calc_pi 2:exit\n");
	while(true)
	{
		int number = 0;
		cin >> number;
		switch(number)
		{
			case 1:
			{
				double pi = calc_pi();
				cout << "pi is " << pi << endl;
				break;
			}
			case 2:
				return 0;
			default:
				break;
		}
	}
	return 0;
}
