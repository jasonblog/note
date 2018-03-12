---
title: Python 与 Curses
layout: post
comments: true
language: chinese
category: [python,program]
keywords: python,ncurse,curse
description:
---

<!-- more -->


<!--
如果你有许多的窗口对象(都需要刷新), 为了避免不必要的闪烁, 你可以先对各个需要刷新的窗口调用 noutrefresh(), 它将升级内在的数据结构使之匹配你所要的内容, 然后统一调用 doupdate() 来刷新屏幕.

通常，如果有 6 个参数 (pad 的 refresh 函数)，其参数及其含义如下。

window.refresh([pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol])

pminrow, pmincol
    左上角的位置；

The 6 optional arguments can only be specified when the window is a pad created with newpad(). The additional parameters are needed to indicate what part of the pad and screen are involved. pminrow and pmincol specify the upper left-hand corner of the rectangle to be displayed in the pad.

sminrow, smincol, smaxrow, and smaxcol specify the edges of the rectangle to be displayed on the screen.
The lower right-hand corner of the rectangle to be displayed in the pad is calculated from the screen coordinates, since the rectangles must be the same size. Both rectangles must be entirely contained within their respective structures. Negative values of pminrow, pmincol, sminrow, or smincol are treated as if they were zero.
-->

## curses

在 C 提供的库中，除了提供标准库之外，还包括了如下的内容。

{% highlight text %}
/usr/lib64/libform.so.5
/usr/lib64/libformw.so.5
/usr/lib64/libmenu.so.5
/usr/lib64/libmenuw.so.5
/usr/lib64/libncurses++.so.5
/usr/lib64/libncurses++w.so.5
/usr/lib64/libncurses.so.5
/usr/lib64/libncursesw.so.5
/usr/lib64/libpanel.so.5
/usr/lib64/libpanelw.so.5
/usr/lib64/libtic.so.5
/usr/lib64/libtinfo.so.5
{% endhighlight %}

除了上述的库之外，可以从 [github mirro](https://github.com/mirror/ncurses) 下载代码，其中包含了很多的测试或者示例代码。

### curses.panel

如下是一个简单的示例，会绘制两个 panel ，而且第二个 panel 会自动移动。

{% highlight python %}
from time import sleep
import curses, curses.panel

def make_panel(h, l, y,x, str):
    win = curses.newwin(h, l, y, x)
    win.erase()
    win.box()
    win.addstr(2, 2, str)

    panel = curses.panel.new_panel(win)
    return win, panel

def test(stdscr):
    try:
        curses.curs_set(0)
    except:
        pass

    stdscr.box()
    stdscr.addstr(2, 2, "panels everywhere")
    win1, panel1 = make_panel(10,12, 5,5, "Panel 1")
    win2, panel2 = make_panel(10,12, 8,8, "Panel 2")
    curses.panel.update_panels(); stdscr.refresh()
    sleep(1)

    panel1.top(); curses.panel.update_panels(); stdscr.refresh()
    sleep(1)

    for i in range(20):
        panel2.move(8, 8+i)
        curses.panel.update_panels(); stdscr.refresh()
        sleep(0.1)

    sleep(1)

if __name__ == '__main__':
    curses.wrapper(test)
{% endhighlight %}

## 异步程序

默认来说，当从键盘获取输入时，会阻塞读取，如下一步步介绍异步实现调用。

### 同步程序

如下是一个同步程序。

{% highlight c %}
#include <stdlib.h>
#include <curses.h>

int main(void)
{
  initscr();
  crmode();
  noecho();
  clear();

  char c;
  while((c = getch()) != 'q'){
    if(c == 'w')
      mvaddch(20, 20, '!');
    else if(c == 'e')
      mvaddch(20, 20, '0');
    else if(c == 'r')
      mvaddch(20, 20, 't');
  }

  move(20, 21);
  addstr("do other thing");
  getch();

  endwin();
  return EXIT_SUCCESS;
}
{% endhighlight %}

上述程序会一直阻塞在 `getch()` 函数中，当输入 `q` 后会退出 `while` 循环，然后才执行 `addstr()` 函数。如果要实现异步调用，可以通过 A) 设置输入`O_ASYNC`位；B) 使用 `aio_read()` 实现。

### O_ASYNC

接下来，看看如何通过设置 `O_ASYNC` 标志位来实现异步读取。

{% highlight c %}
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <sys/types.h>

void on_input (  )
{
  char c;
  c = getch();
  if(c == 'w')
    mvaddch(20, 20, '!');
  else if(c == 'e')
    mvaddch(20, 20, '0');
  else if(c == 'r')
    mvaddch(20, 20, 't');
}

int main(void)
{
  initscr();
  crmode();
  noecho();
  clear();

  int fd_flags;
  fcntl(0, F_SETOWN, getpid());            // 当输入就绪的时候发送信号
  fd_flags = fcntl(0, F_GETFL);
  fcntl(0, F_SETFL, (fd_flags | O_ASYNC)); // 设置输入为O_ASYNC
  signal(SIGIO, on_input);                 // IO信号回调函数

  while(1){
    move(20, 22);
    addstr("do other thing");
    refresh();
    sleep(1);
  }

  endwin();
  return EXIT_SUCCESS;
}
{% endhighlight %}

上面这个例子可以看到，会先输出 `do other thing` 字符串，也就是运行 `addstr()` 函数，同时在你输入字符程序可以通过信号处理函数 `on_input` 来接受输入进行响应，从而达到了异步的效果。

### aio_read()

