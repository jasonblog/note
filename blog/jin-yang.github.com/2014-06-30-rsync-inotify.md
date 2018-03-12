---
title: Rsync & Inotify
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: rsync,inotify
description: rsync 是一个远程数据同步工具，使用 "rsync算法" 来使本地和远程两个主机之间的文件达到同步，该算法只传送两个文件的不同部分；而 inotify 是一种强大的、细粒度的、异步的文件系统事件监控机制。这里介绍下 rsync 和 inotify 的使用。
---

rsync 是一个远程数据同步工具，使用 "rsync算法" 来使本地和远程两个主机之间的文件达到同步，该算法只传送两个文件的不同部分；而 inotify 是一种强大的、细粒度的、异步的文件系统事件监控机制。

这里介绍下 rsync 和 inotify 的使用。

<!-- more -->

## RSYNC

也就是 Remote Synchronize，这是一个在 *nix 下的一个远程数据同步工具，可以通过网络快速同步多台主机间的文件和目录；而且并非每次都整份传输，而是只传输两个文件的不同部分，因此其传输速度相当快。

rsync 的传输效率要比 scp 高很多，其特点有：

* 可镜像保存整个目录树和文件系统；
* 可做到保持原文件的权限、时间、软硬连接等；

如下是常见的数据传输方式。

### 传输方式

这三种传输方式的主要差异是，是否有冒号 (:)，本地传送不需要冒号，通过 ssh 或 rsh 时，需要一个冒号，通过 rsync 传送需要两个冒号。

#### 1. 复制本地文件

在本地直接运行，用法与 cp 几乎一模一样。

{% highlight text %}
$ rsync [OPTION]... SRC DEST

$ rsync -av /etc /tmp              ← 将/etc/的资料备份到/tmp/etc內
{% endhighlight %}

#### 2. 利用ssh

实际上就是使用远程 shell 程序，一般为 rsh 或者 ssh 实现内容的远程复制，通常路径地址包含单个冒号 ":" 分隔符时启动该模式。

{% highlight text %}
$ rsync [OPTION]... SRC [USER@]host:DEST

----- 将remote-server的/etc备份到本地主机/tmp內
$ rsync -ave ssh user@remote-server:/etc /tmp
{% endhighlight %}

#### 3. 使用rsync服务器

通过 rsync 提供的服务来传送，此时 rsync 服务主机会启动 873 端口，也就意味着必须要在服务端启动 rsync 服务，可以查看 /etc/xinetd.d/rsync 服务。

此外，还需要 A) 编辑 /etc/rsyncd.conf 配置文件；B) 需要设置好客户端密码文件。

<!--
### 认证方式

实际上，对应上述的三种传输方式，也就对应了两种认证方式，分别是 rsync-daemon 认证、ssh 认证。

A) rsync-daemon 需要服务端启动rsync服务，默认监听873端口，可以通过配置文件对服务进行配置。
B) ssh 类似于 scp 工具，可通过系统用户进行认证，并不需要启动rsync服务，只需要安装即可。
-->

### 命令行

{% highlight text %}
$ rsync [OPTION]... SRC [USER@]HOST::DEST
$ rsync [OPTION]... rsync://[USER@]HOST[:PORT]/SRC [DEST]
$ rsync -av user@hostname::/dir/path /local/path
{% endhighlight %}

#### 参数列表

常用的参数列表如下。

{% highlight text %}
rsync [-avrlptgoD] [-e ssh] [user@host:/dir] [/local/path]

参数简介：
  --compress/-z, --compress-level=level
    传输过程中是否进行数据压缩，以及指定的压缩级别；
  --archive/-a, --times, --perms, --owner, --group
    用于设置保留原来的更新时间戳、权限、所有者和组信息，注意，对于 -a 同时会设置 --recursive 和 --links，
    相当于 -rlptgoD；
  --recursive/-r VS. --dirs/-d
    前者递归复制所有目录(含子目录)；后者会跳过子目录及其内容；
  --delete
    默认从源复制到目的端时，不删除额外文件，通过该选项保证源和目标目录的内容完全一致，如果源是空，那么会
    导致目的目录内容被删除；
  --links/-l, --hard-links/-H, --copy-links/-L, --copy-unsafe-links, --safe-links
    分别为保持符号链接、保持硬链接、复制符号链接指向的内容而非符号链接本身、如果符号链接指向源目录外则仍
    复制、指向源目录外时为保证安全不复制；
  --verbose, --progress, --stats
    用于显示正在执行的状态，默认不打印任何信息；
  --rsh/-e
    默认使用的就是ssh，通过该参数强制使用，可以通过 --rsh "ssh -p 12345" 指定 ssh 的参数；
  --exclude, --include
    选择要同步的文件，可以指定多次，例如 --exclude "*bak" --exclude "*~" ；
    示例：注意，指定时使用的是源地址的相对路径。
    --exclude "checkout"    某个目录
    --exclude "filename*"   某类文件
    --exclude-from=sync-exclude.list  通过文件指定要忽略的文件
  --update/-u
    增量传输，也就是会跳过所有存在于目标，且时间较新的文件。注意，只检查文件名和最后修改时间，并不检查文
    件大小，如果目的端有1M的A.txt文件，但是更新时间新于源端10M的A.txt那么本地文件不会更新；
  --partial, -P
    断点续传，会保留没有完全传输的文件，以加快随后的再次传输，其中 -P 相当于 --partial --progress；注意，
    该参数与 -u 冲突，会导致传了一半的文件会被 rsync 跳过，详见上面的介绍；

