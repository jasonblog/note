# C语言互斥锁-双条件变量实现循环打印


```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <error.h>
#include <unistd.h>
#include <pthread.h>

int number[10]={1,2,3,4,5,6,7,8,9,10}; 
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond2;
int i=0;		//数组下标
void read_even(void)		//此线程用来打印偶数
{
	do
	{
		pthread_mutex_lock(&mutex);//锁定互斥锁
		if(i%2==0)		//对应是奇数，就阻塞线程
		{
			/*阻塞线程,等待另外一个线程发送信号，同时为公共数据区解锁*/
			pthread_cond_wait(&cond,&mutex);
		}
		else if(i%2==1)//偶数，打印
		{
			printf("thread2--->number[%d]=%d\n",i,number[i]);
			i++;
			pthread_cond_signal(&cond2);//条件改变，唤醒阻塞的线程
		}
		pthread_mutex_unlock(&mutex);//打开互斥锁
	}while(i<=9);
	pthread_exit(NULL);
}

void read_odd(void)		//此线程用来打印奇数
{
	do
	{
		pthread_mutex_lock(&mutex);
		if(i%2==1)
		{
			pthread_cond_wait(&cond2,&mutex);
		}
		else if(i%2==0)
		{
			printf("thread1--->number[%d]=%d\n",i,number[i]);
			i++;
			pthread_cond_signal(&cond);//条件改变，唤醒阻塞的线程	
		}
		pthread_mutex_unlock(&mutex);
	}while(i<=9);
	pthread_exit(NULL);
}
int main(int argc,char **argv)
{
	pthread_t id,id2;int ret=-1;
	pthread_cond_init(&cond,NULL);//初始化条件变量
	pthread_cond_init(&cond2,NULL);
	pthread_mutex_init(&mutex,NULL);//初始化互斥锁
	ret=pthread_create(&id,NULL,(void *)read_odd,NULL);
	if(ret!=0)
	{
		printf ("Create pthread error!\n");
		exit (1);
	}
	ret=pthread_create(&id2,NULL,(void *)read_even,NULL);//返回值为0，创建成功
	if(ret!=0)
	{
		printf ("Create pthread error!\n");
		exit (1);
	}
	pthread_join(id,NULL);
	pthread_join(id2,NULL);
	pthread_mutex_destroy(&mutex);//销毁互斥锁
	pthread_cond_destroy(&cond);//销毁条件变量
	pthread_cond_destroy(&cond2);
	return 0;
}
```

