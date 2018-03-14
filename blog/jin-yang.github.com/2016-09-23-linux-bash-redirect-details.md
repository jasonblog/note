---
title: Bash 重定向
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: bash,redirect,重定向
description: 所謂 IO 重定向簡單來說就是一個過程，這個過程捕捉一個文件、命令、程序、腳本、甚至腳本中的代碼塊的輸出，然後把捕捉到的輸出，作為輸入發送給另外一個文件、命令、程序、或者腳本。這裡簡單介紹常用命令及其實現。
---

所謂 IO 重定向簡單來說就是一個過程，這個過程捕捉一個文件、命令、程序、腳本、甚至腳本中的代碼塊的輸出，然後把捕捉到的輸出，作為輸入發送給另外一個文件、命令、程序、或者腳本。

這裡簡單介紹常用命令及其實現。

<!-- more -->


## 重定向

文件描述符 (File Descriptor) 是進程對其所打開文件的索引，形式上是個非負整數。在 Linux 系統中，系統為每一個打開的文件指定一個文件標識符以便系統對文件進行跟蹤。

和 C 語言編程裡的文件句柄相似，文件標識符是一個數字，不同數字代表不同的含義，默認情況下，系統佔用了 3 個，分別是 0 標準輸入 (stdin) ， 1 標準輸出 (stdout) ， 2 標準錯誤 (stderr) ，另外 3-9 是保留的標識符，可以把這些標識符指定成標準輸入，輸出或者錯誤作為臨時連接。通常這樣可以解決很多複雜的重定向請求。

### 使用方法

可以簡單地用 `<` 或 `>` ，默認相當於使用 `0<` 或 `1>`；管道 (`|` pipe line)，把上一個命令的 stdout 接到下一個命令的 stdin；`tee` 命令的作用是在不影響原本 IO 的情況下，將 stdout 複製一份到文件中去，例如 `cat file.txt | tee backup` 。

重新定義文件標識符可以用 `i>&j` 命令，表示把文件標識符 i 重新定向到 j ，你可以把 `"&"` 理解為 "取地址" 。


### 常見用例

------------------------------

{% highlight text %}
cmd > file
{% endhighlight %}

把 cmd 命令的輸出重定向到文件 file 中。如果 file 已經存在，則清空原有文件，使用 bash 的 `noclobber` 選項可以防止覆蓋原有文件。

{% highlight text %}
----- 阻止文件重定向操作對一個文件的覆蓋，或者使用 set -C
set -o noclobber
----- 恢復文件重定向操作對一個文件的覆蓋
set +o noclobber
{% endhighlight %}

------------------------------

{% highlight text %}
cmd >| file
{% endhighlight %}

功能同 `>` ，但即便在設置了 `noclobber` 時也會覆蓋 file 文件，注意用的是 `|` 而非一些資料中說的 `!` ，目前僅在 csh 中仍沿用 `>!` 實現這一功能。

<!--
		cmd >> file<br>
        把 cmd 命令的輸出重定向到文件 file 中，如果 file 已經存在，則把信息加在原有文件後面。</li><br><li>

		cmd &lt; file<br>
        使 cmd 命令從 file 讀入。</li><br><li>

		cmd &lt;&lt; text<br>
        從命令行讀取輸入，直到一個與 text 相同的行結束。除非使用引號把輸入括起來，此模式將對輸入內容進行 shell 變量替換。如果使用 &lt;&lt;- ，則會忽略接下來輸入行首的 tab ，結束行也可以是一堆 tab 再加上一個與 text 相同的內容。</li><br><li>

		cmd &lt;&lt;&lt; word ???????????????<br>
        把 word（而不是文件 word ）和後面的換行作為輸入提供給 cmd 。</li><br><li>

    cmd &lt;&gt; file<br>
        以讀寫模式把文件 file 重定向到輸入，文件 file 不會被破壞。僅當應用程序利用了這一特性時，它才是有意義的。</li><br><li>


		使用文件描述符的重定向 (都使用了 & 符號)<br>
<pre>
cmd &>file      標準輸入輸出重定向到 file
cmd >&file
cmd >file 2>&1

&>>word         標準輸入輸出追加到 file ，沒有 >>&word 的表達方法
>>word 2>&1

cmd >&n         把輸出送到文件描述符 n ，複製文件描述符
cmd m>&n        把輸出到文件符 m 的信息重定向到文件描述符 n
cmd >&-         關閉標準輸出

cmd <&n         輸入來自文件描述符 n ，複製文件描述符
cmd m<&n        m 來自文件描述符 n
cmd <&-         關閉標準輸入

