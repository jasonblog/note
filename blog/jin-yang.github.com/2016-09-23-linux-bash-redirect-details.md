---
title: Bash 重定向
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: bash,redirect,重定向
description: 所谓 IO 重定向简单来说就是一个过程，这个过程捕捉一个文件、命令、程序、脚本、甚至脚本中的代码块的输出，然后把捕捉到的输出，作为输入发送给另外一个文件、命令、程序、或者脚本。这里简单介绍常用命令及其实现。
---

所谓 IO 重定向简单来说就是一个过程，这个过程捕捉一个文件、命令、程序、脚本、甚至脚本中的代码块的输出，然后把捕捉到的输出，作为输入发送给另外一个文件、命令、程序、或者脚本。

这里简单介绍常用命令及其实现。

<!-- more -->


## 重定向

文件描述符 (File Descriptor) 是进程对其所打开文件的索引，形式上是个非负整数。在 Linux 系统中，系统为每一个打开的文件指定一个文件标识符以便系统对文件进行跟踪。

和 C 语言编程里的文件句柄相似，文件标识符是一个数字，不同数字代表不同的含义，默认情况下，系统占用了 3 个，分别是 0 标准输入 (stdin) ， 1 标准输出 (stdout) ， 2 标准错误 (stderr) ，另外 3-9 是保留的标识符，可以把这些标识符指定成标准输入，输出或者错误作为临时连接。通常这样可以解决很多复杂的重定向请求。

### 使用方法

可以简单地用 `<` 或 `>` ，默认相当于使用 `0<` 或 `1>`；管道 (`|` pipe line)，把上一个命令的 stdout 接到下一个命令的 stdin；`tee` 命令的作用是在不影响原本 IO 的情况下，将 stdout 复制一份到文件中去，例如 `cat file.txt | tee backup` 。

重新定义文件标识符可以用 `i>&j` 命令，表示把文件标识符 i 重新定向到 j ，你可以把 `"&"` 理解为 "取地址" 。


### 常见用例

------------------------------

{% highlight text %}
cmd > file
{% endhighlight %}

把 cmd 命令的输出重定向到文件 file 中。如果 file 已经存在，则清空原有文件，使用 bash 的 `noclobber` 选项可以防止覆盖原有文件。

{% highlight text %}
----- 阻止文件重定向操作对一个文件的覆盖，或者使用 set -C
set -o noclobber
----- 恢复文件重定向操作对一个文件的覆盖
set +o noclobber
{% endhighlight %}

------------------------------

{% highlight text %}
cmd >| file
{% endhighlight %}

功能同 `>` ，但即便在设置了 `noclobber` 时也会覆盖 file 文件，注意用的是 `|` 而非一些资料中说的 `!` ，目前仅在 csh 中仍沿用 `>!` 实现这一功能。

<!--
		cmd >> file<br>
        把 cmd 命令的输出重定向到文件 file 中，如果 file 已经存在，则把信息加在原有文件后面。</li><br><li>

		cmd &lt; file<br>
        使 cmd 命令从 file 读入。</li><br><li>

		cmd &lt;&lt; text<br>
        从命令行读取输入，直到一个与 text 相同的行结束。除非使用引号把输入括起来，此模式将对输入内容进行 shell 变量替换。如果使用 &lt;&lt;- ，则会忽略接下来输入行首的 tab ，结束行也可以是一堆 tab 再加上一个与 text 相同的内容。</li><br><li>

		cmd &lt;&lt;&lt; word ???????????????<br>
        把 word（而不是文件 word ）和后面的换行作为输入提供给 cmd 。</li><br><li>

    cmd &lt;&gt; file<br>
        以读写模式把文件 file 重定向到输入，文件 file 不会被破坏。仅当应用程序利用了这一特性时，它才是有意义的。</li><br><li>


		使用文件描述符的重定向 (都使用了 & 符号)<br>
<pre>
cmd &>file      标准输入输出重定向到 file
cmd >&file
cmd >file 2>&1

&>>word         标准输入输出追加到 file ，没有 >>&word 的表达方法
>>word 2>&1

