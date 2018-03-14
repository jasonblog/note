---
title: Linux 進程簡介
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,進程,daemon
description: 簡單介紹一下 Linux 中的常見的一些與進程相關的操作，例如執行命令、守護進程等。
---

簡單介紹一下 Linux 中的常見的一些與進程相關的操作，例如執行命令、守護進程等。

<!-- more -->

## 進程執行

Linux 上將進程創建和新進程加載分開，分別通過 `fork()` 和 `execNN()` 執行，其中後者包含了一組可用的函數，包括了 `execl`、`execlp`、`execle`、`execv`、`execvp` 。

創建了一個進程後，再將子進程替換成新的進程，其聲明如下：

{% highlight c %}
#include <unistd.h>

extern char **environ;

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
{% endhighlight %}

其中，`path` 表示要啟動程序的名稱包括路徑名；`arg` 表示啟動程序所帶的參數，一般第一個參數為要執行命令名，不是帶路徑且 arg 必須以 NULL 結束。

注意，上述 exec 系列函數底層都是通過 `execve()` 系統調用實現。

{% highlight python %}
#include <unistd.h>
int execve(const char *filename, char *const argv[],char *const envp[]);
{% endhighlight %}

其中各個函數的區別如下。

#### 1. 帶 l 的 exec 函數

包括了 execl、execlp、execle，表示後邊的參數以可變參數的形式給出，且都以一個空指針結束。


{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	printf("entering main process---\n");
	execl("/bin/ls","ls","-l",NULL);
	printf("exiting main process ----\n");
	return 0;
}
{% endhighlight %}

利用 execl 將當前進程 main 替換掉，所有最後那條打印語句不會輸出。

#### 2. 帶 p 的 exec 函數

也就是 execlp、execvp，表示第一個參數 path 不用輸入完整路徑，只有給出命令名即可，它會在環境變量 PATH 當中查找命令。

{% highlight python %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	printf("entering main process---\n");
	execlp("ls","ls","-l",NULL);
	printf("exiting main process ----\n");
	return 0;
}
{% endhighlight %}

#### 3. 不帶 l 的 exec 函數

包括了 execv、execvp 表示命令所需的參數以 `char *arg[]` 形式給出，且 arg 最後一個元素必須是 NULL 。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	printf("entering main process---\n");
	char *argv[] = {"ls","-l",NULL};
	execvp("ls", argv);
	printf("exiting main process ----\n");
	return 0;
}
{% endhighlight %}

#### 4. 帶 e 的 exec 函數

包括了 execle ，可以將環境變量傳遞給需要替換的進程，再上述的聲明中，有一個指針數組的變量 `extern char **environ;`，每個指針指向的字符串為 KV 結構。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	char *const envp[] = {"AA=11", "BB=22", NULL};
	printf("entering main process---\n");
	execle("/bin/bash", "bash", "-c", "echo $AA", NULL, envp);
	printf("exiting main process ----\n");
	return 0;
}
{% endhighlight %}









## 守護進程 (daemon)

沒有終端限制，讓某個進程不因為用戶、終端或者其他的變化而受到影響，那麼就必須把這個進程變成一個守護進程。

守護進程的編程本身並不複雜，複雜的是各種版本的 Unix 的實現機制不盡相同，造成不同 Unix 環境下守護進程的編程規則並不一致。

守護進程的特性包括了：

1. 後臺運行；
2. 與運行前的環境隔離開來，這些環境包括未關閉的文件描述符、控制終端、會話和進程組、工作目錄以及文件創建 mask 等，這些環境通常是守護進程從執行它的父進程，特別是 shell，中繼承下來的；
3. 守護進程的啟動方式有其特殊之處，可以從啟動腳本 `/etc/rc.d` 中啟動，`crond` 啟動，還可以由用戶終端執行。

編程步驟包括了。

#### 1. 後臺運行

創建子進程，父進程退出，在進程中調用 `fork()`，然後使父進程終止，讓 Daemon 在子進程中後臺執行，此時在形式上脫離了控制終端。

#### 2. 脫離控制終端、登錄會話和進程組

進程屬於一個進程組，進程組號 (GID) 就是進程組長的進程號；會話可以包含多個進程組，這些進程組共享一個控制終端；這個控制終端通常是創建進程的登錄終端。

控制終端、登錄會話和進程組通常是從父進程繼承下來的，我們的目的就是要擺脫它們，使之不受它們的影響。在此通過調用 `setsid()` 創建新的會話+進程組，並使當前進程成為會話組長。

