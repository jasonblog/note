#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
int threadcount;
long loopcount;
double globalPi=0.0;
pthread_mutex_t mutex;

void thread(void)
{
	double localPi = 0.0;
	double point_x,point_y;
	long incount=0;
	long i;
	long localtotal = loopcount/threadcount;
	
	srand((unsigned)time(NULL));
	
	for(i=0;i<localtotal;i++) {
		/*rand()是否是thread safe*/	
		point_x = (rand())/(RAND_MAX+1.0);
		point_y = (rand())/(RAND_MAX+1.0);	
	
		if( (point_x*point_x + point_y*point_y) < 1.0)
			incount+=1;
	}
	printf("%ld\n", incount);

	pthread_mutex_lock(&mutex);
	globalPi += incount;
	pthread_mutex_unlock(&mutex);
}

void main(int argc,char*argv[]) {
	long count=atol(argv[1]);
	loopcount = count;
	int tcount=atoi(argv[2]);
	threadcount = tcount;
	int i;
	pthread_t id[100];
	pthread_mutex_init(&mutex,NULL);	
	for(i=0;i<threadcount;i++)	
		pthread_create(&id[i],NULL,(void *)thread,NULL);
	for(i=0;i<threadcount;i++)
		pthread_join(id[i],NULL);
	globalPi = (double)4*(globalPi/loopcount);

	printf("pi = %.8lf\n",globalPi);
}
