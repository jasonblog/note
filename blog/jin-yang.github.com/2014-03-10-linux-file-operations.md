---
title: Linux 文件操作
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,文件系统
description: Linux 的设计理念是：一切都是文件！在此简单介绍下 Linux 中常见的文件操作。
---

Linux 的设计理念是：一切都是文件！在此简单介绍下 Linux 中常见的文件操作。

<!-- more -->

## 文件

通过 ```ls -l test``` 命令可以查看文件 test 的属性，通常可以得到如下的内容：

![Linux file properties]({{ site.url }}/images/linux/filesystem-file-property-1.jpg "Linux file properties"){: .pull-center}

第二栏表示的是有多少文件连接到 inode ，如果是一个文件，此时这一字段表示这个文件所具有的硬链接数，如果是一个目录，则此字段表示该目录所含子目录的个数，包括 ```.``` 、```..``` 以及隐藏目录。

每个文件都会将他的权限与属性记录到文件系统的 inode 中，不过，我们使用的目录树却是使用文件名来记录，因此每个档名就会连结到一个 i-node 这个属性记录的，就是有多少不同的档名连结到相同的一个 i-node 号码去就是了。

### 文件类型

在 Linux 中，总共有四种文件类型，包括普通文件、目录文件、连接文件和特殊文件，那么可以通过 file 命令来查看。

* 普通文件(regular file)：包括纯文本文件 (ASCII) ，二进制文件 (binary) ，资料格式文件 (data) ， data 主要是指以特定格式存储的文件，如使用 Linux 登录时会将记录存放在 ```/var/log/wtmp``` 中，这就是一个 data file，可以通过 last 来查看，但是不能用 cat 。```ls -l``` 可以看到 ```-``` 。

* 目录文件(directory)：包括文件名、子目录名及其指针。```ls -l``` 可以看到 ```d``` 。

* 连接文件(link)：是指向同一索引节点的那些目录条目，通常为符号链接。```ls -l``` 可以看到 ```l``` 。

* 设备相关文件(device)：这些文件通常放在 ```/dev``` 中，又可以分为块设备文件 (如硬盘) 和字符文件 (如键盘、鼠标)。```ls -l``` 可以分别看到 ```b/c``` 。

* 网络接口文件(sockets)：用于网络上的数据传输，常在 ```/var/run``` 中。```ls -l``` 可以看到 ```s``` 。

* 管道文件(FIFO, pipe)：主要用于数据的读写，如命令管道。```ls -l``` 可以看到 ```p``` 。

### 文件权限

![Linux file properties]({{ site.url }}/images/linux/filesystem-file-property-2.png "Linux file properties"){: .pull-center}

其中第一位表示文件的类型；后九位中，每三位为一组，```r-read/4```、```w-write/2```、```x-excute/1```，分别表示 拥有者、同组用户、其他人的权限。上图表示该文件为目录，拥有者有读、写、执行的权利，而用户组其他成员和其他人没有写的权利。

<!--
* 当用户对某个目录只有读权限时，那么该用户可以列出该目录下的文件列表，即可以使用ls来列出目录下的文件，但是不能进入该目录，即不能cd目录名来进入该目录。同时也不能通过ls -l查看文件属性，将显示为?。</li><br><li>

    当用户对某个目录只有执行权限时，该用户是可以进入该目录的，即可以通过cd来进入该目录；但该用户不能列出这个目录下的文件列表的，也即不能用ls命令列出该目录下的信息。</li><br><li>

    当用户具有写权限时，用户可以在当前目录增加或者删除文件，但需要几个前提：1、需要有可执行权限2、要想删除文件，那么sticky bit位是没有设置的。
-->


只有当文件所在上一级目录有 w 的权限时，才可以新建或者删除文件，对于已经存在的文件可以进行修改。

{% highlight text %}
$ chown -R user:group file/dir             # 修改用户和用户组，-R 表示递归
$ chmod -R 740 file/dir                    # 修改文件或目录的权限，-R 表示递归
$ chmod u/g/o/a +/-/= r/w/x file/dir       # 分别表示user/group/others/all
{% endhighlight %}

文件默认属性可通过 ```umask``` 设置，```umask``` 就是指 "Linux文件的默认属性需要减掉的权限"。

对于 Linux 来说，普通文件的最大默认属性是 666，目录的最大属性是 777；如果不想要用户在新建立文件时使用默认的属性，那么就要设置 umask 值。

在 CentOS 中，系统默认的 umask 值是 0002，那么用户在新建立普通文件时，普通文件的属性就是 666-0002=664，新建目录时，目录的属性就是 777-0002=755。

可以通过如下的命令进行查看设置。

{% highlight text %}
$ umask                  # 查看
$ umaks 0002             # 设置
{% endhighlight %}

### 文件的特殊权限

除了上述的读写执行权限之外，Linux 还定义了其他的权限。

#### SUID

当一个设置了 SUID 位的可执行文件被执行时，无论那个用户来执行该文件，该文件将以所有者的身份运行，都有文件所有者的特权。如果所有者是 root 的话，那么执行人就有超级用户的特权了。

例如 ```/etc/passwd``` 的权限为 ```-rw-r--r-- root root``` ，也就是说只有 root 才能修改，但是我们可以通过 ```/usr/bin/passwd``` 来修改密码。通过 ```ls -l /usr/bin/passwd``` 可以看到其属性为 ```-rwsr-xr-x root root``` 因此在执行时获得了 root 的权限。

**NOTE**: 该属性仅可用在可执行文件，不用于脚本文件和目录(可以进行设置，但是应该无效)。因为脚本时一系列命令的集合体，不同的命令可能会对应于不同的属性。

#### SGID

当一个设置了 SGID 位的可执行文件运行时，该文件将具有所属组的特权， 任意存取整个组所能使用的系统资源。