cmd <&n-        移動輸入文件描述符 n 而非複製它
cmd >&n-        移動輸出文件描述符 n 而非複製它</pre></li><br><li>

		關閉文件標識符的操作<br>
	"j&lt;&gt;filename" 命令表示把文件打開，並指明文件標識符為 j ； "&-" 表示關閉文件標識符。
<pre>
n<&- 關閉輸入文件標識符 n
0<&- 或 <&- 關閉標準輸入 stdin
n>&- 關閉輸出文件標識符 n
1>&- 或 >&- 關閉標準輸出 stdout
</pre></li><br><li>

		重定向的組合應用<br>
<pre>
cmd 2>file         把文件描述符 2 重定向到 file，即把錯誤輸出存到 file 中。
cmd > file 2>&1    把標準錯誤重定向到標準輸出，再重定向到file，即stderr和stdout都被輸出到file中
cmd >> file 2>&1   追加把標準錯誤重定向到標準輸出，再重定向到file，即stderr和stdout都被輸出到file中
cmd &> file        功能與上一個相同，更為簡便的寫法。
cmd >& file        功能仍與上一個相同。
cmd >>& file       追加。
cmd > f1 2>f2      把stdout重定向到f1，而把stderr重定向到f2
tee files          把stdout原樣輸出的同時，複製一份到files中。
tee files          把stderr和stdout都輸出到files中，同時輸出到屏幕。</pre>
	</li></ol><br><br><br>


	如果要深刻理解重定向，先要明白以下 2 點。
	<ol type="A"><li>
		shell ( bash 或者 csh 等) 負責重定向，對於程序或者函數，這一切都是透明的，它只管輸入輸出，至於從哪輸入哪輸出，是 shell 解釋器負責。</li><br><li>

		shell 命令解析過程中，在實際執行命令前， IO 重定向先設置好。
	</li></ol>

-->

> exec 是 Shell 的內建命令，通常用於替換 Shell 執行命令。在重定向中，exec 用來操作文件描述符，當不指定命令時，會修改當前 shell 的文件描述符。



### 示例

{% highlight text %}
# ls /dev 1>filename               # 將標準輸出重定向到文件中， "1" 和 ">" 中間沒有空格
# ls /dev  >filename               # 與上等價，系統默認是 1 ，因此 1 可以省略
# ls /dev >>filename               # 追加到文件，而非創建
# ls -qw /dev 2>filename           # 將標準錯誤重定向到文件
# ls /dev &>filename               # 將 stdio/stderr 都輸入到文件，"&" 在這裡代表 stdio/stderr
# ls /dev >&filename               # 與上同
# ls /dev >filename 2>&1           # 與上同
# ls 2>&1 > filename               # 只有標準輸出重定向到 filename

# exec 5>&1                        # 把文件標識符 5 定向到標準輸出，通常用來臨時保存標準輸入
# grep word <filename              # 重定向標準輸入，與下面相同
# grep word 0<filename             # 把文件 filename 作為 grep 命令的標準輸入，而不是從鍵盤輸入

# echo 123456789 >file             # 把字符串寫到文件 file 中
# exec 3<>file                     # 把文件 file 打開，並指定文件標識符為 3 ，默認為 0
# read -n 4 <&3                    # 從文件中讀 4 個字符，句柄已經指到第四個字符末尾
# echo -n . >&3                    # 在第 5 個字符處寫一個點，覆蓋第 5 個字符， -n 表示不換行
# exec 3>&-                        # 關閉文件標識符 3
# cat file                         # file 文件的結果就成了 1234.6789

# touch filename
# cat filename
# set -o noclobber
# echo 2 >filename
bash: filename: cannot overwrite existing file
# echo 2 >| filename
# cat filename
2
# set +o noclobber
{% endhighlight %}


