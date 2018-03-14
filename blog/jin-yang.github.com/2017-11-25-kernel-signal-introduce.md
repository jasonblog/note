---
title: Linux 信號機制
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,signal,信號
description: 軟中斷信號 (簡稱為信號) 是用來通知進程發生了異步事件，是在軟件層次上是對中斷機制的一種模擬，在原理上，一個進程收到一個信號與處理器收到一箇中斷請求可以說是一樣的。信號是進程間通信機制中唯一的異步通信機制，一個進程不必通過任何操作來等待信號的到達，事實上，進程也不知道信號到底什麼時候到達。進程之間可以互相通過系統調用 kill 發送軟中斷信號，內核也可以因為內部事件而給進程發送信號，通知進程發生了某個事件。信號機制除了基本通知功能外，還可以傳遞附加信息。
---

軟中斷信號 (簡稱為信號) 是用來通知進程發生了異步事件，是在軟件層次上是對中斷機制的一種模擬，在原理上，一個進程收到一個信號與處理器收到一箇中斷請求可以說是一樣的。

信號是進程間通信機制中唯一的異步通信機制，一個進程不必通過任何操作來等待信號的到達，事實上，進程也不知道信號到底什麼時候到達。

進程之間可以互相通過系統調用 kill 發送軟中斷信號，內核也可以因為內部事件而給進程發送信號，通知進程發生了某個事件。

信號機制除了基本通知功能外，還可以傳遞附加信息。

<!-- more -->

## 信號處理

每個信號在 `signal.h` 頭文件中通過宏進行定義，對於 CentOS 來說，實際是在 `/usr/include/asm-generic/signal.h` 中定義，對於編號以及信號名的映射關係可以通過 `kill -l` 命令查看。

其中，[1, 31] 是普通信號 而 [34, 64] 是實時信號。

對於信號，通常有如下的幾種處理方式：

1. 忽略。大部分信號都可以通過這種方式處理，不過 `SIGKILL` 和 `SIGSTOP` 兩個信號有特殊用處，不能被忽略。
2. 默認動作。大多數信號的系統默認動作終止該進程。
3. 捕捉信號。也就是在收到信號時，執行一些用戶自定義的函數。

其中信號可以簡單通過 `signal()` 函數指定。


### 簡單處理信號

其中 `signal()` 函數的聲明如下：

{% highlight c %}
#include <signal.h>
typedef void(*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
{% endhighlight %}

其中，`signal()` 用於對該進程的某個特定信號 (signum) 註冊一個相應的處理函數，也就是修改對該信號的默認處理動作。

注意，`signal()` 會堵塞當前正在處理的信號，不過不能阻塞其它信號，比如正在處理 `SIG_INT`，再來一個 `SIG_INT` 則會堵塞，但如果是 `SIG_QUIT` 則會被其中斷，在處理完 `SIG_QUIT` 信號之後，`SIG_INT` 才會接著剛才處理。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int signum)
{
	printf("Got a signal %d\n", signum);
}

int main()
{
	signal(SIGINT, handler);

	while(1) {
		sleep(1);
		printf(".");
		fflush(stdout);
	}
	return 0;
}
{% endhighlight %}

也可以通過如下方式忽略某一個信號。

{% highlight c %}
signal(SIGHUP, SIG_IGN);
{% endhighlight %}

這裡表示忽略 `SIGHUP` 這個信號，該信號與控制檯有關，當控制檯被關閉時，操作系統會向擁有控制檯 SessionID 的所有進程發送 HUP 信號，而默認 HUP 信號的處理是退出程序。當遠程登陸啟動某個服務進程並在程序運行時關閉終端的話會導致服務進程退出，所以一般服務進程都會用 nohup 工具啟動或寫成一個 daemon。

## 信號處理過程

進程收到一個信號後不會被立即處理，而是在恰當時機進行處理！一般是在中斷返回的時候，或者內核態返回用戶態的時候 (這個情況出現的比較多)。

也就是說，信號不一定會被立即處理，操作系統不會為了處理一個信號而把當前正在運行的進程掛起，因為這樣的話資源消耗太大了，如果不是緊急信號，是不會立即處理的，操作系統多選擇在內核態切換回用戶態的時候處理信號。

