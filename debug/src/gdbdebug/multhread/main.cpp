#include <iostream>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
using namespace std;

void *test_thread(void *data)
{
	int *val = (int*)data;
	cout << "thread data:" << *val << endl;
	sleep(2);
	return 0;
}
int main(int argc,char** argv)
{
	pthread_t id1,id2;
	int data=1;
	pthread_create(&id1, NULL, test_thread, &data);
	int data2=2;
	pthread_create(&id2, NULL, test_thread, &data2);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	cout << "threads exit" << endl;
	return 0;
}
