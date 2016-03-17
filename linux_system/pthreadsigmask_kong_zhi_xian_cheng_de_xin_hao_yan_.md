# pthread_sigmask 控制線程的信號掩碼


當我們的程序裡面想屏蔽某個信號的時候，我們可以使用pthread_sigmask函數進行信號掩碼設置，從而屏蔽信號。

```c
#include <signal.h>
 
int pthread_sigmask(int how, const sigset_t *restrict set,
       sigset_t *restrict oset);
```


其中，how可以取值為：

- SIG_BLOCK
The resulting set shall be the union of the current set and the signal set pointed to by set.

- SIG_SETMASK
The resulting set shall be the signal set pointed to by set.

- SIG_UNBLOCK
The resulting set shall be the intersection of the current set and the complement of the signal set pointed to by set.

 

###示例一：屏蔽信號SIGINT

```c
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>

int main(int argc, char** argv)
{
  pthread_t tid = pthread_self();

  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  pthread_sigmask(SIG_BLOCK, &mask, NULL);

  printf("[main][%lu] working hard ...\n", tid);

  sleep(60);

  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```

```sh
編譯：gcc -o demo1 demo1.c -lpthread
```

運行後，你發現你按下CTRL+C，這個程序根本停不下來。因為SIGINT信號已經如我們所願被屏蔽掉了。

 

###示例二：主進程創建出來的線程將繼承主進程的掩碼

在主線程裡面pthread_create創建子線程，這個子線程會天生繼承主線程對信號的“喜好”！比如主線程屏蔽SIGINT信號，子線程也天生屏蔽SIGINT信號。主線程可以處理SIGUSR1信號，子線程天生也可以處理SIGUSR1信號！


```c
#include <pthread.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
 
void handler(int signo)
{
  pthread_t tid = pthread_self();
  printf("Thread[%lu] catch signo = %d\n", tid, signo);
  return;
}
 
void* run(void *param)
{
  pthread_t tid = pthread_self();
  printf(">>> Thread[%lu] Running ......\n", tid);
 
  int rc = sleep(300);
  printf("Thread[%lu] waitup(%d), and say good bye!\n", tid, rc);
  return NULL;
}
 
int main(int argc, char** argv)
{
  int ret = 0, i = 0;
  pthread_t tid = pthread_self();
  
  /* 註冊SIGUSR1信號處理函數 */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);
 
  /* 屏蔽信號SIGINT */
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  pthread_sigmask(SIG_BLOCK, &mask, NULL);
 
  pthread_t threads[2];
  pthread_create(&threads[0], NULL, run, NULL);
  printf("[main][%lu] working hard ...\n", tid);
  sleep(1);
 
  /* 主進程創建出來的線程將繼承主進程的掩碼。所以子線程收不到SIGINT信號。 */
  pthread_kill(threads[0], SIGINT);
  printf("[main][%lu] send signal SIGINT ...\n", tid);
  sleep(5);
 
  /* 子線程可以收到SIGUSR1信號。 */
  pthread_kill(threads[0], SIGUSR1);
 
  pthread_join(threads[0], NULL);
 
  sleep(1);
  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```

```sh
編譯：gcc -o demo2 demo2.c -lpthread
```

運行後，你可以發現，子線程果然接收不到pthread_kill發送給自己的SIGINT信號，但可以收到SIGUSR1信號！本來要休眠300s，但是收到了SIGUSR1信號，才休眠5秒就（提前295秒）醒來say goodbye了。

```sh
[main][47519069964448] working hard …
[main][47519069964448] send signal SIGINT …
>>> Thread[1100441920] Running ……
Thread[1100441920] catch signo = 10
Thread[1100441920] waitup(295), and say good bye!
[main][47519069964448] good bye and good luck!
```

### 實例三：子線程可以後天培養自己對信號的喜好


