
libev 库的示例程序。

### example.c

简单程序，要么等待5秒超时，或者等待stdin输入字符，例如Ctrl-D、输入字符+回车。


### timer.c

定时器示例程序，包括了三个定时器，默认在初始化时会设置开始时间+重复时间间隔；如果不设置开始时间，则会在开始立即执行一次。

      start at 1492353120
    oneshot at 1492353122
    repeate at 1492353125
    repeate at 1492353126
    repeate at 1492353127
    repeate at 1492353128
    repeate at 1492353129
    repeate at 1492353130
    timeout at 1492353130

### signal.c

等待SIGINT信号，可以通过kill -SIGINT PID发送信号。

### child.c

创建一个子进程，然后主进程中等待子进程退出；而子进程会sleep 1秒后退出。

### filestat.c

监控Makefile文件的状态，可以通过```touch Makefile```修改文件的属性。

### periodic.c

其中示例可以查看上述源码文件中的注释。

### misc.c

输出libev库的版本，然后sleep 2.3秒，主要是一些常用的函数调用。
