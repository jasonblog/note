# GDB调试原理 ptrace系统调用


- gdb基本上大家都在用，你有没有想过它的实现原理是什么？为什么它可以控制程序执行、中断、访问内存甚至直接使程序流程改变？

- 在使用gdb调试程序时，程序的进程状态是”T”，但又似乎并非接到了SIGSTOP信号，那么这个”T”是什么呢？




追根溯源，我们今天来研究一下Linux下这个强大的系统调用：ptrace()


首先，linux的进程状态大体分为以下几种：

```sh
D (TASK_UNINTERRUPTIBLE)，不可中断的睡眠状态。
R (TASK_RUNNING)，进程执行中。
S (TASK_INTERRUPTIBLE)，可中断的睡眠状态。
T (TASK_STOPPED)，暂停状态。
t (TASK_TRACED)，进程被追踪。
w (TASK_PAGING)，进程调页中，2.6以上版本的内核中已经被移除。
X (TASK_DEAD – EXIT_DEAD)，退出状态，进程即将被销毁。
Z (TASK_DEAD – EXIT_ZOMBIE)，退出状态，进程成为僵尸进程。
```
 (以上内容来自ps命令的manual手册，原文请看↓)
 
 