# ldconfig及 LD_LIBRARY_PATH,LD_DEBUG ,LD_PRELOAD



1. 往/lib和/usr/lib里面加东西，是不用修改/etc/ld.so.conf的，但是完了之后要调一下ldconfig，不然这个library会找不到

2. 想往上面两个目录以外加东西的时候，一定要修改/etc/ld.so.conf，然后再调用ldconfig，不然也会找不到
比 如安装了一个mysql到/usr/local/mysql，mysql有一大堆library在/usr/local/mysql/lib下面，这时就 需要在/etc/ld.so.conf下面加一行/usr/local/mysql/lib，保存过后ldconfig一下，新的library才能在程 序运行时被找到。

3. 如果想在这两个目录以外放lib，但是又不想在/etc/ld.so.conf中加东西（或者是没有权限加东西）。那也可以，就是export一个全局变 量LD_LIBRARY_PATH，然后运行程序的时候就会去这个目录中找library。一般来讲这只是一种临时的解决方案，在没有权限或临时需要的时 候使用。

4. ldconfig做的这些东西都与运行程序时有关，跟编译时一点关系都没有。编译的时候还是该加-L就得加，不要混淆了。

5. 总之，就是不管做了什么关于library的变动后，最好都ldconfig一下，不然会出现一些意想不到的结果。不会花太多的时间，但是会省很多的事。



##几个特殊的环境变量：
 
###LD_DEBUG的用法：

```sh 
[root@ljj c_c++]# LD_DEBUG=help ls
Valid options for the LD_DEBUG environment variable are:
  libs        display library search paths
  reloc       display relocation processing
  files       display progress for input file
  symbols     display symbol table processing
  bindings    display information about symbol binding
  versions    display version dependencies
  all         all previous options combined
  statistics  display relocation statistics
  unused      determined unused DSOs
  help        display this help message and exit
To direct the debugging output into a file instead of standard output
a filename can be specified using the LD_DEBUG_OUTPUT environment variable.
[root@ljj c_c++]#`
```

```sh
Linux支持共享库已经有悠久的历史了，不再是什么新概念了。大家都知道如何编译、连接以及动态加载(dlopen/dlsym/dlclose) 共享库。但是，可能很多人，甚至包括一些高手，对共享库相关的一些环境变量认识模糊。当然，不知道这些环境变量，也可以用共享库，但是，若知道它们，可能就会用得更好。下面介绍一些常用的环境变量，希望对家有所帮助：

 

LD_LIBRARY_PATH 这个环境变量是大家最为熟悉的，它告诉loader：在哪些目录中可以找到共享库。可以设置多个搜索目录，这些目录之间用冒号分隔开。在linux下，还提供了另外一种方式来完成同样的功能，你可以把这些目录加到/etc/ld.so.conf中，或则在/etc/ld.so.conf.d里创建一个文件，把目录加到这个文件里。当然，这是系统范围内全局有效的，而环境变量只对当前shell有效。按照惯例，除非你用上述方式指明，loader是不会在当前目录下去找共享库的，正如shell不会在当前目前找可执行文件一样。

 

LD_PRELOAD 这个环境变量对于程序员来说，也是特别有用的。它告诉loader：在解析函数地址时，优先使用LD_PRELOAD里指定的共享库中的函数。这为调试提供了方便，比如，对于C/C++程序来说，内存错误最难解决了。常见的做法就是重载malloc系列函数，但那样做要求重新编译程序，比较麻烦。使用LD_PRELOAD机制，就不用重新编译了，把包装函数库编译成共享库，并在LD_PRELOAD加入该共享库的名称，这些包装函数就会自动被调用了。在linux下，还提供了另外一种方式来完成同样的功能，你可以把要优先加载的共享库的文件名写在/etc/ld.so.preload里。当然，这是系统范围内全局有效的，而环境变量只对当前shell有效。

 

LD_ DEBUG 这个环境变量比较好玩，有时使用它，可以帮助你查找出一些共享库的疑难杂症（比如同名函数引起的问题）。同时，利用它，你也可以学到一些共享库加载过程的知识。它的参数如下：

  libs display library search paths

  reloc display relocation processing

  files display progress for input file

  symbols display symbol table processing

  bindings display information about symbol binding

  versions display version dependencies

  all all previous options combined

  statistics display relocation statistics

  unused determined unused DSOs

  help display this help message and exit


BIND_NOW 这个环境变量与dlopen中的flag的意义是一致，只是dlopen中的flag适用于显示加载的情况，而BIND_NOW/BIND_NOT适用于隐式加载。

 

