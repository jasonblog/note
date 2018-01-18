# linux 用户空间 和 内核空间 延时函数

##用户空间:

1.`unsigned int sleep(unsigned int seconds);`<br>
  sleep()会使当前程序休眠seconds秒。如果sleep()没睡饱，它将会返回还需要补眠的时间，否则一般返回零。 
 
2. `void usleep(unsigned long usec); ` <br>
 usleep与sleep()类同，不同之处在于休眠的时间单位为毫秒（10E-6秒）。 
 
3. `int select(0,NULL,NULL,NULL,struct timeval *tv);`  <br>
  可以利用select实现sleep()的功能，它将不会等待任何事件发生。 
 
4. `int nanosleep(struct timespec *req,struct timespec *rem);`<br>  
  nanosleep会沉睡req所指定的时间，若rem为non-null，而且没睡饱，将会把要补眠的时间放在rem上。

实际上用select是`万能`的，下面的是一个使用select的例子：


```c
#include <time.h>
#include <sys/time.h>
 
void Sleep(int iSec,int iUsec)
{
    struct timeval tv;
    tv.tv_sec=iSec;
    tv.tv_usec=iUsec;
    select(0,NULL,NULL,NULL,&tv);
}
```

iSec 为延时秒数,Usec为延时微秒数.

注:
1秒=1000毫秒=1000000微秒=1000000000纳秒=1000000000000皮秒=1000000000000000飞秒
1s=1000ms=1000000us=1000000000ns=1000000000000ps=1000000000000000fs



##内核空间:
内核里面已经实现了延时函数.
#include <linux/delay.h>
udelay(int n);  延时n微秒
mdelay(int n);  延时n毫秒
ndelay(int n);  延时n纳秒