<!--
	<br><br>
	<h2>tee 命令</h2>
	<p>
		tee 指令會從標準輸入設備讀取數據，將其內容輸出到標準輸出設備，同時保存成文件。如果指定的文件不存在，則創建；有則覆蓋，除非使用 -a 參數。常用的參數為
		<ul><li>
		-a 或 --append<br>
		附加到既有文件的後面，而非覆蓋它。</li><li>
		-i 或 --ignore-interrupts<br>
		忽略中斷信號。
		</li></ul>

		最簡單的是
		<pre># cat file | tee copy1 copy2 copy3</pre>


		tee 命令當想要轉移數據，並不想下次再通過 read 讀取數據時非常有用。如通過 wget 下載文件，並通過 sha1sum 進行校驗，通常使用如下命令。
		<pre>$ wget http://example.com/some.iso && sha1sum some.iso</pre>

		上述方法的問題時，需要一直等到下載完成才能進行校驗，而且通常校驗比較費時。另外，當從網絡下載完後，需要再讀取一次。通常可以下載和計算並行運行。
		<pre>$ wget -O - http://example.com/dvd.iso | tee >(sha1sum > dvd.sha1) > dvd.iso</pre>

		此時可以將文件下載到 dvd.iso 並將校驗的結果保存到 dvd.sha1 中。這裡使用了一個現代的 Shell 常用的一個特性，被稱為進程替換，通常在 zsh、bash、ksh 有效，但是在 /bin/sh 無效，因此對於腳本應該使用 #!/bin/bash 開頭。<br><br>

		上述的命令重定向到一個文件和進程中，另一種比較常用的方法是。
		<pre># wget -O - http://example.com/dvd.iso | tee dvd.iso | sha1sum > dvd.sha1</pre>

		當然可以重定向到兩個進程中去，如分別計算 MD5 和 SHA1 。
		<pre># wget -O - http://example.com/dvd.iso \<br>   | tee >(sha1sum > dvd.sha1) \<br>         >(md5sum > dvd.md5) \<br>         > dvd.iso</pre><br><br>

		這種方法另一個比較有效的應用是：將管道的內容壓縮。如希望通過 GUI 工具分析硬盤的使用情況，在調用 ‘du -ak’ 命令時，會消耗很長時間，且產生的文件比較大，因此不希望保存為壓縮的文件。通常在執行命令時不能打開 GUI 工具，比較低效的方法是
		<pre># du -ak | gzip -9 > /tmp/du.gz<br># gzip -d /tmp/du.gz | xdiskusage -a</pre>

		利用 tee 和進程替換，可以立即打開 GUI 工具，並忽略解壓過程。
		<pre># du -ak | tee >(gzip -9 > /tmp/du.gz) | xdiskusage -a</pre><br><br>

		另外，如果需要使用不同的壓縮方法時，也可以使用該方法。如在發佈一個源碼包時，需要使用不同的壓縮方法。最簡單的方法如下。
		<pre># tardir=your-pkg-M.N<br># tar chof - "$tardir" | gzip  -9 -c > your-pkg-M.N.tar.gz<br># tar chof - "$tardir" | bzip2 -9 -c > your-pkg-M.N.tar.bz2</pre>

		當源碼包很大時，打包和壓縮會很浪費時間，尤其是當使用多核大內存的電腦。最好的方法是並行運行。
		<pre># tardir=your-pkg-M.N<br># tar chof - "$tardir" \<br>    | tee >(gzip -9 -c > your-pkg-M.N.tar.gz) \<br>    | bzip2 -9 -c > your-pkg-M.N.tar.bz2</pre>

		返回 0 ，表示成功；非 0 表示失敗。<br><br><br>

		另一個比較有意思的命令為
		<pre>$ ps -ef | tee >(grep tom >toms-procs.txt) \<br>    >(grep root >roots-procs.txt) \<br>    >(grep -v httpd >not-apache-procs.txt) \<br>    >(sed 1d | awk '{print $1}' >pids-only.txt)</pre>
	</p>

-->




## DUP

如下的兩個函數均為複製一個現存的文件的描述，通常使用這兩個系統調用來重定向一個打開的文件描述符。

{% highlight c %}
#include <unistd.h>
int dup(int fd);
int dup2(int fd1, int fd2);
{% endhighlight %}

由 `dup` 返回的新文件描述符是當前最小的可用文件描述，用 `dup2()` 則可以用 `fd2` 參數指定新的描述符數值，如果 `fd2` 已經打開，則先關閉，若 `fd1=fd2`，則 `dup2` 返回 `fd2`，而不關閉它。

{% highlight c %}
#include <string.h>
#include <stdio.h>

void flush(FILE *stream)
{
	int duphandle;

        /* flush TC's internal buffer */
        fflush(stream);

        /* make a duplicate file handle */
        duphandle = dup(fileno(stream));

        /* close the duplicate handle to flush the buffer */
        close(duphandle);
}

int main(void)
{
        FILE    *fp;
        char    msg[] = "This is a test";

        fp = fopen("DUMMY.FIL","w"); /*create a file*/

        /*write some date to the file*/
        fwrite(msg, strlen(msg), 1, fp);
        printf("Press any key to flush DUMMY.FIL");
        getchar();

        /*flush the data to DUMMY.FIL without closing it*/
        flush(fp);
        printf("\n File was flushed, Press any key to quit:");
        getchar();

        return 0;
}
{% endhighlight %}