由此可見，linux裡的每個線程有自己的信號掩碼，所以使用pthread_kill給指定線程發送信號時，一定謹慎設置好線程的信號掩碼。

當然，用kill發送信號，在多線程環境下，kill所產生的信號時傳遞到整個進程的，並且所有線程都有機會收到這個信號，但具體是哪個線程處理這個信號，就不一定。一般情況下，都是主線程處理這個信號。

```c
#include <pthread.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
 
void handler(int signo)
{
  pthread_t tid = pthread_self();
  printf("[%u]Thread[%lu] catch signo = %d ...\n", time(NULL), tid, signo);
  sleep(20);
  printf("[%u]Thread[%lu] catch signo = %d ... done\n", time(NULL), tid, signo);
  return;
}
 
void* run(void *param)
{
  pthread_t tid = pthread_self();
  sigset_t mask;
#if 1
  /* 這種情況下，本線程屏蔽所有的信號 */
  sigfillset(&mask);
#endif
 
#if 0
  /* 這種情況下，本線程不屏蔽任何信號 */
  sigemptyset(&mask);
#endif
 
#if 0 
  /* 這種情況，本線程屏蔽以下的指定信號 */
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGHUP);
  sigaddset(&mask, SIGTERM);
#endif 
 
  pthread_sigmask(SIG_SETMASK, &mask, NULL);
  
  printf(">>> [%u]Thread[%lu] Running ......\n", time(NULL), tid);
 
  int rc = sleep(300);
 
  printf("[%u]Thread[%lu] waitup(%d), and say good bye!\n", time(NULL), tid, rc);
  return NULL;
}
 
int main(int argc, char** argv)
{
  pthread_t tid = pthread_self();
  
  /* 註冊SIGUSR1信號處理函數 */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);
 
  pthread_t threads[1];
  pthread_create(&threads[0], NULL, run, NULL);
  printf("[main][%lu] working hard ...\n", tid);
  sleep(5);
 
  /* 子線程屏蔽了SIGUSR1信號，所以子線程收不到SIGUSR1信號。 */
  pthread_kill(threads[0], SIGUSR1);
  printf("[main][%lu] send signal SIGUSR1 ...\n", tid);
  sleep(5);
 
  pthread_join(threads[0], NULL);
  sleep(1);
 
  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```
```sh
編譯：gcc -o demo3 demo3.c -lpthread
```

運行程序：
```sh
[amcool@leoox signal]$ ./demo3
[main][47634313767072] working hard …
>>> [1426338343]Thread[1101465920] Running ……
[main][47634313767072] send signal SIGUSR1 …
```

可見子線程果然通過後天的努力，屏蔽了SIGUSR1信號。pthread_kill發來SIGUSR1信號，子線程仍然高枕無憂的睡大覺！

這時候在另外一個終端，通過kill命令給demo3進程發送多個SIGUSR1信號，

```sh
[amcool@leoox ~]$ kill -10 360
[amcool@leoox ~]$ kill -10 360
```

你會看到主線程會排隊處理這些信號，子線程收不到任何這些信號。

```sh
[main][47634313767072] working hard …
>>> [1426338343]Thread[1101465920] Running ……
[main][47634313767072] send signal SIGUSR1 …

[1426338375]Thread[47634313767072] catch signo = 10 …
[1426338395]Thread[47634313767072] catch signo = 10 … done
[1426338395]Thread[47634313767072] catch signo = 10 …
[1426338415]Thread[47634313767072] catch signo = 10 … done
[1426338643]Thread[1101465920] waitup(0), and say good bye!
[main][47634313767072] good bye and good luck!
```

### 實例四：主線程收到信號，沒處理完，又來一個信號，子線程會處理嗎?