异步输入的第二种方法是通过 `aio_read()` 实现，相比来说更加灵活，但是设置起来也比较复杂，其设置步骤如下：
  1. 设置信号处理函数，接受用户输入；
  2. 设置 `struct aiocb` 中的变量指明等待什么类型的输入，当输入的时候产生什么信号；
  3. 将 `struct aiocb` 传递给 `aio_read()` 来递交读入请求。

其中 `struct aiocb` 定义如下:

{% highlight c %}
struct aiocb {
  /* The order of these fields is implementation-dependent */
  int             aio_fildes;     /* File descriptor */
  off_t           aio_offset;     /* File offset */
  volatile void  *aio_buf;        /* Location of buffer */
  size_t          aio_nbytes;     /* Length of transfer */
  int             aio_reqprio;    /* Request priority */
  struct sigevent aio_sigevent;   /* Notification method */
  int             aio_lio_opcode; /* Operation to be performed; lio_listio() only */
  /* Various implementation-internal fields not shown */
};
struct sigevent {
    int          sigev_notify; /* Notification method */
    int          sigev_signo;  /* Notification signal */
    union sigval sigev_value;  /* Data passed with notification */
    void       (*sigev_notify_function) (union sigval);
                     /* Function used for thread notification (SIGEV_THREAD) */
    void        *sigev_notify_attributes;
                     /* Attributes for notification thread (SIGEV_THREAD) */
    pid_t        sigev_notify_thread_id;
                     /* ID of thread to signal (SIGEV_THREAD_ID) */
};
{% endhighlight %}

下面是一个简单的例子:

{% highlight c %}
#include <aio.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>

struct aiocb kbcbuf;

void on_input (  )
{
  char c, *cp = (char *)kbcbuf.aio_buf;
  if(aio_error(&kbcbuf) != 0) {
    perror("reading faild");
  } else if(aio_return(&kbcbuf) ==1) {
    c = *cp;
    if(c == 'w')
      mvaddch(20, 20, '!');
    else if(c == 'e')
      mvaddch(20, 20, '0');
    else if(c == 'r')
      mvaddch(20, 20, 't');
  }
  aio_read(&kbcbuf);
}

int main(void)
{
  initscr();
  crmode();
  noecho();
  clear();

  signal(SIGIO, on_input);

  static char input[1];
  kbcbuf.aio_fildes = 0;
  kbcbuf.aio_offset = 0;
  kbcbuf.aio_buf = input; // 设置接受输入的buf
  kbcbuf.aio_nbytes = 1;  // 设置接受输入的字节大小
  // 设置处理输入的方法，SIGE_SIGNAL代表通过发送信号来处理
  kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  // 设置要发送的信号
  kbcbuf.aio_sigevent.sigev_signo = SIGIO;

  aio_read(&kbcbuf);

  while(1){
    move(20, 22);
    addstr("do other thing");
    refresh();
    sleep(1);
  }

  endwin();
  return EXIT_SUCCESS;
}
{% endhighlight %}

可以通过 `gcc -o main main.c -Wall -lncurses -lrt` 编译。

### 异步函数

实际上，也可以直接使用类似 select()、poll()、epoll() 函数。


## 参考

可以查看下官方的介绍文档 [Curses Programming with Python](https://docs.python.org/2/howto/curses.html)，以及 Python 中的 curses 模块，可查看 [Terminal handling for character-cell displays](https://docs.python.org/2/library/curses.html) 。

<!--
Terminal handling for character-cell displays
https://docs.python.org/dev/library/curses.html
-->

关于 C 语言中 ncurses 的相关内容可以参考 [NCURSES Programming HOWTO](http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/) 文档，以及官方网站 [www.gnu.org](https://www.gnu.org/software/ncurses/) 中的介绍。

<!--
A panel stack extension for curses
https://docs.python.org/dev/library/curses.panel.html

Simplified curses
https://pypi.python.org/pypi/cursed

Ncurses Programming Guide
hughm.cs.ukzn.ac.za/~murrellh/os/notes/ncurses.html

http://www.cnblogs.com/nzhl/p/5603600.html

libev+ncurse
https://lists.gnu.org/archive/html/bug-ncurses/2015-06/msg00046.html
ncurse贪吃蛇
http://www.cnblogs.com/eledim/p/4857557.html

http://www.cnblogs.com/starof/p/4703820.html


https://stackoverflow.com/questions/7738546/how-to-set-a-timeout-for-a-function-in-c
https://github.com/tony/NCURSES-Programming-HOWTO-examples
https://github.com/ffainelli/bqlmon
https://github.com/mazarf/editor
https://github.com/weechat/weechat
https://github.com/ulfalizer/readline-and-ncurses
http://blog.chinaunix.net/uid-29547110-id-5047281.html

https://github.com/JFreegman/toxic
https://github.com/boothj5/profanity
https://github.com/rofl0r/ncdu
https://github.com/wereHamster/ncurses
https://github.com/FedeDP/ncursesFM
https://github.com/jubalh/nudoku
https://github.com/jvns/snake
https://cmake.org/cmake/help/v3.0/command/configure_file.html

http://keyvanfatehi.com/2011/08/02/Asynchronous-c-programs-an-event-loop-and-ncurses/

BSD Games is a collection of the classic text based games distributed with *BSD
http://wiki.linuxquestions.org/wiki/BSD_games

有趣！10个你不得不知的Linux终端游戏 
http://www.freebuf.com/articles/others-articles/124743.html
-->

{% highlight text %}
{% endhighlight %}