cmd >&n         把输出送到文件描述符 n ，复制文件描述符
cmd m>&n        把输出到文件符 m 的信息重定向到文件描述符 n
cmd >&-         关闭标准输出

cmd <&n         输入来自文件描述符 n ，复制文件描述符
cmd m<&n        m 来自文件描述符 n
cmd <&-         关闭标准输入

cmd <&n-        移动输入文件描述符 n 而非复制它
cmd >&n-        移动输出文件描述符 n 而非复制它</pre></li><br><li>

		关闭文件标识符的操作<br>
	"j&lt;&gt;filename" 命令表示把文件打开，并指明文件标识符为 j ； "&-" 表示关闭文件标识符。
<pre>
n<&- 关闭输入文件标识符 n
0<&- 或 <&- 关闭标准输入 stdin
n>&- 关闭输出文件标识符 n
1>&- 或 >&- 关闭标准输出 stdout
</pre></li><br><li>

		重定向的组合应用<br>
<pre>
cmd 2>file         把文件描述符 2 重定向到 file，即把错误输出存到 file 中。
cmd > file 2>&1    把标准错误重定向到标准输出，再重定向到file，即stderr和stdout都被输出到file中
cmd >> file 2>&1   追加把标准错误重定向到标准输出，再重定向到file，即stderr和stdout都被输出到file中
cmd &> file        功能与上一个相同，更为简便的写法。
cmd >& file        功能仍与上一个相同。
cmd >>& file       追加。
cmd > f1 2>f2      把stdout重定向到f1，而把stderr重定向到f2
tee files          把stdout原样输出的同时，复制一份到files中。
tee files          把stderr和stdout都输出到files中，同时输出到屏幕。</pre>
	</li></ol><br><br><br>


	如果要深刻理解重定向，先要明白以下 2 点。
	<ol type="A"><li>
		shell ( bash 或者 csh 等) 负责重定向，对于程序或者函数，这一切都是透明的，它只管输入输出，至于从哪输入哪输出，是 shell 解释器负责。</li><br><li>

		shell 命令解析过程中，在实际执行命令前， IO 重定向先设置好。
	</li></ol>

-->

> exec 是 Shell 的内建命令，通常用于替换 Shell 执行命令。在重定向中，exec 用来操作文件描述符，当不指定命令时，会修改当前 shell 的文件描述符。



### 示例

{% highlight text %}
# ls /dev 1>filename               # 将标准输出重定向到文件中， "1" 和 ">" 中间没有空格
# ls /dev  >filename               # 与上等价，系统默认是 1 ，因此 1 可以省略
# ls /dev >>filename               # 追加到文件，而非创建
# ls -qw /dev 2>filename           # 将标准错误重定向到文件
# ls /dev &>filename               # 将 stdio/stderr 都输入到文件，"&" 在这里代表 stdio/stderr
# ls /dev >&filename               # 与上同
# ls /dev >filename 2>&1           # 与上同
# ls 2>&1 > filename               # 只有标准输出重定向到 filename

# exec 5>&1                        # 把文件标识符 5 定向到标准输出，通常用来临时保存标准输入
# grep word <filename              # 重定向标准输入，与下面相同
# grep word 0<filename             # 把文件 filename 作为 grep 命令的标准输入，而不是从键盘输入

