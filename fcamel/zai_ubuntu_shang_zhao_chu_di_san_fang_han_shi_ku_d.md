# 在 Ubuntu 上找出第三方函式庫的程式碼


篇藉由一個小例子說明如何使用 gdb, locate, apt-file, apt-get 找出問題原因。藉由取得第三方函式庫的程式碼，可以減少瞎猜的時間。

問題

我發現某個程式會不預期的結束。但它不是 crash，沒有 core dump 可看。

`用 gdb 找出怎麼結束的`

先用 gdb attach 程式，繼續操作。程式結束時 gdb 顯示是呼叫 exit() 結束的。

於是再執行一次，這次用 gdb 在 exit 設中斷點再繼續執行。

取得的 backtrace 如下：

```sh
#0  __GI_exit (...) at exit.c:104
#1  0x00007fdd27f60408 in _XDefaultError (...) at ../../src/XlibInt.c:1414
#2  0x00007fdd27f6054b in _XError (...) at ../../src/XlibInt.c:1463
#3  0x00007fdd27f5d5e7 in handle_error ...) at ../../src/xcb_io.c:213
#4  0x00007fdd27f5e687 in _XReply (...) at ../../src/xcb_io.c:699
#5  0x00007fdd27f45346 in XGetWindowProperty (...) at ../../src/GetProp.c:69
#6  0x00007fdd2825db30 in XmuClientWindow () from /usr/lib/x86_64-linux-gnu/libXmu.so.6
...
```

看 backtrace 沒什麼頭緒，都是第三方函式庫的程式。照 XmuClientWindow() 的說明，它可能會失敗，但它不該直接呼叫 exit()。先找 XmuClientWindow 的原始碼，看看有什麼線索。

##用 Ubuntu 的 package 系統找出原始碼

首先用 apt-file 找出 libXmu.so.6 在那個套件 (第一次執行需先跑 apt-file update 更新索引)：

```sh
$ apt-file search /usr/lib/x86_64-linux-gnu/libXmu.so.6
libxmu6: /usr/lib/x86_64-linux-gnu/libXmu.so.6
libxmu6: /usr/lib/x86_64-linux-gnu/libXmu.so.6.2.0
libxmu6-dbg: /usr/lib/debug/usr/lib/x86_64-linux-gnu/libXmu.so.6.2.0
```

然後用 apt-get source 取得原始碼：

```sh
$ apt-get source libxmu6
```

在 src/ClientWin.c 裡找到 XmuClientWindow [*1]：

```c
Window
XmuClientWindow(Display *dpy, Window win)
{
    ...
    XGetWindowProperty(dpy, win, WM_STATE, 0, 0, False, AnyPropertyType,
               &type, &format, &nitems, &after, &data);
    ....
}
```

沒什麼有用資訊，繼續找 XGetWindowProperty 的程式碼。從 backtrace 裡知道它在 ../../src/GetProp.c 裡，但我不知道 GetProp.c 是什麼套件的程式。用 Google 搜尋 GetProp.c 也許會有線索，不過這裡我用另一個作法。

##從 header 找出原始碼

src/ClientWin.c 引入的 header 不多，只有這些：

```c
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <X11/Xmu/WinUtil.h>
```

先用 locate 查出完整路徑：

```sh
$ locate X11/Xlib.h
/usr/include/X11/Xlib.h
```

再用 apt-file 找套件名稱：

```sh
$ apt-file search /usr/include/X11/Xlib.h
libx11-dev: /usr/include/X11/Xlib.h
```

取得原始碼：

```sh
$ apt-get source libx11-dev
```

附帶一提，可透過 gcc -E 展開 #include 驗證 XGetWindowProperty 確實出自 X11/Xlib.h：

```sh
$ echo "#include " | gcc - -E | grep XGetWindowProperty
extern int XGetWindowProperty(
```

header 太多的話，直接編譯原始檔比較快：

```sh
$ gcc -E src/ClientWin.c > t
```

得到的資訊如下：
```sh
1674 # 1303 "/usr/include/X11/Xlib.h" 3 4
...
3060 extern int XGetWindowProperty(
```

找到目標後，往回找第一個 header 路徑就是來源了。如果找不到的話，可能是少指定 include header 的來源目錄。可以從 make 產生的結果得知完整的編譯訊息：

```sh
$ ./configure
$ make V=1 | grep ClientWin.c
/bin/bash ../libtool  --tag=CC   --mode=compile gcc -std=gnu99 -DHAVE_CONFIG_H \
-I. -I..  -I../include -I../include/X11/Xmu < ... 略 ...> -c -o ClientWin.lo ClientWin.c
...
```

找出 -I 的部份，和 -E 一樣加在 gcc 的參數上展開 include。

關於如何讓 make 顯示編譯時的參數 (cflags)，參見這裡。

繼續追蹤程式碼

在 src/GetProp.c 裡發現：

```c
 69     if (!_XReply (dpy, (xReply *) &reply, 0, xFalse)) {
```

這和前面的 backtrace 一致：

```sh
#0  __GI_exit (...) at exit.c:104
#1  0x00007fdd27f60408 in _XDefaultError (...) at ../../src/XlibInt.c:1414
#2  0x00007fdd27f6054b in _XError (...) at ../../src/XlibInt.c:1463
#3  0x00007fdd27f5d5e7 in handle_error ...) at ../../src/xcb_io.c:213
#4  0x00007fdd27f5e687 in _XReply (...) at ../../src/xcb_io.c:699
#5  0x00007fdd27f45346 in XGetWindowProperty (...) at ../../src/GetProp.c:69
#6  0x00007fdd2825db30 in XmuClientWindow () from /usr/lib/x86_64-linux-gnu/libXmu.so.6
```

表示尋找的方向正確。再繼續看 src/xcb_io.c 和 src/XlibInt.c 內相關函式，得知：


// src/XlibInt.c

```c
1463     rtn_val = (*_XErrorFunction)(dpy, (XErrorEvent *)&event); /* upcall */
```

對照 backtrace 得知執行 _XErrorFunction 後，是呼叫 _XDefaultError。_XDefaultError 程式如下：

```c
1409 int _XDefaultError(
1410     Display *dpy,
1411     XErrorEvent *event)
1412 {
1413     if (_XPrintDefaultError (dpy, event, stderr) == 0) return 0;
1414     exit(1);
1415     /*NOTREACHED*/
1416 }
```

確認是這裡呼叫 exit()。所以換掉 _XErrorFunction 可以解決問題。

在程式碼裡搜尋 _XErrorFunction 發現可用 XSetErrorHandler() 設值。這樣至少不會造成程式結束了。

備註

1. 可用 grep 尋找，我是用 gj，找起來更有效率。