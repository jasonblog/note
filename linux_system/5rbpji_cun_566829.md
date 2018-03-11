# 5(rbp寄存器)


昨天在家瀏覽了紐約大學的一個課件，找到一個例子，很有趣，我稍微修改了一下，支持32位和64位環境：如下。

以後該系列採用這樣的模式，首篇只貼代碼，續篇進行解釋和分析，期間希望讀者朋友能自己做實驗。

理解該代碼就算對rbp，rsp寄存器以及一些調用有了直接的接觸。

裡面有兩個奇怪的數字17和23，這是為什麼呢？續篇給予解答。

編譯方法：

64位環境：

```sh
g++ -g  test.cpp -o test_64    //64位代碼
g++ -g  -m32 test.cpp -o test_32        //32位代碼
```
32位環境：

```sh
g++ -g  test.cpp -o test_32
```
 
```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
        
int     done = 0;

int thread1( void )
{
        printf( "Hello from thread1 /n" );
        while ( !done );
        printf( "/nThread1 is quitting/n" );
        exit(0);
}       
        
int thread2( void )
{       
        printf( "Hello from thread2 /n" );
        while ( !done );
        printf( "/nThread2 is quitting/n" );
        exit(0);
}      

int thread3( void )
{
        printf( "Hello from thread3 /n" );
        while ( !done );
        printf( "/nThread3 is quitting/n" );
        exit(0);
}

int thread4( void )
{
        printf( "Hello from thread4 /n" );
        while ( !done );
        printf( "/nThread4 is quitting/n" );
        exit(0);
}

typedef int (*THREAD)(void);
THREAD  ready[] = { thread1, thread2, thread3, thread4 }; 
const int thread_count = sizeof( ready )/sizeof( THREAD );

int     next_thread = 0;        // array-index for 'ready' list

void upon_signal( int signum )
{
        unsigned long   *tos;   // this is used for addressing the stack-frame
        #if defined(__i386__)
        asm(" movl %%ebp, %0 " : "=m" (tos) );  // initializes frame-pointer
        tos[17] = (unsigned long)ready[ next_thread ];
        #elif defined(__x86_64__)

        asm(" mov %%rbp, %0 " : "=m" (tos) );   // initializes frame-pointer
        tos[23] = (unsigned long)ready[ next_thread ];
        #endif
        // This was useful during development -- to display the stack elements
        //for (int i = 0; i < 30; i++) printf( "before:tos[%d]=%16X /n", i, tos[i] );

        next_thread = ( 1 + next_thread ) % thread_count;  // "circular" array

        if ( signum == SIGINT ) done = 1;       // to terminate on <CONTROL>-C
}

int main( int argc, char **argv )
{
        signal( SIGINT, upon_signal );

        struct itimerval  it;
        it.it_value.tv_sec = 1;
        it.it_value.tv_usec = 0;
        it.it_interval.tv_sec = 0;
        it.it_interval.tv_usec = 200000;
        setitimer( ITIMER_REAL, &it, NULL );
        signal( SIGALRM, upon_signal );

        while ( !done );

        printf( "/nQuitting/n" );
}
```