# echo 123456789 >file             # 把字符串写到文件 file 中
# exec 3<>file                     # 把文件 file 打开，并指定文件标识符为 3 ，默认为 0
# read -n 4 <&3                    # 从文件中读 4 个字符，句柄已经指到第四个字符末尾
# echo -n . >&3                    # 在第 5 个字符处写一个点，覆盖第 5 个字符， -n 表示不换行
# exec 3>&-                        # 关闭文件标识符 3
# cat file                         # file 文件的结果就成了 1234.6789

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
		tee 指令会从标准输入设备读取数据，将其内容输出到标准输出设备，同时保存成文件。如果指定的文件不存在，则创建；有则覆盖，除非使用 -a 参数。常用的参数为
		<ul><li>
		-a 或 --append<br>
		附加到既有文件的后面，而非覆盖它。</li><li>
		-i 或 --ignore-interrupts<br>
		忽略中断信号。
		</li></ul>

		最简单的是
		<pre># cat file | tee copy1 copy2 copy3</pre>


		tee 命令当想要转移数据，并不想下次再通过 read 读取数据时非常有用。如通过 wget 下载文件，并通过 sha1sum 进行校验，通常使用如下命令。
		<pre>$ wget http://example.com/some.iso && sha1sum some.iso</pre>

		上述方法的问题时，需要一直等到下载完成才能进行校验，而且通常校验比较费时。另外，当从网络下载完后，需要再读取一次。通常可以下载和计算并行运行。
		<pre>$ wget -O - http://example.com/dvd.iso | tee >(sha1sum > dvd.sha1) > dvd.iso</pre>

		此时可以将文件下载到 dvd.iso 并将校验的结果保存到 dvd.sha1 中。这里使用了一个现代的 Shell 常用的一个特性，被称为进程替换，通常在 zsh、bash、ksh 有效，但是在 /bin/sh 无效，因此对于脚本应该使用 #!/bin/bash 开头。<br><br>

		上述的命令重定向到一个文件和进程中，另一种比较常用的方法是。
		<pre># wget -O - http://example.com/dvd.iso | tee dvd.iso | sha1sum > dvd.sha1</pre>

		当然可以重定向到两个进程中去，如分别计算 MD5 和 SHA1 。
		<pre># wget -O - http://example.com/dvd.iso \<br>   | tee >(sha1sum > dvd.sha1) \<br>         >(md5sum > dvd.md5) \<br>         > dvd.iso</pre><br><br>

		这种方法另一个比较有效的应用是：将管道的内容压缩。如希望通过 GUI 工具分析硬盘的使用情况，在调用 ‘du -ak’ 命令时，会消耗很长时间，且产生的文件比较大，因此不希望保存为压缩的文件。通常在执行命令时不能打开 GUI 工具，比较低效的方法是
		<pre># du -ak | gzip -9 > /tmp/du.gz<br># gzip -d /tmp/du.gz | xdiskusage -a</pre>

		利用 tee 和进程替换，可以立即打开 GUI 工具，并忽略解压过程。
		<pre># du -ak | tee >(gzip -9 > /tmp/du.gz) | xdiskusage -a</pre><br><br>

		另外，如果需要使用不同的压缩方法时，也可以使用该方法。如在发布一个源码包时，需要使用不同的压缩方法。最简单的方法如下。
		<pre># tardir=your-pkg-M.N<br># tar chof - "$tardir" | gzip  -9 -c > your-pkg-M.N.tar.gz<br># tar chof - "$tardir" | bzip2 -9 -c > your-pkg-M.N.tar.bz2</pre>

		当源码包很大时，打包和压缩会很浪费时间，尤其是当使用多核大内存的电脑。最好的方法是并行运行。
		<pre># tardir=your-pkg-M.N<br># tar chof - "$tardir" \<br>    | tee >(gzip -9 -c > your-pkg-M.N.tar.gz) \<br>    | bzip2 -9 -c > your-pkg-M.N.tar.bz2</pre>

		返回 0 ，表示成功；非 0 表示失败。<br><br><br>

		另一个比较有意思的命令为
		<pre>$ ps -ef | tee >(grep tom >toms-procs.txt) \<br>    >(grep root >roots-procs.txt) \<br>    >(grep -v httpd >not-apache-procs.txt) \<br>    >(sed 1d | awk '{print $1}' >pids-only.txt)</pre>
	</p>

-->




## DUP

如下的两个函数均为复制一个现存的文件的描述，通常使用这两个系统调用来重定向一个打开的文件描述符。

{% highlight c %}
#include <unistd.h>
int dup(int fd);
int dup2(int fd1, int fd2);
{% endhighlight %}

由 `dup` 返回的新文件描述符是当前最小的可用文件描述，用 `dup2()` 则可以用 `fd2` 参数指定新的描述符数值，如果 `fd2` 已经打开，则先关闭，若 `fd1=fd2`，则 `dup2` 返回 `fd2`，而不关闭它。

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

