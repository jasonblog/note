# Linux gcc链接动态库出错：LIBRARY_PATH和LD_LIBRARY_PATH的区别


昨天在自己的CentOs7.1上写makefile的时候，发现在一个C程序在编译并链接一个已生成好的lib动态库的时候出错。链接命令大概是这样的：


```sh
[root@typecodes tcpmsg]# gcc -o hello main.c -lmyhello
/usr/bin/ld: cannot find -lmyhello
collect2: error: ld returned 1 exit status
```