```c
#include <pthread.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
 
void handler(int signo)
{
  pthread_t tid = pthread_self();
  printf("[%u]Thread[%lu] catch signo = %d ...\n", time(NULL), tid, signo);
  /*
   * 信號處理函數休眠20秒，這期間再發送同一個信號，觀察子線程的表現。
   */
  sleep(20);
  printf("[%u]Thread[%lu] catch signo = %d ... done\n", time(NULL), tid, signo);
  return;
}
 
void* run(void *param)
{
  pthread_t tid = pthread_self();
  printf(">>> [%u]Thread[%lu] Running ......\n", time(NULL), tid);
 
  int rc = sleep(300);
 
  printf(">>> [%u]Thread[%lu] waitup(%d), and say good bye!\n", time(NULL), tid, rc);
  return NULL;
}
 
int main(int argc, char** argv)
{
  pthread_t tid = pthread_self();
  
  /* 註冊SIGUSR1信號處理函數 */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);
 
  pthread_t threads[1];
  pthread_create(&threads[0], NULL, run, NULL);
  printf("[main][%lu] working hard ...\n", tid);
 
  pthread_join(threads[0], NULL);
  sleep(1);
 
  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```


編譯運行，運行後，其他終端發送4個SIGUSR1信號。
```sh
[amcool@leoox signal]$ gcc -o demo4 demo4.c -lpthread
[amcool@leoox signal]$ ./demo4
```

```sh
[amcool@leoox ~]$ kill -10 464
[amcool@leoox ~]$ kill -10 464
[amcool@leoox ~]$ kill -10 464
[amcool@leoox ~]$ kill -10 464
```

程序運行的結果如下：
<p>
[main][47848475240608] working hard …<br>
&gt;&gt;&gt; [1426341588]Thread[1089280320] Running ……<br>
[<span style="color: rgb(255, 0, 0);">1426341600</span>]Thread[<span style="color: rgb(255, 0, 0);">47848475240608</span>] catch signo = 10 …<br>
[<span style="color: rgb(0, 128, 0);">1426341606</span>]Thread[<span style="color: rgb(0, 128, 0);">1089280320</span>] catch signo = 10 …<br>
[<span style="color: rgb(255, 0, 0);">1426341620</span>]Thread[<span style="color: rgb(255, 0, 0);">47848475240608</span>] catch signo = 10 … done<br>
[<span style="color: rgb(0, 0, 255);">1426341620</span>]Thread[<span style="color: rgb(0, 0, 255);">47848475240608</span>] catch signo = 10 …<br>
[<span style="color: rgb(0, 128, 0);">1426341626</span>]Thread[<span style="color: rgb(0, 128, 0);">1089280320</span>] catch signo = 10 … done<br>
&gt;&gt;&gt; [1426341626]Thread[1089280320] waitup(282), and say good bye!<br>
[<span style="color: rgb(0, 0, 255);">1426341640</span>]Thread[<span style="color: rgb(0, 0, 255);">47848475240608</span>] catch signo = 10 … done<br>
[main][47848475240608] good bye and good luck!</p>


##【分析結果】

可見，當子線程天生的繼承主線程的信號掩碼的時候，子線程也是可以收到SIGUSR1信號，並進行處理的。

紅色的日誌，表明第一個SIGUSR1信號，主線程捕獲，並且執行。

因為這個信號執行過程比較久（需要20秒），所以下一個SIGUSR1信號，就由子線程捕獲並進行處理了（綠色日誌）。這也就說明了，kill指令發送的信號是送達整個進程的，而多線程環境裡面，只要沒屏蔽這個信號的線程都是有機會收到信號並進入信號處理函數進行信號處理的。

子線程處理完這個信號後，之前的sleep(300)被打擾了，只能提前282秒結束了，然後子線程就退出了。

 

主線程處理完第一個信號後，立馬接受第三個SIGUSR1信號，並進行處理（藍色日誌）。需要20秒。這20秒期間，子線程處理完第二個SIGUSR1信號退出了。

當主線程處理完第三個SIGUSR1信號，整個程序就結束了，所以第四個SIGUSR1信號是得不到處理的。