### 使用示例

這裡簡單列舉一些常見的方法，將 stdout 定向到文件有 3 種方法：

#### 1. close open

在 open 時操作系統會選擇最小的一個文件描述符，所以可以使用類似如下的方法，不過只適用於單個的重定向。

{% highlight c %}
close(1);       // stdout/1 成為最小的空閒文件描述符
fd = open("/tmp/stdout", O_WRONLY | O_CREAT);
{% endhighlight %}

#### 2. open close dup close

簡單來說操作步驟如下。

{% highlight c %}
fd = open("/tmp/stdout", O_WRONLY | O_CREAT); // 打開需要重定向的文件
close(1);   // 關閉標準輸出，1成為最小描述符
dup(fd);    // 複製文件描述符fd，複製時會使用最小的文件描述符也就是1
close(fd);  // 將原有的文件描述符關閉
{% endhighlight %}

第一次打開文件獲取的描述符非 1 ，因為 1 還在打開著。

#### 3. open dup2 close

這裡使用到了 `dup2()` 函數，可以指定複製的目標文件描述符，如下是其聲明。

{% highlight c %}
#include <unistd.h>
int dup2(oldfd, newfd);
{% endhighlight %}

oldfd 需要複製的文件描述符，newfd 為期望複製 oldfd 後得到的文件描述符，成功則返回 newfd，否則返回 -1 。

如下是簡單將 stdout 重定向到 stdout 文件。

{% highlight c %}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
        int fd;

        fd = open("stdout", O_RDWR | O_CREAT, 0644);
        close(1);
        if (dup2(fd, 1) != 1) {
                fprintf(stderr, "Failed to dup2(%d, 1), %s\n",
                        fd, strerror(errno));
                exit(1);
        }
        printf("Print sth\n");
        fprintf(stderr, "Done running\n");

        return 0;
}
{% endhighlight %}
<!--
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(void)
{
        pid_t pid;
        int fd;

        printf("About to run the who.\n");

        pid = fork();
        if (pid < 0) {
                perror("fork");
                exit(1);
        } else if (pid == 0) { /* child */
                fd = open("userlist", O_RDWR | O_CREAT, 0644);
                close(1);
                if (dup2(fd, 1) != 1) {
                        fprintf(stderr, "Failed to dup2(%d, 1), %s\n",
                                fd, strerror(errno));
                        exit(1);
                }
                execlp("who", "who", NULL);
                perror("execlp"); /* NOT reach */
        }

        wait(0);
        printf("Done running who. results in userlist.\n");

        return 0;
}
-->

如下是通過命名管道 FIFO 將子進程的輸出重定向到父進程，也可以使用匿名管道 PIPE 。

{% highlight c %}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FIFO "stdout_fifo"

int main(void)
{
        /* Make a fifo that redirect the data from stdout of child */
        unlink(FIFO);
        mkfifo(FIFO, 0777);

        /* child process */
        if(fork() == 0) {
                int fd = open(FIFO, O_WRONLY);
                dup2(fd, 1); /* readirect the stdout */
                execl("/bin/ls", "-a", NULL);
        } else { /* parent process */
                char child_stdout[1024] = {0};

                /* Read and print the data from stdout of child */
                int fd = open(FIFO, O_RDONLY), rc;
                rc = read(fd, child_stdout, sizeof(child_stdout));
                child_stdout[rc] = 0;
                printf("Parent: %s\nFinished\n", child_stdout);

                /* Wait for child process */
                wait(NULL);
        }

        return 0;
}
{% endhighlight %}

<!--
子進程將輸出重定向到同一個PIPE中

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
        int fdpipe[2];
        pipe(fdpipe); /* 1: write, 0: read */

        /* child process */
        if(fork() == 0) {
                close(0);
                open("/dev/null", O_RDONLY);
                close(fdpipe[0]);
                dup2(fdpipe[1], 1); /* readirect the stdout */
                dup2(fdpipe[1], 2); /* readirect the stderr */
                close(fdpipe[1]);
                fprintf(stderr, "Write to STDERR\n");
                execl("/bin/ls", "-a", NULL);
        } else { /* parent process */
                char child_stdout[1024] = {0};
                int rc;

                close(fdpipe[1]);
                /* Read and print the data from stdout of child */
                while ((rc = read(fdpipe[0], child_stdout, sizeof(child_stdout) - 1)) > 0) {
                        child_stdout[rc] = 0;
                        puts(child_stdout);
                };

                /* Wait for child process */
                wait(NULL);
        }

        return 0;
}
-->