这里简单列举一些常见的方法，将 stdout 定向到文件有 3 种方法：

#### 1. close open

在 open 时操作系统会选择最小的一个文件描述符，所以可以使用类似如下的方法，不过只适用于单个的重定向。

{% highlight c %}
close(1);       // stdout/1 成为最小的空闲文件描述符
fd = open("/tmp/stdout", O_WRONLY | O_CREAT);
{% endhighlight %}

#### 2. open close dup close

简单来说操作步骤如下。

{% highlight c %}
fd = open("/tmp/stdout", O_WRONLY | O_CREAT); // 打开需要重定向的文件
close(1);   // 关闭标准输出，1成为最小描述符
dup(fd);    // 复制文件描述符fd，复制时会使用最小的文件描述符也就是1
close(fd);  // 将原有的文件描述符关闭
{% endhighlight %}

第一次打开文件获取的描述符非 1 ，因为 1 还在打开着。

#### 3. open dup2 close

这里使用到了 `dup2()` 函数，可以指定复制的目标文件描述符，如下是其声明。

{% highlight c %}
#include <unistd.h>
int dup2(oldfd, newfd);
{% endhighlight %}

oldfd 需要复制的文件描述符，newfd 为期望复制 oldfd 后得到的文件描述符，成功则返回 newfd，否则返回 -1 。

如下是简单将 stdout 重定向到 stdout 文件。

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

如下是通过命名管道 FIFO 将子进程的输出重定向到父进程，也可以使用匿名管道 PIPE 。

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
子进程将输出重定向到同一个PIPE中

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




### 单进程

假设进程 A 拥有一个已打开的文件描述符 fd3，它的状态如下。

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

经调用 `nfd = dup2(fd3, STDOUT_FILENO);` 后进程状态如下：

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

如上的函数表示，`nfd` 与 `fd3` 共享一个文件表项，它们的文件表指针指向同一个文件表项，`nfd` 在文件描述符表中的位置为 `STDOUT_FILENO` 的位置，而原先的 `STDOUT_FILENO` 所指向的文件表项被关闭，所以会有如下的特点：

1. 第一个参数必须为已打开的合法 filedes 。
2. 第二个参数可以是任意合法范围的 filedes 值。<!-- 在Unix其取值区间为[0,255] -->

### 内核结构

每个进程都对应一个结构体 `struct task_struct`，里面包含一个数据成员：

{% highlight c %}
struct task_struct {
	/* open file information */
	struct files_struct *files;
}
{% endhighlight %}

其中，`struct files_struct` 结构体定义如下：

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

对于上面的两个句柄 `nfd` `fd3` 他们在 `files_struct` 里面的数组 `fd_array` 里面对应的数值是相等的。

<!--
二、重定向后恢复
CU上有这样一个帖子，就是如何在重定向后再恢复原来的状态？首先大家都能想到要保存重定向前的文件描述符。那么如何来保存呢，象下面这样行么？
int s_fd = STDOUT_FILENO;
int n_fd = dup2(fd3, STDOUT_FILENO);
还是这样可以呢？
int s_fd = dup(STDOUT_FILENO);
int n_fd = dup2(fd3, STDOUT_FILENO);
这两种方法的区别到底在哪呢？答案是第二种方案才是正确的，分析如下：按照第一种方法，我们仅仅在"表面上"保存了相当于fd_t（按照我前面说的理解方法）中的index，

而在调用dup2之后，ptr所指向的文件表项由于计数值已为零而被关闭了，我们如果再调用dup2(s_fd, fd3)  （注意此时s_fd对应的文件表项已经关闭）就会出错。而第二种方法我们首先做一下复制（这样对应的文件表项不会被关闭），复制后的状态如下图所示:
进程A的文件描述符表(after dup)
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

调用dup2后状态为：
进程A的文件描述符表(after dup2)
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


dup(fd)的语意是返回的新的文件描述符与fd共享一个文件表项。就如after dup图中的s_fd和fd1共享文件表1一样。
-->






{% highlight text %}
{% endhighlight %}