因為進程有可能在睡眠的時候收到信號，操作系統肯定不願意切換當前正在運行的進程，於是就得把信號儲存在進程唯一的 PCB(task_struct) 當中。

### 信號觸發

一般信號的觸發大致可以分為如下的幾類：

1. 用戶在終端中觸發信號，終端驅動程序會發送信號給前臺程序。例如 `Ctrl-C(SIGINT)`、`Ctrl-\(SIGQUIT)`、`Ctrl-Z(SIGTSTP)` 。
2. 硬件異常產生信號。通常由硬件檢測到並通知內核，然後內核向當前進程發送適當的信號。例如，除 0 異常 (CPU運算單元觸發，內核通過`SIGFPE`信號發送給當前進程)；內存非法地址訪問 (MMU觸發異常，內核通過`SIGSEGV`信號發送給當前進程)。
3. 一個進程調用 `kill()` 函數可以發送信號給另一個進程，默認會發送 `SIGTERM` 信號，該信號的默認處理動作是終止進程。

當 CPU 正在執行某個進程時，通過終端驅動程序發送了一個 SIGINT 信號給該進程，該信號會記錄在響應進程的 PCB 中，則該進程的用戶空間代碼暫停執行，CPU 從用戶態切換到內核態處理硬件中斷。

從內核態回到用戶態之前，會先處理 PCB 中記錄的信號，發現有一個 `SIGINT` 信號待處理，而這個信號的默認處理動作是終止進程，所以直接終止進程而不再返回它的用戶空間代碼執行。

#### 函數調用

通過 `raise()` 可以給當前進程發送指定的信號；`kill()` 函數向指定進程發送信號；而 `abort()` 函數使當前進程接收到 `SIGABRT` 信號，其函數聲明如下：

{% highlight c %}
#include<signal.h>
int kill(pid_t pid,int signo);
int raise(int signo);

#include<stdlib.h>
void abort(void);
{% endhighlight %}

類似於 `exit()` 函數，`abort()` 函數總是會成功的，所以沒有返回值。

### 信號阻塞

信號在內核中的表示大致分為如下幾類：

1. 信號遞達 (delivery) 實際執行信號處理信號的動作。
2. 信號未決 (pending) 信號從產生到抵達之間的狀態，信號產生了但是未處理。
3. 忽略，抵達之後的一種動作。
4. 阻塞 (block) 收到信號不立即處理，被阻塞的信號將保持未決狀態，直到進程解除對此信號的阻塞，才執行抵達動作。

每個信號都由兩個標誌位分別表示阻塞和未決，以及一個函數指針表示信號的處理動作。

![signal task structure]({{ site.url }}/images/linux/kernel/signal-task-struct-stuff.png "signal task structure"){: .pull-center }

在上圖的例子中，其狀態信息解釋如下：

* `SIGHUP` 未阻塞也未產生過，當它遞達時執行默認處理動作。
* `SIGINT` 信號產生過，但正在被阻塞，所以暫時不能遞達。雖然它的處理動作是忽略，但在沒有解除阻塞之前不能忽略這個信號，因為進程仍有機會改變處理動作之後再解除阻塞。
* `SIGQUIT` 信號未產生過，一旦產生 `SIGQUIT` 信號將被阻塞，它的處理動作是用戶自定義函數 sighandler。

信號產生但是不立即處理，前提條件是要把它保存在 pending 表中，表明信號已經產生。

### 信號集操作函數

信號集用來描述信號的集合，每個信號佔用一位，總共 64 位，Linux 所支持的所有信號可以全部或部分的出現在信號集中，主要與信號阻塞相關函數配合使用。

執行信號的處理動作稱為信號遞達 (Delivery)，信號從產生到遞達之間的狀態，稱為信號未決 (Pending)，進程可以選擇阻塞 (Block) 某個信號，被阻塞的信號產生時將保持在未決狀態，直到進程解除對此信號的阻塞，才執行遞達的動作。