LD_PROFILE/LD_PROFILE_OUTPUT：为指定的共享库产生profile数据，LD_PROFILE指定共享库的名称，LD_PROFILE_OUTPUT指定输出profile文件的位置，是一个目录，且必须存在，默认的目录为/var/tmp/或/var/profile。通过profile数据，你可以得到一些该共享库中函数的使用统计信息。
```


Linux 运行的时候，是如何管理共享库(*.so)的？在 Linux 下面，共享库的寻找和加载是由 /lib/ld.so 实现的。 ld.so 在标准路经(/lib, /usr/lib) 中寻找应用程序用到的共享库。


但是，如果需要用到的共享库在非标准路经，ld.so 怎么找到它呢？


目前，Linux 通用的做法是将非标准路经加入 /etc/ld.so.conf，然后运行 ldconfig 生成 /etc/ld.so.cache。 ld.so 加载共享库的时候，会从 ld.so.cache 查找。


传 统上， Linux 的先辈 Unix 还有一个环境变量 - LD_LIBRARY_PATH 来处理非标准路经的共享库。ld.so 加载共享库的时候，也会查找这个变量所设置的路经。但是，有不少声音主张要避免使用 LD_LIBRARY_PATH 变量，尤其是作为全局变量。这些声音是：


* LD_LIBRARY_PATH is not the answer - http://prefetch.net/articles/linkers.badldlibrary.html

* Why LD_LIBRARY_PATH is bad - http://xahlee.org/UnixResource_dir/_/ldpath.html 

* LD_LIBRARY_PATH - just say no - http://blogs.sun.com/rie/date/20040710
解决这一问题的另一方法是在编译的时候通过 -R<path> 选项指定 run-time path。


```sh
1. 往/lib和/usr/lib里面加东西，是不用修改/etc/ld.so.conf的，但是完了之后要调一下ldconfig，不然这个library会找不到


2. 想往上面两个目录以外加东西的时候，一定要修改/etc/ld.so.conf，然后再调用ldconfig，不然也会找不到
比如安装了一个mysql到/usr/local/mysql，mysql有一大堆library在/usr/local/mysql/lib下面，这时就需要在/etc/ld.so.conf下面加一行/usr/local/mysql/lib，保存过后ldconfig一下，新的library才能在程序运行时被找到。


3. 如果想在这两个目录以外放lib，但是又不想在/etc/ld.so.conf中加东西（或者是没有权限加东西）。那也可以，就是export一个全局变量LD_LIBRARY_PATH，然后运行程序的时候就会去这个目录中找library。一般来讲这只是一种临时的解决方案，在没有权限或临时需要的时候使用。


4. ldconfig做的这些东西都与运行程序时有关，跟编译时一点关系都没有。编译的时候还是该加-L就得加，不要混淆了。


5. 总之，就是不管做了什么关于library的变动后，最好都ldconfig一下，不然会出现一些意想不到的结果。不会花太多的时间，但是会省很多的事。

```
http://hi.baidu.com/fanzier/blog/item/222ba3ec74cb9a2463d09fb5.html




LD_LIBRARY_PATH 这个环境变量是大家最为熟悉的，它告诉loader：在哪些目录中可以找到共享库。可以设置多个搜索目录，这些目录之间用冒号分隔开。在linux下，还提供了另外一种方式来完成同样的功能，你可以把这些目录加到/etc/ld.so.conf中，或则在/etc/ld.so.conf.d里创建一个文件，把目录加到这个文件里。当然，这是系统范围内全局有效的，而环境变量只对当前shell有效。按照惯例，除非你用上述方式指明，loader是不会在当前目录下去找共享库的，正如shell不会在当前目前找可执行文件一样。
 


LD_DEBUG 是 glibc 中的 loader 为了方便自身调试而设置的一个环境变量。通过设置这个环境变量，可以方便的看到 loader 的加载过程。 以在 64位 centos 上编译安装 memcached 为例，安装阶段顺利，执行 memcached 命令时出现错误：

```sh
#memcached –h
/usr/local/memcached/bin/memcached: error while loading shared libraries: libevent-1.4.so.2:
cannot open shared object file: No such file or directory
#find / -name libevent-1.4.so.2
/usr/lib/libevent-1.4.so.2
```

##设置 LD_DEBUG 变量获得更多信息:

```sh
#LD_DEBUG=help ls
Valid options for the LD_DEBUG environment variable are:

  libs        display library search paths
  reloc       display relocation processing
  files       display progress for input file
  symbols     display symbol table processing
  bindings    display information about symbol binding
  versions    display version dependencies
  all         all previous options combined
  statistics  display relocation statistics
  unused      determined unused DSOs
  help        display this help message and exit

To direct the debugging output into a file instead of standard output
a filename can be specified using the LD_DEBUG_OUTPUT environment variable.
```

##库依赖的问题，使用 libs 参数:

```sh
#LD_DEBUG=libs /usr/local/memcached/bin/memcached –h
9808:      find library=libevent-1.4.so.2 [0]; searching
9808:      search cache=/etc/ld.so.cache
9808:      search path=/lib64/tls/x86_64:/lib64/tls:/lib64/x86_64:/lib64:/usr/lib64/tls/x86_64
/usr/lib64/tls:/usr/lib64/x86_64:/usr/lib64
(system search path)
9808:      trying file=/lib64/tls/x86_64/libevent-1.4.so.2
9808:      trying file=/lib64/tls/libevent-1.4.so.2
9808:      trying file=/lib64/x86_64/libevent-1.4.so.2
9808:      trying file=/lib64/libevent-1.4.so.2
9808:      trying file=/usr/lib64/tls/x86_64/libevent-1.4.so.2
9808:      trying file=/usr/lib64/tls/libevent-1.4.so.2
9808:      trying file=/usr/lib64/x86_64/libevent-1.4.so.2
9808:      trying file=/usr/lib64/libevent-1.4.so.2
9808:      /usr/local/memcached/bin/memcached: error while loading shared libraries: libevent-1.4.so.2: cannot open shared object file: No such file or directory
```


