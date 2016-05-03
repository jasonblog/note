# 在程式碼中使用 signal() 插入中斷點，讓 GDB 暫停程式做 debugging


天想要用 gdb debug 一個程式，看看某個變數的值，

平常可能可以用 gdb 的中斷點，但有時候想要在某些情況下，才讓 gdb 停在特定地方～

gdb 應該有支援條件式的中斷點，不過另一種方法是直接在程式碼裡喚醒 gdb，

這種方法有時候可以很精準的貼近要 debug 的位置，也算是蠻實用的技巧囉～

  

查了一下，一般常用的是產生一個 SIGINT 中斷，讓 gdb 去攔，

或者是用 asm 指令直接下一個 int 3 的中斷，gdb 一樣會攔到～

舉例來說，我們在下面的程式碼裡面，呼叫了 raise(SIGINT)，產生一個 SIGINT 的中斷：

```c
#include <stdio.h>
#include <signal.h>

int main()
{
    raise(SIGINT);
    int a = 0, b = 1;
    printf("a=%d, b=%d\n", a, b);
    return 0;
}
```

當用 gdb 執行這個程式，跑到 raise(SIGINT) 的地方時，

產生的 SIGINT 中斷就會被 gdb 攔截到：

```sh
(gdb) run
Starting program: /var/app_data/U-Sandbox/u-sandbox/./a.out

Program received signal SIGINT, Interrupt.
0x00007ffff72305d7 in raise () from /lib64/libc.so.6
Missing separate debuginfos, use: debuginfo-install glibc-2.17-

這時 gdb 就已經將程式暫停下來了，不管是要單步執行、檢查變數值都沒問題囉～

```sh
(gdb) n
Single stepping until exit from function raise,
which has no line number information.
main () at test.cpp:7
7		int a = 0, b = 1;
```

##參考資料：

stackoverflow: Set breakpoint in C or C++ code programmatically for gdb on Linux

How to programmatically break into gdb from gcc C++ source
