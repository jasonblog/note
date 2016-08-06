# 抓漏 - gdb 和 valgrind 合體技


接續[前一篇](http://wen00072.github.io/blog/2014/11/29/catching-leakage-use-valgrind-checking-c-memory-leak)的話題。在查資料的時候看到valgrind可以和gdb合作，試了一下發現他的作法和我想的不一樣。原本以為是valgrind跑出來以後直接把漏掉的call stack和program counter傳給gdb，而gdb把他當成一種中斷點。結果完全不是，他的用法是：

* 跑valgrind，參數指定要和gdb合體，還有你要抓漏的程式
* 另外一邊執行gdb，載入你要抓漏的程式
* gdb 連到valgrind
* 從gdb中設**懷疑漏掉**的中斷點
* 從gdb叫valgrind開始執行你要抓漏的程式，gdb將會中斷在你設置的中斷點
* 在使用valgrind提供的指令，比對執行中斷點之前和之後是否有memory leak

測試程式碼和測試環境和[前一篇](http://wen00072.github.io/blog/2014/11/29/catching-leakage-use-valgrind-checking-c-memory-leak)一模一樣，所以就請自行看連結。

## 合體示範

* 跑valgrind，參數指定要和gdb合體，還有你要抓漏的程式
  * `valgrind --vgdb=yes --vgdb-error=0 ./你要抓漏的程式`

```sh
$ valgrind --vgdb=yes --vgdb-error=0 ./leak
==28450== Memcheck, a memory error detector
==28450== Copyright (C) 2002-2013, and GNU GPL'd, by Julian Seward et al.
==28450== Using Valgrind-3.10.0.SVN and LibVEX; rerun with -h for copyright info
==28450== Command: ./leak
==28450== 
==28450== (action at startup) vgdb me ... 
==28450== 
==28450== TO DEBUG THIS PROCESS USING GDB: start GDB like this
==28450==   /path/to/gdb ./leak
==28450== and then give GDB the following command
==28450==   target remote | /usr/lib/valgrind/../../bin/vgdb --pid=28450
==28450== --pid is optional if only one valgrind process is running
==28450== 
```

* 另外一邊執行gdb，載入你要抓漏的程式

```sh
$ gdb ./leak
GNU gdb (Ubuntu 7.7.1-0ubuntu5~14.04.2) 7.7.1
Copyright (C) 2014 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./leak...done.
(gdb) 
```

* gdb 連到valgrind
  * `target remote | vgdb`
  
```sh
(gdb)  target remote | vgdb
Remote debugging using | vgdb
relaying data between gdb and process 28450
Reading symbols from /lib64/ld-linux-x86-64.so.2...Reading symbols from /usr/lib/debug//lib/x86_64-linux-gnu/ld-2.19.so...done.
done.
Loaded symbols for /lib64/ld-linux-x86-64.so.2
0x00000000040012d0 in _start () from /lib64/ld-linux-x86-64.so.2
(gdb) 
```

* 從gdb中設**懷疑漏掉**的中斷點

```sh
(gdb) b 47
Breakpoint 1 at 0x400703: file leak.c, line 47.
(gdb) b 48
Breakpoint 2 at 0x400708: file leak.c, line 48.
```

47, 48行內容是

```c
47	    leak1();
48	    ch_ptr = leak2();
```

* 從gdb叫valgrind開始執行你要抓漏的程式，gdb將會中斷在你設置的中斷點

```sh
(gdb) c
Continuing.

Breakpoint 1, main () at leak.c:47
47	    leak1();
(gdb)
```

* 在使用valgrind提供的指令，比對執行中斷點之前和之後是否有memory leak
  * 詳細指令可以在gdb中打`help monitor`查詢

執行中斷點前

```sh
(gdb) monitor leak_check full reachable any
==28450== All heap blocks were freed -- no leaks are possible
==28450== 
```

執行下一步後再檢查

```sh
(gdb) n

Breakpoint 2, main () at leak.c:48
48	    ch_ptr = leak2();
(gdb) monitor leak_check full reachable any
==28450== 100 bytes in 1 blocks are still reachable in loss record 1 of 1
==28450==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==28450==    by 0x40065E: leak1 (leak.c:11)
==28450==    by 0x400707: main (leak.c:47)
==28450== 
==28450== LEAK SUMMARY:
==28450==    definitely lost: 0 bytes in 0 blocks
==28450==    indirectly lost: 0 bytes in 0 blocks
==28450==      possibly lost: 0 bytes in 0 blocks
==28450==    still reachable: 100 bytes in 1 blocks
==28450==         suppressed: 0 bytes in 0 blocks
```

多執行幾步後再看看leak情況

```sh
(gdb) n
49	    if(ch_ptr) {
(gdb) n
50	        printf("%s", ch_ptr);
(gdb) n
53	    lk_ptr = leak3();
(gdb) n
54	    if(lk_ptr) {
(gdb) monitor leak_check full reachable any
==28450== 8 bytes in 1 blocks are still reachable in loss record 2 of 5
==28450==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==28450==    by 0x4006C3: leak3 (leak.c:33)
==28450==    by 0x400732: main (leak.c:53)
==28450== 
==28450== 14 bytes in 1 blocks are still reachable in loss record 3 of 5
==28450==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==28450==    by 0x4EC02B9: strdup (strdup.c:42)
==28450==    by 0x400687: leak2 (leak.c:20)
==28450==    by 0x40070C: main (leak.c:48)
==28450== 
==28450== 14 bytes in 1 blocks are still reachable in loss record 4 of 5
==28450==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==28450==    by 0x4EC02B9: strdup (strdup.c:42)
==28450==    by 0x4006E2: leak3 (leak.c:37)
==28450==    by 0x400732: main (leak.c:53)
==28450== 
==28450== 100 bytes in 1 blocks are definitely lost in loss record 5 of 5
==28450==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==28450==    by 0x40065E: leak1 (leak.c:11)
==28450==    by 0x400707: main (leak.c:47)
==28450== 
==28450== LEAK SUMMARY:
==28450==    definitely lost: 100 bytes in 1 blocks
==28450==    indirectly lost: 0 bytes in 0 blocks
==28450==      possibly lost: 0 bytes in 0 blocks
==28450==    still reachable: 36 bytes in 3 blocks
==28450==         suppressed: 0 bytes in 0 blocks
==28450== 
```

## 參考資料

* [gdb + valgrind - (billiob's blog)](http://billiob.net/blog/20140330_vgdb.html)
* `man valgrind`