注意: 在指定复制源时，路径是否有最后的 "/" 有不同的含义，例如：
    /data  ：表示将整个 /data 目录复制到目标目录含；
    /data/ ：表示将 /data/ 目录中的所有内容复制到目标目录。
{% endhighlight %}


## inotify

inotify 是基于 inode 级别的文件系统监控技术，是一种强大的、细粒度的、异步的机制。

<!--
sersync

不需要对被监视的目标打开文件描述符，而且如果被监视目标在可移动介质上，那么在 umount 该介质上的文件系统后，被监视目标对应的 watch 将被自动删除，并且会产生一个 umount 事件。
既可以监视文件，也可以监视目录。
使用系统调用而非 SIGIO 来通知文件系统事件。
使用文件描述符作为接口，因而可以使用通常的文件 I/O 操作select 和 poll 来监视文件系统的变化。


IN_ACCESS : 即文件被访问
IN_MODIFY : 文件被 write
IN_ATTRIB : 文件属性被修改，如 chmod、chown、touch 等
IN_CLOSE_WRITE : 可写文件被 close
IN_CLOSE_NOWRITE : 不可写文件被 close
IN_OPEN : 文件被open
IN_MOVED_FROM : 文件被移走,如 mv
IN_MOVED_TO : 文件被移来，如 mv、cp
IN_CREATE : 创建新文件
IN_DELETE : 文件被删除，如 rm
IN_DELETE_SELF : 自删除，即一个可执行文件在执行时删除自己
IN_MOVE_SELF : 自移动，即一个可执行文件在执行时移动自己
IN_UNMOUNT : 宿主文件系统被 umount
IN_CLOSE : 文件被关闭，等同于(IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)
IN_MOVE : 文件被移动，等同于(IN_MOVED_FROM | IN_MOVED_TO)


/proc/sys/fs/inotify/max_queued_events 默认值: 16384 该文件中的值为调用inotify_init时分配给inotify instance中可排队的event的数目的最大值，超出这个值得事件被丢弃，但会触发IN_Q_OVERFLOW事件

/proc/sys/fs/inotify/max_user_instances 默认值: 128 指定了每一个real user ID可创建的inotify instatnces的数量上限

/proc/sys/fs/inotify/max_user_watches 默认值: 8192 指定了每个inotify instance相关联的watches的上限

注意: max_queued_events 是 Inotify 管理的队列的最大长度，文件系统变化越频繁，这个值就应该越大
如果你在日志中看到Event Queue Overflow，说明max_queued_events太小需要调整参数后再次使用.
-->