注意，當進程是會話組長時 `setsid()` 調用失敗，但第一步已保證該進程不是會話組長。調用成功後，進程成為新的會話組長和新的進程組長，並與原來的登錄會話和進程組脫離。由於會話過程對控制終端的獨佔性，進程同時與控制終端脫離。

#### 3. 禁止進程重新打開控制終端

現在，進程已經成為無終端的會話組長，但它可以重新申請打開一個控制終端，可以通過使進程不再成為會話組長來禁止進程重新打開控制終端。

一般來說，也就是再次 `fork()` 一次，不過這個是可選的。

#### 4. 關閉打開的文件描述符

進程從創建它的父進程那裡繼承了打開的文件描述符，如不關閉，將會浪費系統資源，造成進程所在的文件系統無法卸下以及引起無法預料的錯誤。

一般需要關閉 0~2 標準輸出。

<!-- getdtablesize() 返回所在進程的文件描述符表的項數，即該進程打開的文件數目-->

#### 5. 改變當前工作目錄

進程活動時，其工作目錄所在的文件系統不能卸下，一般需要將工作目錄改變到根目錄。對於需要轉儲核心，寫運行日誌的進程將工作目錄改變到特定目錄如 `chdir("/tmp")` 。

#### 6.重設文件權限掩模

進程從創建它的父進程那裡繼承了文件權限掩模，它可能修改守護進程所創建的文件的存取位，為防止這一點，將文件創建掩模清除 `umask(0)`。

#### 7. 處理 SIGCHLD 信號

處理 `SIGCHLD` 信號並不是必須的，但對於某些進程，特別是服務器進程往往在請求到來時生成子進程處理請求。如果父進程不等待子進程結束，子進程將成為殭屍進程 (zombie) 從而佔用系統資源。如果父進程等待子進程結束，將增加父進程的負擔，影響服務器進程的併發性能。

在 Linux 下可以簡單地將 `SIGCHLD` 信號的操作設為 `SIG_IGN，signal(SIGCHLD, SIG_IGN)`，這樣，內核在子進程結束時不會產生殭屍進程，這一點與 BSD4 不同，BSD4 下必須顯式等待子進程結束才能釋放殭屍進程。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main()
{
        FILE *fp;
        time_t t;

        int pid;
        int i;

        pid = fork();   // STEP 1
        if (pid < -1) {        // error
                perror("fork()");
                exit(EXIT_FAILURE);
        } else if (pid != 0) { // parent
                fprintf(stdout, "Parent PID(%d) running", pid);
                exit(EXIT_SUCCESS);
        }

        /* child */
        setsid();     // STEP 2, Detach from session.

        pid = fork(); // STEP 3, fork again to prevent recreate a console.
        if (pid < -1) {        // error
                perror("fork()");
                exit(EXIT_FAILURE);
        } else if (pid != 0) { // parent
                fprintf(stdout, "Parent PID(%d) running", pid);
                exit(EXIT_SUCCESS);
        }

        for(i = 0; i < getdtablesize(); i++) // STEP 4, close all opend file discript.
                close(i);

        chdir("/tmp");  // STEP 5
        umask(0);       // STEP 6

        while (1) {
                fp = fopen("test.log", "a");
                if(fp != NULL ) {
                        t = time(0);
                        fprintf(fp, "I'm here at %s\n", asctime(localtime(&t)) );
                        fclose(fp);
                }
                sleep(60);
        }
}
{% endhighlight %}

### 一次 VS. 兩次 fork()

實際上，在上述的第二步中，可以再執行一次 `fork()` 操作。

第一次 `fork()` 後子進程繼承了父進程的進程組 ID，但有一個新進程 ID，這就保證了子進程不是一個進程組的首進程，然後 `setsid()` 是為了跟主進程的 SID PGID 脫離設置成子進程的 SID PGID。

雖然此時子進程已經被 init 接管了，但是隻有 `setsid()` 之後才算是跟那個主進程完全脫離，不受他的影響。

#### 第二次 fork()

第二次 `fork()` 不是必須的，主要目的是為了防止進程再次打開一個控制終端。

因為打開一個控制終端的前提條件是該進程必須是會話組長，那麼再 `fork()` 一次後，子進程 ID 不再等於 sid (sid 是進程父進程的 sid)，所以也無法打開新的控制終端。

此時這個子進程是首進程了 ，然後此時為了避免他是首進程，所以又 `fork()` 了一次。


## 參考


{% highlight python %}
{% endhighlight %}
