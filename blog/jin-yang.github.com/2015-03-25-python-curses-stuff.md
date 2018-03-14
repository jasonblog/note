---
title: Python 與 Curses
layout: post
comments: true
language: chinese
category: [python,program]
keywords: python,ncurse,curse
description:
---

<!-- more -->


<!--
如果你有許多的窗口對象(都需要刷新), 為了避免不必要的閃爍, 你可以先對各個需要刷新的窗口調用 noutrefresh(), 它將升級內在的數據結構使之匹配你所要的內容, 然後統一調用 doupdate() 來刷新屏幕.

通常，如果有 6 個參數 (pad 的 refresh 函數)，其參數及其含義如下。

window.refresh([pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol])

pminrow, pmincol
    左上角的位置；

The 6 optional arguments can only be specified when the window is a pad created with newpad(). The additional parameters are needed to indicate what part of the pad and screen are involved. pminrow and pmincol specify the upper left-hand corner of the rectangle to be displayed in the pad.

sminrow, smincol, smaxrow, and smaxcol specify the edges of the rectangle to be displayed on the screen.
The lower right-hand corner of the rectangle to be displayed in the pad is calculated from the screen coordinates, since the rectangles must be the same size. Both rectangles must be entirely contained within their respective structures. Negative values of pminrow, pmincol, sminrow, or smincol are treated as if they were zero.
-->

## curses

在 C 提供的庫中，除了提供標準庫之外，還包括瞭如下的內容。

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

除了上述的庫之外，可以從 [github mirro](https://github.com/mirror/ncurses) 下載代碼，其中包含了很多的測試或者示例代碼。

### curses.panel

如下是一個簡單的示例，會繪製兩個 panel ，而且第二個 panel 會自動移動。

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

## 異步程序

默認來說，當從鍵盤獲取輸入時，會阻塞讀取，如下一步步介紹異步實現調用。

### 同步程序

如下是一個同步程序。

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

上述程序會一直阻塞在 `getch()` 函數中，當輸入 `q` 後會退出 `while` 循環，然後才執行 `addstr()` 函數。如果要實現異步調用，可以通過 A) 設置輸入`O_ASYNC`位；B) 使用 `aio_read()` 實現。

### O_ASYNC

接下來，看看如何通過設置 `O_ASYNC` 標誌位來實現異步讀取。

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
  fcntl(0, F_SETOWN, getpid());            // 當輸入就緒的時候發送信號
  fd_flags = fcntl(0, F_GETFL);
  fcntl(0, F_SETFL, (fd_flags | O_ASYNC)); // 設置輸入為O_ASYNC
  signal(SIGIO, on_input);                 // IO信號回調函數

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

上面這個例子可以看到，會先輸出 `do other thing` 字符串，也就是運行 `addstr()` 函數，同時在你輸入字符程序可以通過信號處理函數 `on_input` 來接受輸入進行響應，從而達到了異步的效果。

### aio_read()

異步輸入的第二種方法是通過 `aio_read()` 實現，相比來說更加靈活，但是設置起來也比較複雜，其設置步驟如下：
  1. 設置信號處理函數，接受用戶輸入；
  2. 設置 `struct aiocb` 中的變量指明等待什麼類型的輸入，當輸入的時候產生什麼信號；
  3. 將 `struct aiocb` 傳遞給 `aio_read()` 來遞交讀入請求。

其中 `struct aiocb` 定義如下:

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

下面是一個簡單的例子:

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
  kbcbuf.aio_buf = input; // 設置接受輸入的buf
  kbcbuf.aio_nbytes = 1;  // 設置接受輸入的字節大小
  // 設置處理輸入的方法，SIGE_SIGNAL代表通過發送信號來處理
  kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  // 設置要發送的信號
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

可以通過 `gcc -o main main.c -Wall -lncurses -lrt` 編譯。

### 異步函數

實際上，也可以直接使用類似 select()、poll()、epoll() 函數。


## 參考

可以查看下官方的介紹文檔 [Curses Programming with Python](https://docs.python.org/2/howto/curses.html)，以及 Python 中的 curses 模塊，可查看 [Terminal handling for character-cell displays](https://docs.python.org/2/library/curses.html) 。

<!--
Terminal handling for character-cell displays
https://docs.python.org/dev/library/curses.html
-->

關於 C 語言中 ncurses 的相關內容可以參考 [NCURSES Programming HOWTO](http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/) 文檔，以及官方網站 [www.gnu.org](https://www.gnu.org/software/ncurses/) 中的介紹。

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
ncurse貪吃蛇
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

有趣！10個你不得不知的Linux終端遊戲 
http://www.freebuf.com/articles/others-articles/124743.html
-->

{% highlight text %}
{% endhighlight %}
