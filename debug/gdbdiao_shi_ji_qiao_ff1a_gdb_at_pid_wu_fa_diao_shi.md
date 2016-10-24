# GDB調試技巧：gdb at pid無法調試的問題


當我們使用GDB調試一個daemon的時候，一般有兩種方式：
1. 直接在shell命令行鍵入gdb attach pid （要調試daemon的進程ID）。一般情況下，我都習慣於縮寫gdb at pid；
2. 在shell中鍵入gdb，進入gdb，然後attach pid（同樣是要調試daemon的進程ID）。我不常用這種方式，因為這種方式需要2個步驟——儘管鍵入的字母是一樣的。

下面來說說今天遇到的問題。公司的一個實習生來問我問題。他目前正在做一個工作，大致的功能是得到某個daemon的某一時刻的調用棧——就像gdb一樣。但是他發現當daemon處於系統調用狀態時，從EBP得到的返回地址不正確。然後我讓他直接使用gdb查看是否可以看到完整的調用棧，如果GDB可以，那麼就沒有問題，可以去看GDB如何實現的。如果不能，再想為什麼。——這裡先插一句，當時我去查看了他的EBP的值，根據其值獲得的返回地址確實不正確。今晚查了查，知道在某些函數調用時，為了速度，EBP有可能並沒有被壓棧。但是這個不是今天的主題，讓我們再轉回來。他很快的試了試，告訴我gdb雖然可以使用bt(backtrace)得到函數棧，但是顯示出來的結果卻不正確。

怎麼回事呢？不可能啊。我在他那寫了一個很簡單的測試程序：


```c
#include <stdlib.h>
#include <stdio.h>

static void test(void)
{
    pause();
}


int main(void)
{
    test();

    return 0;
}
```

編譯並運行：

```sh
[xxx@xxx-vm-fc13 test]$ gcc -g test.c
[xxx@xxx-vm-fc13 test]$ ./a.out
```
然後使用另外一個終端進行調試：

```sh
[xxx@xxx-vm-fc13 test]$ ps auwx|grep a.out
xxx 2412 0.0 0.0 1816 288 pts/4 S+ 11:19 0:00 ./a.out
xxx 2415 0.0 0.0 4308 732 pts/5 S+ 11:20 0:00 grep a.out
[xxx@xxx-vm-fc13 test]$ gdb at 2412
GNU gdb (GDB) Fedora (7.1-18.fc13)
Copyright (C) 2010 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-redhat-linux-gnu".
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>...
Reading symbols from /usr/bin/at...(no debugging symbols found)...done.
Attaching to program: /usr/bin/at, process 2412
Reading symbols from /lib/ld-linux.so.2...(no debugging symbols found)...done.
Loaded symbols for /lib/ld-linux.so.2
0x009f4424 in __kernel_vsyscall ()
Missing separate debuginfos, use: debuginfo-install at-3.1.12-5.fc13.i686
(gdb) bt
#0 0x009f4424 in __kernel_vsyscall ()
#1 0x0065dac6 in ?? ()
#2 0x080483dc in ?? ()
#3 0x005d6cc6 in ?? ()
#4 0x08048331 in ?? ()
(gdb)
```

看到這個結果，我還真有些意外。我使用gdb也有些年頭了，從來都是這樣調試daemon的啊。怎麼會這樣呢啊？看這個輸出，很像沒有調試信息啊。可是前面的編譯有-g選項啊。

使用第二種方法試試？

```sh
[xxx@xxx-vm-fc13 test]$ gdb
GNU gdb (GDB) Fedora (7.1-18.fc13)
Copyright (C) 2010 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-redhat-linux-gnu".
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
(gdb) at 2412
Attaching to process 2412
Reading symbols from /home/fgao/works/test/a.out...done.
Reading symbols from /lib/libc.so.6...(no debugging symbols found)...done.
Loaded symbols for /lib/libc.so.6
Reading symbols from /lib/ld-linux.so.2...(no debugging symbols found)...done.
Loaded symbols for /lib/ld-linux.so.2
0x009f4424 in __kernel_vsyscall ()
Missing separate debuginfos, use: debuginfo-install glibc-2.12-1.i686
(gdb) bt
#0 0x009f4424 in __kernel_vsyscall ()
#1 0x0065dac6 in __pause_nocancel () from /lib/libc.so.6
#2 0x080483cf in test () at test.c:6
#3 0x080483dc in main () at test.c:12
(gdb)
```
<ol start="1" class="dp-css"><li>(gdb) q</li><li>
A debugging session is active.</li><li>
</li><li>
        Inferior 1 [process 2412] will be detached.</li><li>
</li><li>
Quit anyway? (y or n) y</li><li><font color="#f00000">
Detaching from program: /usr/bin/at, process 2412</font></li></ol>



啊，這種方式就沒有問題啊。怎麼回事呢？難道是因為這兩種方式的attach還有區別不成？於是我又查了gdb的說明，應該沒有區別啊。

想了半天，又試了幾次第一種方式，還是沒有結果。當我感覺我以前的GDB的世界觀都要崩潰的時候，對自己的GDB技能產生了極度的不自信的時候，再次退出gdb的時候，我發現了原因。該原因差點沒把我鬱悶死。。。

大家想一下哦。。。

注意看這裡的紅色代碼。進程號沒有錯，但是program卻是/usr/bin/at。My god！原來我使用第一種方式的時候，一直在嘗試調試at——這個linux命令。怪不得沒有調試信息呢！在我以前的GDB應用中，因為系統中沒有at這個命令，所以gdb at pid就等於gdb attach pid，是可以縮寫的。但是在當前這個環境中，很並不幸的存在著at這個命令。結果GDB沒有嘗試去attach，而是直接去調試at命令。

不過這裡，我覺得GDB可以做得更友好一些。雖然在這個情況下，GDB去調試at沒有問題，但是後面的PID已經被我的測試程序佔用了，它應該報個錯誤或者警告吧。

最後總結一下，GDB的縮寫是好，但是小心過分的縮寫會與已有命令衝突哦~~總之，縮寫需謹慎。



