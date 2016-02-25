# 把共享庫(SO)加載到指定的內存地址
 
把共享庫(SO)加載到指定的內存地址
　　一位朋友最近遇到一個棘手的問題，希望把共享庫(SO)加載到指定的內存地址，目的可能是想通過prelink來加快應用程序的起動速度。他問我有沒有什麼方法。我知道Windows下是可以的，比如在VC6裡設置/base的值就行了，所以相信在linux下也是可行的。

VC有編譯選項可以設置，猜想gcc也應該有吧。gcc本身只是一個外殼，鏈接工作是由於l
d完成的，當然是應該去閱讀ld命令行選項文檔。很快發現ld有個—image-base選項，可以設置動態庫的加載地址。

通過Xlinker把這個參數傳遞給ld，但是ld不能識別這個選項：
　　
```sh
gcc -g -shared test.c -Xlinker --image-base -Xlinker 0x00c00000 -o libtest.so
/usr/bin/ld: unrecognized option '--image-base'
/usr/bin/ld: use the --help option for usage information
collect2: ld returned 1 exit status
```

再仔細看手冊，原來這個選項只適用於PE文件，PE文件是Windows下專用的，在linux下自然用不了，看來得另想辦法。

我知道ld script可以控制ld的行為，於是研究ld script的寫法，按照手冊裡的說明，寫了一個簡單的ld script:

```sh
SECTIONS
　　 {
　　 . = 0x00c00000;
　　 .text : { *(.text) }
　　 .data : { *(.data) }
　　 .bss : { *(.bss) }
　　 }
```

按下列方式編譯：

```sh
gcc -shared -g -Xlinker --script -Xlinker ld.s test.c -o libtest.so
gcc -g main.c -L./ -ltest -o test.exe
```

用ldd查看，加載地址正確。

```sh
linux-gate.so.1 => (0x00aff000)
libtest.so => ./libtest.so (0x00c00000)
libc.so.6 => /lib/libc.so.6 (0x007fa000)
/lib/ld-linux.so.2 (0x007dd000)
```

但運行時會crash:
[root@localhost lds]# ./test.exe 
Segmentation fault
調試運行可以發現：

```sh
(gdb) r
　　Starting program: /work/test/lds/test.exe 
　　Reading symbols from shared object read from target memory...done.
　　Loaded system supplied DSO at 0x452000
　　Program received signal SIGSEGV, Segmentation fault.
　　0x007e7a10 in _dl_relocate_object () from /lib/ld-linux.so.2
　　(gdb) bt
　　#0 0x007e7a10 in _dl_relocate_object () from /lib/ld-linux.so.2
　　#1 0x007e0833 in dl_main () from /lib/ld-linux.so.2
　　#2 0x007f056b in _dl_sysdep_start () from /lib/ld-linux.so.2
　　#3 0x007df48f in _dl_start () from /lib/ld-linux.so.2
　　#4 0x007dd847 in _start () from /lib/ld-linux.so.2
```

猜想可能是ld.s寫得不全，導致一些信息不正確。於是用ld –verbose導出一個默認的ld script。不出所料，默認的ld script非常冗長，下面是開頭一段：


```sh
/* Script for -z combreloc: combine and sort reloc sections */
OUTPUT_FORMAT("elf32-i386", "elf32-i386",
"elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
SEARCH_DIR("/usr/i386-redhat-linux/lib"); SEARCH_DIR("/usr/local/lib"); SEARCH_DIR("/lib"); SEARCH_DIR("/usr/lib");
SECTIONS
{
/* Read-only sections, merged into text segment: */
PROVIDE (__executable_start = 0x08048000); . = 0x08048000 + SIZEOF_HEADERS;
.interp : { *(.interp) }
.hash : { *(.hash) }
.dynsym : { *(.dynsym) }
.dynstr : { *(.dynstr) }
.gnu.version : { *(.gnu.version) }
.gnu.version_d : { *(.gnu.version_d) }
.gnu.version_r : { *(.gnu.version_r) }
}
```