### 單進程

假設進程 A 擁有一個已打開的文件描述符 fd3，它的狀態如下。

{% highlight text %}
------------
fd0 0   | p0
------------
fd1 1   | p1 -------> 文件表1 ---------> vnode1
------------
fd2 2   | p2
------------
fd3 3   | p3 -------> 文件表2 ---------> vnode2
------------
... ...
... ...
------------
{% endhighlight %}

經調用 `nfd = dup2(fd3, STDOUT_FILENO);` 後進程狀態如下：

{% highlight text %}
------------
fd0 0   | p0
------------
nfd 1   | p1 ------------+
------------             |
fd2 2   | p2             |
------------             V
fd3 3   | p3 -------> 文件表2 ---------> vnode2
------------
... ...
... ...
------------
{% endhighlight %}

如上的函數表示，`nfd` 與 `fd3` 共享一個文件表項，它們的文件表指針指向同一個文件表項，`nfd` 在文件描述符表中的位置為 `STDOUT_FILENO` 的位置，而原先的 `STDOUT_FILENO` 所指向的文件表項被關閉，所以會有如下的特點：

1. 第一個參數必須為已打開的合法 filedes 。
2. 第二個參數可以是任意合法範圍的 filedes 值。<!-- 在Unix其取值區間為[0,255] -->

### 內核結構

每個進程都對應一個結構體 `struct task_struct`，裡面包含一個數據成員：

{% highlight c %}
struct task_struct {
	/* open file information */
	struct files_struct *files;
}
{% endhighlight %}

其中，`struct files_struct` 結構體定義如下：

{% highlight c %}
/* Open file table structure */
struct files_struct {
	/* read mostly part */
	atomic_t count;
	struct fdtable __rcu *fdt;
	struct fdtable fdtab;

	/* written part on a separate cache line in SMP */
        spinlock_t file_lock ____cacheline_aligned_in_smp;
        int next_fd;
        struct embedded_fd_set close_on_exec_init;
        struct embedded_fd_set open_fds_init;
        struct file __rcu * fd_array[NR_OPEN_DEFAULT];
};
{% endhighlight %}

對於上面的兩個句柄 `nfd` `fd3` 他們在 `files_struct` 裡面的數組 `fd_array` 裡面對應的數值是相等的。

<!--
二、重定向後恢復
CU上有這樣一個帖子，就是如何在重定向後再恢復原來的狀態？首先大家都能想到要保存重定向前的文件描述符。那麼如何來保存呢，象下面這樣行麼？
int s_fd = STDOUT_FILENO;
int n_fd = dup2(fd3, STDOUT_FILENO);
還是這樣可以呢？
int s_fd = dup(STDOUT_FILENO);
int n_fd = dup2(fd3, STDOUT_FILENO);
這兩種方法的區別到底在哪呢？答案是第二種方案才是正確的，分析如下：按照第一種方法，我們僅僅在"表面上"保存了相當於fd_t（按照我前面說的理解方法）中的index，

而在調用dup2之後，ptr所指向的文件表項由於計數值已為零而被關閉了，我們如果再調用dup2(s_fd, fd3)  （注意此時s_fd對應的文件表項已經關閉）就會出錯。而第二種方法我們首先做一下複製（這樣對應的文件表項不會被關閉），複製後的狀態如下圖所示:
進程A的文件描述符表(after dup)
[plain] view plain copy
print?

       ------------
    fd0 0   | p0
       ------------
    fd1 1   | p1 ------------- 文件表1 --------- vnode1
       ------------                 /|
    fd2 2   | p2                /
       ------------             /
    fd3 3   | p3 ------------- 文件表2 --------- vnode2
       ------------          /
    s_fd 4   | p4 ------/
       ------------
    ... ...
    ... ...
       ------------

調用dup2後狀態為：
進程A的文件描述符表(after dup2)
[plain] view plain copy
print?

       ------------
    fd0 0   | p0
       ------------
    n_fd 1   | p1 ------------
       ------------               \
    fd2 2   | p2                 \
       ------------                _\|
    fd3 3   | p3 ------------- 文件表2 --------- vnode2
       ------------
    s_fd 4   | p4 -------------文件表1 --------- vnode1
       ------------
    ... ...
    ... ...
       ------------


dup(fd)的語意是返回的新的文件描述符與fd共享一個文件表項。就如after dup圖中的s_fd和fd1共享文件表1一樣。
-->






{% highlight text %}
{% endhighlight %}