注意，阻塞和忽略是不同的，信號被阻塞就不會遞達，而忽略是在遞達之後可選的一種處理動作。

每個進程都有一個用來描述哪些信號遞送到進程時將被阻塞的信號集，該信號集中的所有信號在遞送到進程後都將被阻塞，信號在內核中的表示可以看作是這樣的：

如下是常見的信號集的操作函數：

{% highlight c %}
#include <signal.h>
int sigemptyset(sigset_t *set);                  /* 所有信號的對應位清0 */
int sigfillset(sigset_t *set);                   /* 設置所有的信號，包括系統支持的所有信號 */
int sigaddset(sigset_t *set, int signo);         /* 在該信號集中添加有效信號 */
int sigdelset(sigset_t *set, int signo);         /* 在該信號集中刪除有效信號 */
int sigismember(const sigset_t *set, int signo); /* 用於判斷一個信號集的有效信號中是否包含某種信號 */
{% endhighlight %}

如下是一個操作示例。

{% highlight c %}
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

void print_sigset(sigset_t *set)
{
	int i;
	for(i = 1; i < NSIG; ++i){
		if(sigismember(set, i))
			putchar('1');
		else
			putchar('0');
	}
	putchar('\n');
}

int main(void)
{
	sigset_t foobar;

	sigemptyset(&foobar);
	sigaddset(&foobar, SIGINT);
	sigaddset(&foobar, SIGQUIT);
	sigaddset(&foobar, SIGUSR1);
	sigaddset(&foobar, SIGRTMIN);

	print_sigset(&foobar);

	return 0;
}
{% endhighlight %}

調用 `sigprocmask()` 函數可以讀取或更改進程的信號屏蔽字：

{% highlight c %}
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
{% endhighlight %}

一個進程的信號屏蔽字規定了當前阻塞而不能遞送給該進程的信號集，如果調用該函數解除了對當前若干個未決信號的阻塞，則在該函數返回前，至少將其中一個信號遞達。

### 實時信號

實時信號支持隊列，可以保證信號不會丟失，其中 1-31 個信號為非實時信號，32-64 為實時信號，當一信號在阻塞狀態下產生多次信號，當解除該信號的阻塞後，非實時信號僅傳遞一次信號，而實時信號會傳遞多次。

對於非實時信號，內核會為每個信號維護一個信號掩碼，並阻塞信號針對該進程的傳遞。如果將阻塞的信號發送給某進程，對該信號的傳遞將延時，直至從進程掩碼中移除該信號為止。當從進程掩碼中移除該信號時該信號將傳遞給該進程。如果信號在阻塞期間傳遞過多次該信號，信號解除阻塞後僅傳遞一次。

對於實時信號，實時信號採用隊列化處理，一個實時信號的多個實例發送給進程，信號將會傳遞多次。可以制定伴隨數據，用於產生信號時的數據傳遞。不同實時信號的傳遞順序是固定的，優先傳遞信號編號小的。


## Signal VS. Sigaction

實際上，上述的 `signal()` 是最早的函數，現在大多系統，包括 Linux 都用 `sigaction()` 重新實現了 `signal()`，其區別如下：

1. `signal()` 註冊的回調函數，會在調用前先清除掉，所以需要在回調函數中重新註冊；而 `sigaction()` 函數如果要刪除需要顯示調用。
2. `signal()` 處理不能阻塞信號，而 `sigaction()` 則可以阻塞指定的信號。

這也就意味著，`signal()` 函數可能會丟失信號。

如下是兩個函數的聲明。

{% highlight c %}
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

#include <signal.h>
struct sigaction {
	void     (*sa_handler)(int);                        /* 信號處理方式 */
	void     (*sa_sigaction)(int, siginfo_t *, void *); /* 實時信號的處理方式 */
	sigset_t   sa_mask;   /* 額外屏蔽的信號 */
	int        sa_flags;
	void     (*sa_restorer)(void);
};
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
{% endhighlight %}

對於 `sigaction()` 函數，如果 `act` 非空，則會根據 `act` 結構體中的信號處理函數來修改該信號的處理動作；如果 `oldact` 非空則會通過該變量將信號原來的處理動作返回。

