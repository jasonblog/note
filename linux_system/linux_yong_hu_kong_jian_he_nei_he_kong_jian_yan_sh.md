# linux 用戶空間 和 內核空間 延時函數

##用戶空間:

1.`unsigned int sleep(unsigned int seconds);`<br>
  sleep()會使當前程序休眠seconds秒。如果sleep()沒睡飽，它將會返回還需要補眠的時間，否則一般返回零。 
 
2. `void usleep(unsigned long usec); ` <br>
 usleep與sleep()類同，不同之處在於休眠的時間單位為毫秒（10E-6秒）。 
 
3. `int select(0,NULL,NULL,NULL,struct timeval *tv);`  <br>
  可以利用select實現sleep()的功能，它將不會等待任何事件發生。 
 
4. `int nanosleep(struct timespec *req,struct timespec *rem);`<br>  
  nanosleep會沉睡req所指定的時間，若rem為non-null，而且沒睡飽，將會把要補眠的時間放在rem上。

實際上用select是`萬能`的，下面的是一個使用select的例子：


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

iSec 為延時秒數,Usec為延時微秒數.

注:
1秒=1000毫秒=1000000微秒=1000000000納秒=1000000000000皮秒=1000000000000000飛秒
1s=1000ms=1000000us=1000000000ns=1000000000000ps=1000000000000000fs



##內核空間:
內核裡面已經實現了延時函數.
#include <linux/delay.h>
udelay(int n);  延時n微秒
mdelay(int n);  延時n毫秒
ndelay(int n);  延時n納秒