inotify-tools 提供了一个命令行管理的命令，可以查看 [Github inotify-tools](https://github.com/rvoicilas/inotify-tools) ，也可以通过 [Github Wiki](https://github.com/rvoicilas/inotify-tools/wiki) 查看下载最新版本，此时会安装 inotifywait 和 inotifywatch 两个文件。

{% highlight text %}
$ tar -zxf inotify-tools-3.14.tar.gz && cd inotify-tools-3.14
$ ./configure --prefix=/usr && make && su -c 'make install'
# mv /lib/libinotifytools.* /lib64/
{% endhighlight %}


<!--
inotifywait可以在当事件发生一次时就退出，或者一直执行。
<ul><li>
    @&lt;file&gt;<br>
    当监控一个目录时，可以设置不监控某个文件，可以使用该选项去除一些文件。</li><br><li>

    --fromfile<br>
    从文件读取需要监视的文件或排除的文件，一个文件一行，排除的文件以@开头。</li><br><li>

    -e , --event<br>
    指定监视的事件。</li><br><li>

    -m, --monitor<br>
    接收到一个事情而不退出，无限期地执行。默认的行为是接收到一个事情后立即退出。</li><br><li>

    -d, --daemon<br>
    跟--monitor一样，除了是在后台运行，需要指定--outfile把事情输出到一个文件。也意味着使用了--syslog。</li><br><li>

    -o, --outfile<br>
    输出事情到一个文件而不是标准输出。</li><br><li>

    -s, --syslog<br>
    输出错误信息到系统日志。</li><br><li>

    -r, --recursive<br>
    监视一个目录下的所有子目录。</li><br><li>

    -q, --quiet<br>
    指定一次，不会输出详细信息，指定二次，除了致命错误，不会输出任何信息。</li><br><li>

    --timefmt<br>
    指定时间格式，用于--format选项中的%T格式。</li><br><li>

    --format<br>
    指定输出格式。 %w 表示发生事件的目录 %f 表示发生事件的文件 %e 表示发生的事件 %Xe 事件以“X"分隔 %T 使用由--timefmt定义的时间格式</li><br><li>


--exclude
正则匹配需要排除的文件，大小写敏感。
--excludei
正则匹配需要排除的文件，忽略大小写。
-t , --timeout
设置超时时间，如果为0，则无限期地执行下去。
-c, --csv
输出csv格式。

access  文件读取
modify  文件更改。
attrib  文件属性更改，如权限，时间戳等。
close_write     以可写模式打开的文件被关闭，不代表此文件一定已经写入数据。
close_nowrite   以只读模式打开的文件被关闭。
close   文件被关闭，不管它是如何打开的。
open    文件打开。
moved_to    一个文件或目录移动到监听的目录，即使是在同一目录内移动，此事件也触发。
moved_from  一个文件或目录移出监听的目录，即使是在同一目录内移动，此事件也触发。
move    包括moved_to和 moved_from
move_self   文件或目录被移除，之后不再监听此文件或目录。
create  文件或目录创建
delete  文件或目录删除
delete_self     文件或目录移除，之后不再监听此文件或目录
unmount     文件系统取消挂载，之后不再监听此文件系统。

<ul><li>
    监控 java 目录，通过 cat test/foo 测试。
    <pre>$ inotifywait test</pre></li><br><li>

    等待httpd相关的信息。
<pre>
#!/bin/sh
while inotifywait -e modify /var/log/messages; do
    if tail -n1 /var/log/messages | grep httpd; then
        kdialog --msgbox "Apache needs love!"
    fi
done
</pre></li><br><li>

    监控~/test，同时执行如下命 touch ~/test/badfile、touch ~/test/goodfile, rm ~/test/badfile 。
    <pre>$ inotifywait -m -r --format '%:e %f' ~/test</pre></li><br><li>


#!/bin/bash
src=/var/www
des=backup@192.168.1.200::web
/usr/local/bin/inotifywait -mrq --timefmt '%d/%m/%y %H:%M' --format '%T %w%f' \
-e modify,delete,create,attrib ${src} \
| while read x
    do
        /usr/bin/rsync -avz --delete --progress $src $des --password-file=/root/rsyncpass &&
        echo "$x was rsynced" &lt;&lt; /var/log/rsync.log
    done


注释：
inotifywait
-m：保持监听事件。
-r：递归查看目录。
-q：打印出事件。
-e modify,delete,create,attrib：监听写入，删除，创建，属性改变事件。

rsync
-a：存档模式，相当于使用-rlptgoD。
-v：详细模式输出。
-z：传输过程中压缩文件。

为脚本加执行权限：

chmod +x /root/rsync.sh

在rc.local加入自启动：



<pre>
#!/bin/bash
host=192.168.1.15
src=/tmp/
des=web
user=webuser
inotifywait -mrq --timefmt '%d/%m/%y %H:%M' --format '%T %w%f%e' -e modify,delete,create,attrib $src while read files
do
/usr/bin/rsync -vzrtopg --delete --progress --password-file=/usr/local/rsync/rsync.passwd $src $user@$host::$des
echo "${files} was rsynced" &gt;&gt;/tmp/rsync.log 2&gt;&amp;1
done</pre>

while inotifywait -qq -e create --exclude '\.(swp|txt|tmp)' /pis/src; do
    sleep 1
    gcc /pis/src/main.cpp -o /pis/bin/pistat
    chmod +x /pis/bin/*
done
</li></ul>
</p>


http://www.ttlsa.com/web/let-infotify-rsync-fast/

sersync

-->


















{% highlight text %}
{% endhighlight %}