其中，`sa_handler` 變量用於指定信號的處理函數，有三種方式：

1. SIG_IGN 忽略信號；
2. SIG_DFL 執行系統默認動作；
3. 賦值為函數指針表示用自定義函數捕捉信號。

當某個信號的處理函數被調用時，內核自動將當前信號加入進程的信號屏蔽字，當信號處理函數返回時自動恢復原來的信號屏蔽字，這樣就保證了在處理某個信號時，如果這種信號再次產生，那麼它會被阻塞到當前處理結束為止。


## 常用程序

### Pause

該函數用於掛起進程，直到有信號觸發，其聲明如下：

{% highlight c %}
#include <unistd.h>
int pause(void);
{% endhighlight %}

針對不同的處理行為，其表現不同：A) 終止進程(一般默認)，則進程終止，`pause()` 沒有機會返回；B) 忽略，進程繼續處於掛起狀態，`pause()` 不返回；C) 有信號處理函數，則調用了信號處理函數之後 `pause()` 返回 -1，其 `errno` 設置為 `EINTR` 。

{% highlight c %}
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_alarm(int signum)
{
	printf("I am a custom handler! signal number is %d\n", signum);
}

int main(void)
{
	struct sigaction sig, old;

	/* register a alarm signal handler */
	sig.sa_handler = sig_alarm;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGALRM, &sig, &old);

	printf("A SIGALRM handler has registered\n");
	alarm(3);
	pause();

	printf("Raise another alarm signal, in 1 second later\n");
	alarm(2);

	printf("Try to sleep 10 seconds\n");
	sleep(10);
	alarm(0); /* Cancel the alarm */

	sigaction(SIGALRM, &old, NULL); /* restore */
	alarm(1);
	sleep(2);

	return 0;
}
{% endhighlight %}

如上代碼，其處理過程為：

1. 註冊一個 `SIGALRM` 信號的處理函數，並在 3s 後內核向該進程發送一個 `SIGALRM` 信號；
2. 調用 `pause()` 將進程掛起，內核切換到別的進程運行；
3. 內核向該進程發送 `SIGALRM` 信號，發現其處理動作是一個自定義函數，於是切回用戶態執行該自定義處理函數；
4. 進入處理函數後會自動屏蔽 `SIGALRM` 信號，返回後自動解除屏蔽。

注意，`SIGALRM` 處理會自動取消 `sleep` 的阻塞。

### SIGKILL VS. SIGSTOP

這兩個信號比較特殊，無法在程序中進行屏蔽，用於一些特殊的用途。

#### SIGKILL

也就是直接的 `kill -9` 操作，為 root 提供了一種使進程強制終止方法，此時將會有操作系統直接回收該進程佔用的資源，對於一些保存狀態的應用就可能會導致異常。

#### SIGSTOP

對於前臺運行的程序，可以通過 `Ctrl-Z` 終止程序，切換到後臺，此時進程處於 `TASK_STOPPED` 狀態，`ps` 命令顯示處於 `T` 狀態。如果要恢復運行，應該使用 `fg JOB-ID` 恢復運行，如果直接發送 `SIGCONT` 將會使進程退出。