若目录设置了 SGID ，则所有被复制到这个目录下的文件，其属组都会被重设为和这个目录一样，除非在复制文件时加上 ```-p``` (preserve，保留文件属性) 参数，才能保留原来所属的群组设置。

**NOTE**: 一般来说， SGID 多用在特定的多人团队的项目开发上，在系统中用得较少。

#### 粘着位(Sticky Bit)

只针对目录有效，对文件没有效果。如果用户在该目录下面具有 w 和 x 权限，当用户在该目录下建立文件或者目录时，只有文件拥有者和 root 用户才有权力删除(注意目录的所有者也可以删除文件的)。

#### 设置

可以通过如下的命令进行设置。

{% highlight text %}
$ chmod u+s(SUID) g+s(SGID) o+t(STICKY)
$ chmod 4755
{% endhighlight %}

一个文件的属性通常有 4 个八进制表示，ls 只显示了 3 个，而对于 ```xxx-SUID,SGID,STICKY(4,2,1)```，与 ```rwx``` 相似，也采用一个数字进行表示。

设置后，可以用 ```ls -l``` 来查看，如果本来在该位上有 x (均显示在x位上)，则这些特殊标志显示为小写字母 ```(s, s, t)```；否则，显示为大写字母 ```(S, S, T)```。

对于目录来说如果通过 lsattr 无法查看属性，可以通过 ```ls | lsattr``` 查看；或者直接执行 lsattr。

### 文件特殊属性

文件系统中除了上述的权限配置之外，还可以通过 chattr 进行设置，使用 chattr 必须为 root 用户才可以。

{% highlight text %}
chattr [-R] [-+=] [AacDdijsSu] 文件名
{% endhighlight %}

* A(atime)：如果设置了A属性，则这个文件的最后访问时间atime不能被修改。

* a(append only)：如果设置了a属性，则这个文件只能增加数据，不允许任何进程覆盖或截断这个文件。如果某个目录具有这个属性，那么只能在这个目录下建立和修改文件，而不能删除任何文件。注意，由于vi会创建一些swp文件，因此此时会出错，使用nano。

* d(No dump)：在进行文件系统备份时，dump程序将忽略这个文件。

* i(immutable)：如果设置了i属性，则不能对这个文件做任何修改，包括删除、修改内容等，如果要删除必须去掉该属性。如果某个目录具有这个属性，那么只能修改该目录下的文件，而不能建立和删除文件。这个是针对所有用户，而修改目录的w权限对root无效。

<!--
    c(compressed): 系统以透明的方式压缩这个文件。从这个文件读取时，返回的是解压之后的数据；而向这个文件中写入数据时，数据首先被压缩之后才写入磁盘。</li><br><li>

    s(secure deletion)：如果设置了s属性，则这个文件将从硬盘空间中完全删除，使用0填充所在区域。</li><br><li>

    u(undeletable)：与s完全相反。如果设置了u属性，则这个文件虽然被删除了，但是还在硬盘空间中存在，还可以用来还原恢复。

    （D）synchronous directory updates；（j）data journalling；（S）synchronous updates；（T）and top of directory hierarchy；（t）no tail-merging。-->


### 文件时间属性

在 Linux 中，没有文件创建时间的概念，只有文件的访问时间、修改时间、状态改变时间，也就是说不能知道文件的创建时间。

但如果文件创建后就没有修改过，修改时间 = 创建时间；如果文件创建后，状态就没有改变过，那么状态改变时间 = 创建时间；如果文件创建后，没有被读取过，那么访问时间 = 创建时间，这个基本不太可能。

* modification time(mtime，修改时间)：cp，这个时间指的是文件内容修改的时间，而不是文件属性的修改，当数据内容修改时，这个时间就会改变，用命令 ls -l 默认显示的就是这个时间。

* access time(atime，访问时间)：cp, cat, more，当读取文件内容时，就会更改这个时间，例如使用 cat more vi 等命令，那么该文件的 atime 就会改变，ls stat 不会改变访问时间。注意：可能由于 Linux 缓存机制，第一次读取文件后，下次可能不会访问该文件，因此atime不会改变。

* change time(ctime，属性或位置修改时间)：mv, cp, chmod, chown，当一个文件的状态改变时，这个时间就会改变，例如更改了文件的权限与属性等，它就会改变。

## 其它

### 常用命令

{% highlight text %}
----- 查看文件的类型
$ file /tmp/test

----- 使用stat显示文件详细信息
$ stat file                                    # 显示文件的信息
$ stat -t file                                 # 与上相同，不过在一行显示，方便脚本处理
$ stat -f file                                 # 显示文件系统的信息

----- 显示时间信息
$ ls -l --full-time file                       # 修改时间的完整格式，包含时区
$ ls -l --time=atime/ctime --full-time file    # 显示atime/ctime
{% endhighlight %}

<!--
touch [-actmd] file 用来修改atime和mtime，实际同时也会修改ctime<ul><li>
    -a:如果没有指定时间，则修改atime/ctime；否则atime设置未指定时间，ctime为当今时间。</li><li>
    -m:如果没有指定时间，则修改mtime/ctime；否则mtime设置未指定时间，ctime为当今时间。</li><li>
    -c, --no-create: 不创建任何文件，但同时会修改atime/mtime/ctime</li><li>
    -d:后面可以接日期，也可以使用--date="日期或时间"</li><li>
    -t:后面可以接时间，格式为：[YYMMDDhhmm]</li></ul>
    举例：结果atime和mtime将会改变而ctime不会改变:<br>

touch -d "May 25 3:51 pm" file<br>
touch -d "2 days ago" file<br>
touch -c -m -t 201101110000 file
-->

## 参考

{% highlight text %}
{% endhighlight %}
