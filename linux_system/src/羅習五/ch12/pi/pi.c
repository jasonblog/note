#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

int num_thread;
long total_loopcount;
double global_hit=0.0;
pthread_mutex_t mutex;
long hit[16];
int idx = 0;

void sighandler(int signum){
	for (int i=0; i<idx; i++)
	printf("now, thread%d hit = %ld\n",i ,hit[i]);
}

void thread(void* rand_buffer) {
	double point_x,point_y;
	long i;
	int j;
	long local_loopcount = total_loopcount/num_thread;
	double rand_d;

	pthread_mutex_lock(&mutex);
	long* local_hit = &hit[idx++];
	pthread_mutex_unlock(&mutex);

	for(i=0;i<local_loopcount;i++) {	

		drand48_r(rand_buffer, &rand_d);
		point_x = rand_d;
		drand48_r(rand_buffer, &rand_d);
		point_y = rand_d;
		if( (point_x*point_x + point_y*point_y) < 1.0)
			*local_hit+=1;
	}
	printf("hit = %ld\n", *local_hit);
	pthread_mutex_lock(&mutex);
	global_hit += *local_hit;
	pthread_mutex_unlock(&mutex);
}


void main(int argc,char*argv[]) {
	pthread_t id[16];
	struct drand48_data* rand_buffer[16];
	int i;
	double pi = 0.0;
	double rand_d;

	total_loopcount=atol(argv[1]);
	num_thread=atoi(argv[2]);
	assert(num_thread < 16);

	signal(SIGTSTP, sighandler);

	pthread_mutex_init(&mutex,NULL);	
	for(i=0;i<num_thread;i++) {
		/*使用aligned_alloc分配記憶體，避免false sharing*/
		/*在這個例子中，「剛好」用malloc也會得到相同效果*/
		rand_buffer[i] = aligned_alloc(64, sizeof(struct drand48_data));
		srand48_r(rand(), rand_buffer[i]);
		drand48_r(rand_buffer[i], &rand_d);
		printf("@buffer = %p\n", rand_buffer[i]);
		printf("thread%d's seed = %f\n", i, rand_d);
		pthread_create(&id[i],NULL,(void *)thread,rand_buffer[i]);
	}

	for(i=0;i<num_thread;i++)
		pthread_join(id[i],NULL);

	pi = (double)4*(global_hit/total_loopcount);
	printf("pi = %.8lf\n",pi);
}
