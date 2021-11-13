#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main(int argc,char** argv)
{
	int data=100;
	cout << "begin fork" << endl;
	pid_t pid = fork();
	cout << "after fork,pid is " << pid << endl;
	switch(pid)
	{
	case 0:
		data++;
		cout << "Child data is " << data << endl;
		cout << "My pid is " << getpid() << ",parent pid is " << getppid() << "#####\n" << endl;
		break;
	case -1:
		cout << "error" << endl;
	default:
		data++;
		cout << "Parent data is " << data << endl;
		cout << "My pid is " << getpid() << ",child pid is " << pid << "*****\n" << endl;
		
		break;
	}
	return 0;
}