按照ld script的語法，我把它修改為(紅色部分為新增行)：
```sh
/* Script for -z combreloc: combine and sort reloc sections */
OUTPUT_FORMAT("elf32-i386", "elf32-i386",
        "elf32-i386")
OUTPUT_ARCH(i386)
    ENTRY(_start)
    SEARCH_DIR("/usr/i386-redhat-linux/lib"); SEARCH_DIR("/usr/local/lib"); SEARCH_DIR("/lib"); SEARCH_DIR("/usr/lib");
    SECTIONS
{
/* Read-only sections, merged into text segment: */
PROVIDE (__executable_start = 0x08048000); . = 0x08048000 + SIZEOF_HEADERS;
. = 0x00c00000;
.interp : { *(.interp)  }
.hash : { *(.hash)  }
.dynsym : { *(.dynsym)  }
.dynstr : { *(.dynstr)  }
.gnu.version : { *(.gnu.version)  }
.gnu.version_d : { *(.gnu.version_d)  }
.gnu.version_r : { *(.gnu.version_r)  }
}

```

用這個ld script再次測試，一切正常。又驗證多個共享庫的情況，也正常，下面是測試數據：
- test.c

```c
int test(int n)
{
    return n;
}
```

- test1.c

```c
int test1(int n)
{
    return n;
}
```

- main.c

```c
extern int test(int n);
extern int test1(int n);
#include <stdio.h>
int main(int argc, char* argv[])
{
    printf("Hello: %d %d\n", test(100), test1(200));
    getchar();
    return 0;
}
```

- Makefile

```sh
all:
   gcc -shared -g -Xlinker --script -Xlinker ld.s test.c -o libtest.so
   gcc -shared -g -Xlinker --script -Xlinker ld1.s test1.c -o libtest1.so
   gcc -g main.c -L./ -ltest -ltest1 -o test.exe

clean:
   rm -f *.so *.exe
```

```sh
libtest.so的加載地址為：0x00c00000
libtest1.so的加載地址為：0x00d00000
```

- ldd顯示結果：

```sh
linux-gate.so.1 => (0x00aa3000)
libtest.so => ./libtest.so (0x00c00000)
libtest1.so => ./libtest1.so (0x00d00000)
libc.so.6 => /lib/libc.so.6 (0x007fa000)
/lib/ld-linux.so.2 (0x007dd000)
```

- maps的內容為：

```sh
007dd000-007f6000 r-xp 00000000 03:01 521466 /lib/ld-2.4.so
007f6000-007f7000 r-xp 00018000 03:01 521466 /lib/ld-2.4.so
007f7000-007f8000 rwxp 00019000 03:01 521466 /lib/ld-2.4.so
007fa000-00926000 r-xp 00000000 03:01 523579 /lib/libc-2.4.so
00926000-00929000 r-xp 0012b000 03:01 523579 /lib/libc-2.4.so
00929000-0092a000 rwxp 0012e000 03:01 523579 /lib/libc-2.4.so
0092a000-0092d000 rwxp 0092a000 00:00 0 
00c00000-00c01000 r-xp 00001000 03:03 16370 /work/test/ldsex/libtest.so
00c01000-00c02000 rwxp 00001000 03:03 16370 /work/test/ldsex/libtest.so
00cf1000-00cf2000 r-xp 00cf1000 00:00 0 [vdso]
00d00000-00d01000 r-xp 00001000 03:03 16373 /work/test/ldsex/libtest1.so
00d01000-00d02000 rwxp 00001000 03:03 16373 /work/test/ldsex/libtest1.so
08048000-08049000 r-xp 00000000 03:03 16374 /work/test/ldsex/test.exe
08049000-0804a000 rw-p 00000000 03:03 16374 /work/test/ldsex/test.exe
b7fdf000-b7fe0000 rw-p b7fdf000 00:00 0 
b7fed000-b7ff0000 rw-p b7fed000 00:00 0 
bf8db000-bf8f0000 rw-p bf8db000 00:00 0 [stack]
```
