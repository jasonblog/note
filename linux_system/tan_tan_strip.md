# 談談strip




用中文說，就是從object 檔中把symbol丟掉。讓我們做幾個小實驗吧。

* [測試一：Strip 執行檔](#strip_test1)
* [測試二：Strip Object檔](#strip_test2)
* [測試三：Strip debug 資訊](#strip_test3)
* [測試四：Strip shared library](#strip_test4)

## 測試環境

```sh
$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 14.04.3 LTS
Release:	14.04
Codename:	trusty
```

## 測試程式
- main_test.c

```c 
#include <stdio.h>
extern test();

int main(void)
{
    test();

    return 0;
}
```
- test.c

```c 
#include <stdio.h>

char *g_myStr = "Wen";
static char *gp_myStr = "Liao";

static void s_test(void)
{
    printf("%s %s\n", g_myStr, gp_myStr);
}


void test(void)
{
    printf("Hello ");
    s_test();
}
```
- Makefile

```sh
TARGET=test
SRCS=test.c main_test.c
OBJS=$(patsubst  %.c, %.o, $(SRCS))
CFLAGS=-g

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET) $(OBJS)
```


## 測試一：Strip 執行檔

```sh
$ make clean
rm -rf test  test.o  main_test.o

$ make
cc -g   -c -o test.o test.c
cc -g   -c -o main_test.o main_test.c
cc -g test.o main_test.o -o test

$ ./test 
Hello Wen Liao

$ ls test -gG
-rwxrwxr-x 1 10217 Nov 16 22:33 test
```

可以看到產生出來的執行檔有`10217` bytes。我們進一步來看執行檔的symbol。

```text
$ nm test
0000000000601050 B __bss_start
...
0000000000601040 D g_myStr
00000000004003e0 T _init
...
0000000000601048 d gp_myStr
...
000000000040056d T main
                 U printf@@GLIBC_2.2.5
...
000000000040052d t s_test
0000000000400553 T test
0000000000601050 D __TMC_END__
```

那麼來看看strip後的檔案size和symbol吧。你可以看到size變小而且symbol不見了。另外上面可以比對一下`t`、`T`、`d`、`D`和`s_test`、`test`、`gp_myStr`、`g_myStr`的關係。

```sh
$ strip test

$ ./test 
Hello Wen Liao

$ nm test
nm: test: no symbols

$ ls -gG test
-rwxrwxr-x 1 6296 Nov 16 22:36 test
```


## 測試二：Strip Object檔
因為strip就是把object file (執行檔也是一種object file)的symbol拿掉，所以在link time需要symbol時如果該object檔案被strip過，就會發生錯誤。範例如下：

```sh
$ make clean
rm -rf test  test.o  main_test.o

$ make
cc -g   -c -o test.o test.c
cc -g   -c -o main_test.o main_test.c
cc -g test.o main_test.o -o test

$ nm -a test.o 
0000000000000000 b .bss
0000000000000000 n .comment
0000000000000000 d .data
0000000000000000 N .debug_abbrev
0000000000000000 N .debug_aranges
0000000000000000 N .debug_info
0000000000000000 N .debug_line
0000000000000000 N .debug_str
0000000000000000 r .eh_frame
0000000000000000 D g_myStr
0000000000000008 d gp_myStr
0000000000000000 n .note.GNU-stack
                 U printf
0000000000000000 r .rodata
0000000000000000 t s_test
0000000000000026 T test
0000000000000000 a test.c
0000000000000000 t .text

$ ls -gG test.o
-rw-rw-r-- 1 3944 Nov 16 23:02 test.o

$ strip test.o 

$ ls -gG test.o
-rw-rw-r-- 1 952 Nov 16 23:03 test.o

$ nm test.o
nm: test.o: no symbols

$ make
cc -g test.o main_test.o -o test
/usr/bin/ld: error in test.o(.eh_frame); no .eh_frame_hdr table will be created.
main_test.o: In function `main':
/home/wen/tmp/sandbox/main_test.c:6: undefined reference to `test'
collect2: error: ld returned 1 exit status
make: *** [test] Error 1
```

這邊我`nm`下了`-a`參數，這會顯示出**所有的symbol**，預設的`nm`輸出如下提供比較。

```text
$ nm test.o 
0000000000000000 D g_myStr
0000000000000008 d gp_myStr
                 U printf
0000000000000000 t s_test
0000000000000026 T test
```

<a name="strip_test3"></a>
## 測試三：Strip debug 資訊

其實只是單純要介紹`-d`參數而已

```sh
$ make clean
rm -rf test  test.o  main_test.o

$ ls -gG test.o
-rw-rw-r-- 1 3944 Nov 16 23:02 test.o

$ make
cc -g   -c -o test.o test.c
cc -g   -c -o main_test.o main_test.c
cc -g test.o main_test.o -o test

$ ls -gG test.o
-rw-rw-r-- 1 3944 Nov 16 23:02 test.o

$ strip -d test.o

$ nm -a test.o
0000000000000000 b .bss
0000000000000000 n .comment
0000000000000000 d .data
0000000000000000 r .eh_frame
0000000000000000 D g_myStr
0000000000000008 d gp_myStr
0000000000000000 n .note.GNU-stack
                 U printf
0000000000000000 r .rodata
0000000000000000 t s_test
0000000000000026 T test
0000000000000000 t .text

$ make
cc -g test.o main_test.o -o test

$ ./test
Hello Wen Liao

$ ls -gG test test.o
-rwxrwxr-x 1 9737 Nov 16 23:01 test
-rw-rw-r-- 1 1896 Nov 16 23:01 test.o
```


## 測試四：Strip shared library
這邊要幹的第一件事是修改Makefile如下。主要是把test.o包裝成shared library，這個Makefile很醜，我知道。

- Makefile

```sh
TARGET=test
SRC=main_test.c
OBJ=$(patsubst  %.c, %.o, $(SRC))

LIB_SRC=test.c
LIB_OBJ=$(patsubst  %.c, %.o, $(LIB_SRC))
LIB_NAME=test
LIB=lib$(LIB_NAME).so
CFLAGS=-g

$(TARGET): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $< -o $@ -L./ -l$(LIB_NAME)

$(LIB): $(LIB_OBJ)
	$(CC) -shared -Wl,-soname,$(LIB).0 $^ -o $@
	rm $(LIB).0 && ln -sf $(LIB) $(LIB).0

$(LIB_OBJ): $(LIB_SRC)
	$(CC) $(CFLAGS) -c -fPIC $^

clean:
	rm -rf $(TARGET) $(OBJ) $(LIB_OBJ)
```

自幹shared library執行程式的時候不要忘記加上`LD_LIBRARY_PATH`環境變數：

```sh
$ make clean
rm -rf test  main_test.o  test.o

$ make
cc -g   -c -o main_test.o main_test.c
cc -g -c -fPIC test.c
cc -shared -Wl,-soname,libtest.so.0 test.o -o libtest.so
rm libtest.so.0 && ln -sf libtest.so libtest.so.0
cc -g main_test.o -o test -L./ -ltest

$ LD_LIBRARY_PATH=`pwd` ./test
Hello Wen Liao
```

現在來比較strip前後的shared library 差異吧。

```sh
$ nm -a libtest.so
0000000000000000 a 
0000000000201048 b .bss
0000000000201048 B __bss_start
0000000000000000 n .comment
0000000000201048 b completed.6973
0000000000000000 a crtstuff.c
0000000000000000 a crtstuff.c
                 w __cxa_finalize@@GLIBC_2.2.5
0000000000201030 d .data
0000000000000000 N .debug_abbrev
0000000000000000 N .debug_aranges
0000000000000000 N .debug_info
0000000000000000 N .debug_line
0000000000000000 N .debug_str
0000000000000650 t deregister_tm_clones
00000000000006c0 t __do_global_dtors_aux
0000000000200df0 t __do_global_dtors_aux_fini_array_entry
0000000000201030 d __dso_handle
0000000000200e00 d .dynamic
0000000000200e00 d _DYNAMIC
0000000000000398 r .dynstr
0000000000000230 r .dynsym
0000000000201048 D _edata
00000000000007c0 r .eh_frame
000000000000079c r .eh_frame_hdr
0000000000201050 B _end
000000000000077c T _fini
000000000000077c t .fini
0000000000200df0 t .fini_array
0000000000000700 t frame_dummy
0000000000200de8 t __frame_dummy_init_array_entry
0000000000000840 r __FRAME_END__
0000000000201000 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000201038 D g_myStr
00000000000001f0 r .gnu.hash
000000000000045c r .gnu.version
0000000000000480 r .gnu.version_r
0000000000200fd0 d .got
0000000000201000 d .got.plt
0000000000201040 d gp_myStr
00000000000005f0 T _init
00000000000005f0 t .init
0000000000200de8 t .init_array
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
0000000000200df8 d .jcr
0000000000200df8 d __JCR_END__
0000000000200df8 d __JCR_LIST__
                 w _Jv_RegisterClasses
00000000000001c8 r .note.gnu.build-id
0000000000000610 t .plt
                 U printf@@GLIBC_2.2.5
0000000000000680 t register_tm_clones
00000000000004a0 r .rela.dyn
00000000000005a8 r .rela.plt
0000000000000785 r .rodata
0000000000000735 t s_test
0000000000000760 T test
0000000000000000 a test.c
0000000000000650 t .text
0000000000201048 d __TMC_END__

$ ls -gG libtest.so
-rwxrwxr-x 1 9275 Nov 16 23:47 libtest.so

$ strip libtest.so

$ ls -gG libtest.so
-rwxrwxr-x 1 6104 Nov 16 23:47 libtest.so

$ nm -a libtest.so
nm: libtest.so: no symbols

$ LD_LIBRARY_PATH=`pwd` ./test
Hello Wen Liao
```

這邊變成有新的作業，dynamic link的時候沒有shared library沒有symbol怎麼拿到function address和全域變數？下次有看到再來解釋吧。
## 補充
如果編譯的程式碼有加入debug資訊，objdump在反組譯的時候可以加入`-S`參數比對原始碼對應的機械碼，對於想要研究系統細節的人應該有所幫助。簡單範例如下

```sh
 x$ make clean
rm -rf test  test.o  main_test.o
$ make
cc -g   -c -o test.o test.c
cc -g   -c -o main_test.o main_test.c
cc -g test.o main_test.o -o test

$ objdump -S -d test

test:     file format elf64-x86-64


Disassembly of section .init:

00000000004003e0 <_init>:
  4003e0:	48 83 ec 08          	sub    $0x8,%rsp
...

0000000000400553 <test>:


void test(void)
{
  400553:	55                   	push   %rbp
  400554:	48 89 e5             	mov    %rsp,%rbp
    printf("Hello ");
  400557:	bf 24 06 40 00       	mov    $0x400624,%edi
  40055c:	b8 00 00 00 00       	mov    $0x0,%eax
  400561:	e8 aa fe ff ff       	callq  400410 <printf@plt>
    s_test();
  400566:	e8 c2 ff ff ff       	callq  40052d <s_test>
}
  40056b:	5d                   	pop    %rbp
  40056c:	c3                   	retq   

...
```

## 參考資料
* Binary Hacks：駭客秘傳技巧一百招
