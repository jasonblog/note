# 抓漏 - 使用valgrind檢查C語言memory Leak


使用C 語言，memory leak的問題是最棘手的狀況之一，茫茫code海要一個一個比對簡直是大海撈針。幸好Linux下面有好的的工具可以救你一命。簡單介紹一下希望對大家有幫助。

## 目錄

* [測試環境](#env)
* [測試程式](#ex-prg)
* [執行方式](#ex-run)
* [執行結果](#ex-res)
* [參考資料](#ref)

<a name="env"></a>

## 測試環境

* valgrind 在Ubuntu下面可以直接用`sudo apt-get install valgrind`安裝。

```sh
$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 14.04.1 LTS
Release:	14.04
Codename:	trusty

$ valgrind --version
valgrind-3.10.0.SVN
```

## 測試程式
這邊準備了幾個典型的memory leak程式，應該是很簡單看出哪邊有leak，所以我就不多做說明瞭。

- leak.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct leak_example {
    char *ptr;
};

void leak1(void)
{
    int *ptr = (int *)malloc(100);

    if (!ptr) {
        printf("Oops, malloc fail!\n");
    }
}

char *leak2(void)
{
    char *ptr = strdup("Hey Hey Hey!\n");
 
    if (!ptr) {
        printf("Oops, strdup fail!\n");
        return 0;
    }
    return ptr;
}

struct leak_example *leak3(void)
{
    struct leak_example *ptr = 0;

    ptr = (struct leak_example *) malloc(sizeof(struct leak_example));
    if (!ptr) {
        printf("Oops, malloc fail!\n");
    }
    ptr->ptr = strdup("Hey Hey Hey!\n");

    return ptr;
}

int main()
{
    struct leak_example *lk_ptr;
    char * ch_ptr = 0;

    leak1();

    ch_ptr = leak2();
    if(ch_ptr) {
        printf("%s", ch_ptr);
    }

    lk_ptr = leak3();
    if(lk_ptr) {
        printf("%s", lk_ptr->ptr);
        free(lk_ptr);
    }

    return 0;
}
```

因為只是單一檔案，我懶得寫Makefile，編譯指令如下:

```sh
$ CFLAGS="-g" make leak
```
這行指令可以去找Makefile中`Implicit Rules`得到解答。


## 執行方式
最簡單的方式就是用下面的方式

```sh
$ valgrind 你的執行檔
```

然而這樣只會顯示出有漏掉多少的空間，因此要詳細地列出memory leak細節我會使用:

```sh
$ valgrind --leak-check=full --show-leak-kinds=all --verbose 你的程式檔
```
這些選項應該是可以望文生義，所以就不解釋了。想知道細節可以問男人`man valgrind`



## 執行結果

* 節錄重點，我們可以看到valgrind不但找出洩漏多少空間，也明確地列出未釋放的記憶體被分配時的callstack，真是佛心來者。

```sh 
$ valgrind --leak-check=full --show-leak-kinds=all --verbose
...
==26518== HEAP SUMMARY:
==26518==     in use at exit: 128 bytes in 3 blocks
==26518==   total heap usage: 4 allocs, 1 frees, 136 bytes allocated
==26518== 
==26518== Searching for pointers to 3 not-freed blocks
==26518== Checked 78,136 bytes
==26518== 
==26518== 14 bytes in 1 blocks are definitely lost in loss record 1 of 3
==26518==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==26518==    by 0x4EC02B9: strdup (strdup.c:42)
==26518==    by 0x400687: leak2 (leak.c:20)
==26518==    by 0x40070C: main (leak.c:48)
==26518== 
==26518== 14 bytes in 1 blocks are definitely lost in loss record 2 of 3
==26518==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==26518==    by 0x4EC02B9: strdup (strdup.c:42)
==26518==    by 0x4006E2: leak3 (leak.c:37)
==26518==    by 0x400732: main (leak.c:53)
==26518== 
==26518== 100 bytes in 1 blocks are definitely lost in loss record 3 of 3
==26518==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==26518==    by 0x40065E: leak1 (leak.c:11)
==26518==    by 0x400707: main (leak.c:47)
==26518== 
==26518== LEAK SUMMARY:
==26518==    definitely lost: 128 bytes in 3 blocks
==26518==    indirectly lost: 0 bytes in 0 blocks
==26518==      possibly lost: 0 bytes in 0 blocks
==26518==    still reachable: 0 bytes in 0 blocks
==26518==         suppressed: 0 bytes in 0 blocks
```

* 詳細結果

```sh 
$ CFLAGS= -g make leak
cc  -g    leak.c   -o leak

$ valgrind --leak-check=full --show-leak-kinds=all --verbose ./leak
==26518== Memcheck, a memory error detector
==26518== Copyright (C) 2002-2013, and GNU GPL'd, by Julian Seward et al.
==26518== Using Valgrind-3.10.0.SVN and LibVEX; rerun with -h for copyright info
==26518== Command: ./leak
==26518== 
--26518-- Valgrind options:
--26518--    --leak-check=full
--26518--    --show-leak-kinds=all
--26518--    --verbose
--26518-- Contents of /proc/version:
--26518--   Linux version 3.13.0-40-generic (buildd@comet) (gcc version 4.8.2 (Ubuntu 4.8.2-19ubuntu1) ) #69-Ubuntu SMP Thu Nov 13 17:53:56 UTC 2014
--26518-- Arch and hwcaps: AMD64, amd64-cx16-rdtscp-sse3-avx
--26518-- Page sizes: currently 4096, max supported 4096
--26518-- Valgrind library directory: /usr/lib/valgrind
--26518-- Reading syms from /home/wen/work/practice/Linux_Programming_Practice/15_leak/leak
--26518-- Reading syms from /lib/x86_64-linux-gnu/ld-2.19.so
--26518--   Considering /lib/x86_64-linux-gnu/ld-2.19.so ..
--26518--   .. CRC mismatch (computed 4cbae35e wanted 8d683c31)
--26518--   Considering /usr/lib/debug/lib/x86_64-linux-gnu/ld-2.19.so ..
--26518--   .. CRC is valid
--26518-- Reading syms from /usr/lib/valgrind/memcheck-amd64-linux
--26518--   Considering /usr/lib/valgrind/memcheck-amd64-linux ..
--26518--   .. CRC mismatch (computed 37cdde19 wanted adc367dd)
--26518--    object doesn't have a symbol table
--26518--    object doesn't have a dynamic symbol table
--26518-- Scheduler: using generic scheduler lock implementation.
--26518-- Reading suppressions file: /usr/lib/valgrind/default.supp
==26518== embedded gdbserver: reading from /tmp/vgdb-pipe-from-vgdb-to-26518-by-wen-on-???
==26518== embedded gdbserver: writing to   /tmp/vgdb-pipe-to-vgdb-from-26518-by-wen-on-???
==26518== embedded gdbserver: shared mem   /tmp/vgdb-pipe-shared-mem-vgdb-26518-by-wen-on-???
==26518== 
==26518== TO CONTROL THIS PROCESS USING vgdb (which you probably
==26518== don't want to do, unless you know exactly what you're doing,
==26518== or are doing some strange experiment):
==26518==   /usr/lib/valgrind/../../bin/vgdb --pid=26518 ...command...
==26518== 
==26518== TO DEBUG THIS PROCESS USING GDB: start GDB like this
==26518==   /path/to/gdb ./leak
==26518== and then give GDB the following command
==26518==   target remote | /usr/lib/valgrind/../../bin/vgdb --pid=26518
==26518== --pid is optional if only one valgrind process is running
==26518== 
--26518-- REDIR: 0x4019ca0 (strlen) redirected to 0x38068331 (???)
--26518-- Reading syms from /usr/lib/valgrind/vgpreload_core-amd64-linux.so
--26518--   Considering /usr/lib/valgrind/vgpreload_core-amd64-linux.so ..
--26518--   .. CRC mismatch (computed 329d6860 wanted c0186920)
--26518--    object doesn't have a symbol table
--26518-- Reading syms from /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so
--26518--   Considering /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so ..
--26518--   .. CRC mismatch (computed 1fb85af8 wanted 2e9e3c16)
--26518--    object doesn't have a symbol table
==26518== WARNING: new redirection conflicts with existing -- ignoring it
--26518--     old: 0x04019ca0 (strlen              ) R-> (0000.0) 0x38068331 ???
--26518--     new: 0x04019ca0 (strlen              ) R-> (2007.0) 0x04c2e1a0 strlen
--26518-- REDIR: 0x4019a50 (index) redirected to 0x4c2dd50 (index)
--26518-- REDIR: 0x4019c70 (strcmp) redirected to 0x4c2f2f0 (strcmp)
--26518-- REDIR: 0x401a9c0 (mempcpy) redirected to 0x4c31da0 (mempcpy)
--26518-- Reading syms from /lib/x86_64-linux-gnu/libc-2.19.so
--26518--   Considering /lib/x86_64-linux-gnu/libc-2.19.so ..
--26518--   .. CRC mismatch (computed e7228afa wanted 93ff6981)
--26518--   Considering /usr/lib/debug/lib/x86_64-linux-gnu/libc-2.19.so ..
--26518--   .. CRC is valid
--26518-- REDIR: 0x4ec47e0 (strcasecmp) redirected to 0x4a25720 (_vgnU_ifunc_wrapper)
--26518-- REDIR: 0x4ec6ad0 (strncasecmp) redirected to 0x4a25720 (_vgnU_ifunc_wrapper)
--26518-- REDIR: 0x4ec3fb0 (memcpy@GLIBC_2.2.5) redirected to 0x4a25720 (_vgnU_ifunc_wrapper)
--26518-- REDIR: 0x4ec2240 (rindex) redirected to 0x4c2da30 (rindex)
--26518-- REDIR: 0x4eba1d0 (malloc) redirected to 0x4c2ab10 (malloc)
--26518-- REDIR: 0x4ec0540 (strlen) redirected to 0x4c2e0e0 (strlen)
--26518-- REDIR: 0x4ec9200 (__GI_memcpy) redirected to 0x4c2fc90 (__GI_memcpy)
--26518-- REDIR: 0x4ecb540 (strchrnul) redirected to 0x4c319b0 (strchrnul)
Hey Hey Hey!
Hey Hey Hey!
--26518-- REDIR: 0x4eba870 (free) redirected to 0x4c2bd80 (free)
==26518== 
==26518== HEAP SUMMARY:
==26518==     in use at exit: 128 bytes in 3 blocks
==26518==   total heap usage: 4 allocs, 1 frees, 136 bytes allocated
==26518== 
==26518== Searching for pointers to 3 not-freed blocks
==26518== Checked 78,136 bytes
==26518== 
==26518== 14 bytes in 1 blocks are definitely lost in loss record 1 of 3
==26518==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==26518==    by 0x4EC02B9: strdup (strdup.c:42)
==26518==    by 0x400687: leak2 (leak.c:20)
==26518==    by 0x40070C: main (leak.c:48)
==26518== 
==26518== 14 bytes in 1 blocks are definitely lost in loss record 2 of 3
==26518==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==26518==    by 0x4EC02B9: strdup (strdup.c:42)
==26518==    by 0x4006E2: leak3 (leak.c:37)
==26518==    by 0x400732: main (leak.c:53)
==26518== 
==26518== 100 bytes in 1 blocks are definitely lost in loss record 3 of 3
==26518==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==26518==    by 0x40065E: leak1 (leak.c:11)
==26518==    by 0x400707: main (leak.c:47)
==26518== 
==26518== LEAK SUMMARY:
==26518==    definitely lost: 128 bytes in 3 blocks
==26518==    indirectly lost: 0 bytes in 0 blocks
==26518==      possibly lost: 0 bytes in 0 blocks
==26518==    still reachable: 0 bytes in 0 blocks
==26518==         suppressed: 0 bytes in 0 blocks
==26518== 
==26518== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
==26518== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
```

## 參考資料

* `$ man valgrind`