可以參考 [WikiPedia SIGSTOP](http://en.wikipedia.org/wiki/SIGSTOP) 中的介紹，抄錄如下：

{% highlight text %}
When SIGSTOP is sent to a process, the usual behaviour is to pause that process in its
current state. The process will only resume execution if it is sent the SIGCONT signal.
SIGSTOP and SIGCONT are used for job control in the Unix shell, among other purposes.
SIGSTOP cannot be caught or ignored.
{% endhighlight %}

也就是說，這個信號是用於 Shell 的任務管理，不能被用戶屏蔽。其中常用的是 rsync 的同步任務，例如要清理一些空間，可以暫停運行，清理完成後重新啟動運行。

{% highlight text %}
# kill -s STOP `pidof rsync`
# kill -s CONT `pidof rsync`
{% endhighlight %}



<!--
#define SIG_DFL ((void(*)(int))0)
#define SIG_IGN ((void(*)(int))1)


/* 打印那個進程發送的信號 */

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>


static int int_count = 0;
static int max_int = 5;
static int max_term = 10;

static void multi_handler(int sig, siginfo_t *siginfo, void *context)
{
	pid_t sender_pid = siginfo->si_pid;

	if(sig == SIGINT) {
		printf("INT(%d), from [%d]\n", int_count++, (int)sender_pid);
	} else if(sig == SIGQUIT) {
		printf("Quit, bye, from [%d]\n", (int)sender_pid);
		exit(0);
	} else if(sig == SIGTERM) {
		printf("TERM(%d), from [%d]\n", int_count++, (int)sender_pid);
	}

	return;
}


int main(void)
{
	printf("process [%d] started.\n", (int)getpid());

	static struct sigaction siga;
	siga.sa_sigaction = *multi_handler;
	siga.sa_flags |= SA_SIGINFO; /* get detail info */

	/* Ctrl + C */
	if (sigaction(SIGINT, &siga, NULL) != 0) {
		printf("error sigaction()");
		return errno;
	}

	/* Ctrl + \ */
	if (sigaction(SIGQUIT, &siga, NULL) != 0) {
		printf("error sigaction()");
		return errno;
	}

	if (sigaction(SIGTERM, &siga, NULL) != 0) {
		printf("error sigaction()");
		return errno;
	}


	int sig;
	while (1) {
		if (int_count < max_int) {
			sig = SIGINT;
		} else if (int_count >= max_int && int_count < max_term) {
			sig = SIGTERM;
		} else {
			sig = SIGQUIT;
		}

		raise(sig); /* send signal to itself */
		sleep(1);    // sleep a while, note that: SIGINT will interrupt this, and make program wake up,
	}

	return 0;
}




-->



















<!--
EINTR

## EAGAIN

實際上，在 Linux 中 EAGAIN 和 EWOULDBLOCK 相同。

以非阻塞方式打開文件或者 Sokcet 時，如果連續調用 read() 函數，而當沒有數據可讀時，又不會阻塞等待數據，那麼此時就會返回 EAGAIN 錯誤，用來提示應用程序現在沒有數據可讀請稍後再試。

其它的，如當一個系統調用因為沒有足夠的資源而執行失敗，返回 EAGAIN 提示其再調用一次，也許下次就能成功；例如 fork() 調用時可能內存不足。

## EINTR

當阻塞於某個系統調用時，如果進程捕獲了某個信號，且相應信號處理函數返回後，該系統調用可能會返回一個 EINTR 錯誤。例如，在服務器端等待客戶端鏈接，可能會獲取到 EINTR 。

那麼，為什麼會有這個 EINTR 信號，如果出現瞭如何處理。首先，通常的處理方式是再次調用被中斷的函數，而為什麼有，還比較複雜一些。

假設有如下程序，主循環一直在讀取數據，然後註冊了一箇中斷函數用於標識退出程序，同時在退出前執行一些清理操作。

volatile int stop = 0;
void handler(int)
{
    stop++;
}

void event_loop (int sock)
{
    signal (SIGINT, handler);

    while (1) {
        if (stop) {
            printf ("do cleanup\n");
            return;
        }
        char buf [1];
        int rc = recv (sock, buf, 1, 0);
        if (rc == -1 && errno == EINTR)
            continue;
        printf ("perform an action\n");
    }
}

如上，當程序阻塞到 recv() 時，如果收到了 Ctrl-C 信號，那麼在處理完之後實際上還會阻塞到 recv() 從而形成了死鎖，如要要程序退出只能在接收到數據後進行處理，顯然我們無法判斷到底什麼時候可能會收到數據。

通過返回的 EINTR 錯誤，讓我們可以有機會進行處理，也就是如上的代碼。

http://blog.csdn.net/hs794502825/article/details/52577622

http://blog.csdn.net/hzhsan/article/details/23650697
http://youbingchenyoubing.leanote.com/post/%E8%87%AA%E5%B7%B1%E8%B6%9F%E8%BF%87epoll%E7%9A%84%E5%9D%91
-->

{% highlight text %}
{% endhighlight %}
