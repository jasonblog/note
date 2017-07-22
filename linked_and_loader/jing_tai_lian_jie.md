# 靜態連結


前面目的檔章節介紹 ELF 檔案格式

此章節說明如果有兩個目的檔要如何連結

這裡使用 a.c 和 b.c 兩個檔案當範例

```c
# a.c
extern int shared;

int main()
{
    int a = 100;
    swap(&a, &shared);
    return 0;
}

# b.c
int shared = 1;

void swap(int *a, int *b)
{
    *a ^= *b ^= *a ^= *b;
}
```

```sh
readelf -s a.o
Symbol table '.symtab' contains 11 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS a.c
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    3
     4: 0000000000000000     0 SECTION LOCAL  DEFAULT    4
     5: 0000000000000000     0 SECTION LOCAL  DEFAULT    6
     6: 0000000000000000     0 SECTION LOCAL  DEFAULT    7
     7: 0000000000000000     0 SECTION LOCAL  DEFAULT    5
     8: 0000000000000000    44 FUNC    GLOBAL DEFAULT    1 main
     9: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND shared
    10: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND swap
 
$ readelf -s b.o

Symbol table '.symtab' contains 10 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS b.c
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    2
     4: 0000000000000000     0 SECTION LOCAL  DEFAULT    3
     5: 0000000000000000     0 SECTION LOCAL  DEFAULT    5
     6: 0000000000000000     0 SECTION LOCAL  DEFAULT    6
     7: 0000000000000000     0 SECTION LOCAL  DEFAULT    4
     8: 0000000000000000     4 OBJECT  GLOBAL DEFAULT    2 shared(全域符號)
     9: 0000000000000000    74 FUNC    GLOBAL DEFAULT    1 swap  (全域符號)
```

## 空間和位址分配

連結器就是將不同目的檔合成一個

其中有兩種方法，一種將不同的目的檔直接疊加在後面

另一種方法是類似區段合併(.text 合併、.data 合併)

之後所講的空間分配為虛擬位址空間的分配

目前連結器採用兩步連結方式

第一步空間與位址分配，取得目的檔各區間的長度、屬性、位置

並將符號表中所有符號定義和引用收集起來統一放到全域符號表

第二步符號解析和重定，符號解析和重定，調整程式碼中位址。

連結器可用ld來連結

```sh
$ld a.o b.o -e main -o ab

# -e main 表示進入點是 main
# 如果不加預設由 _start 進入，警告如下
# ld: warning: cannot find entry symbol _start; defaulting to 00000000004000e8
```

下面看目的檔的分配


```sh
$ objdump -h a.o

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         0000002c  0000000000000000  0000000000000000  00000040  2**0                 
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  0000006c  2**0                 
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  0000006c  2**0                
                  ALLOC
  ......

  
$ objdump -h b.o

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         0000004a  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000004  0000000000000000  0000000000000000  0000008c  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  00000090  2**0
                  ALLOC
                  

$ objdump -h ab

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000076  00000000004000e8  00000000004000e8  000000e8  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  2 .data         00000004  00000000006001b8  00000000006001b8  000001b8  2**2
                  CONTENTS, ALLOC, LOAD, DATA
```

VMA = 虛擬位址，LMA = 載入位址，一般來說會一樣


```sh
      a.o
+---------------+
|  File Header  | 0x40
+---------------+
|     .text     | 0x2C
+---------------+

      b.o
+---------------+
|  File Header  | 0x40
+---------------+
|     .text     | 0x4a
+---------------+
|     .data     | 0x04
+---------------+

      ab.o
+---------------+
|  File Header  | 0xe8
+---------------+ 
|     .text     | 0x76 (a.o + b.o)
+---------------+
|     .data     | 0x04 (b.o)
+---------------+

      VM
+---------------+ 0x6001BC
|     .data     | Size = 0x4
+---------------+ 0x6001b8

+---------------+ 0x40015E
|     .text     | Size = 0x76
+---------------+ 0x4000e8
```
這裡書上有提到 ELF 可執行檔預設由 0x08048000 開始，不過實際上卻沒有?
##符號位址確定

上面第一步已完成，主要就是分配 ab 區段的 VMA 起始位址
##符號解析與重定

空間位址分配好之後，就進入符號解析與重定步驟

先使用 objdump 對 a.o 進行反組譯

程式裡面使用的都是 VMA，例如下面的 main 起始位址就是 0x0

未進行空見分配前目的檔都是以 0x0 開始，空間分配結束才會知道 VMA

此函式的 main 佔用 0x2b byte，共 12 行指令

```sh
$ objdump -d a.o

a.o:     file format elf64-x86-64

Disassembly of section .text:

0000000000000000 <main>:
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
   4:   48 83 ec 10             sub    $0x10,%rsp
   8:   c7 45 fc 64 00 00 00    movl   $0x64,-0x4(%rbp)
   f:   48 8d 45 fc             lea    -0x4(%rbp),%rax
  13:   be 00 00 00 00          mov    $0x0,%esi
  18:   48 89 c7                mov    %rax,%rdi
  1b:   b8 00 00 00 00          mov    $0x0,%eax
  20:   e8 00 00 00 00          callq  25 <main+0x25>
  25:   b8 00 00 00 00          mov    $0x0,%eax
  2a:   c9                      leaveq
  2b:   c3   
  ````

下面指令是將shared的引用指定到esi暫存器

```sh
13:   be 00 00 00 00          mov    $0x0,%esi
```

## 全域建構與解構

實際上程式並不是由 main 開始執行

main 之前會做一些事情，例如堆積分配初始化 malloc、free

建構式也是在 main 之前執行，一般入口為 _start (Glibs)

因此在 ELF 中還定義兩種區段，.init 和 .fini


## ABI

不同編譯器編出的目的檔不一定可以連結

如果要能連結需要滿足一些條件


```sh
同樣的目的檔格式
同樣的名稱修飾標準
同樣的記憶體配置
......
```

這些和可執行檔相容性相關的內容稱為 ABI

ABI 是二進位層面的介面，例如 C++ 物件記憶體配置是 ABI 的一部分

有一個包好的函示庫為 libc.a 位於/usr/lib/x86_64-linux-gnu$

使用ar來看此函示庫包了什麼目的檔

```sh
$ar -t libc.a
   1 init-first.o
   2 libc-start.o
   3 sysdep.o
   4 version.o
   5 check_fds.o
   6 libc-tls.o
   7 elf-init.o
   8 dso_handle.o
   9 errno.o
  10 init-arch.o
  11 errno-loc.o
  12 hp-timing.o
  13 iconv_open.o
  14 iconv.o
  15 iconv_close.o
  16 gconv_open.o
  17 gconv.o
  18 gconv_close.o
  19 gconv_db.o
  20 gconv_conf.o
  21 gconv_builtin.o
  22 gconv_simple.o
  23 gconv_trans.o
  24 gconv_cache.o
  25 gconv_dl.o
  26 catnames.o
  27 mb_cur_max.o
  28 setlocale.o
  29 findlocale.o
  30 loadlocale.o
  31 loadarchive.o
  32 localeconv.o
  33 nl_langinfo.o
  ......

# 查看符號表
$objdump -t libc.a
 5977 printf.o:     file format elf64-x86-64
 5978
 5979 SYMBOL TABLE:
 5980 0000000000000000 l    d  .text  0000000000000000 .text
 5981 0000000000000000 l    d  .data  0000000000000000 .data
 5982 0000000000000000 l    d  .bss   0000000000000000 .bss
 5983 0000000000000000 l    d  .comment   0000000000000000 .comment
 5984 0000000000000000 l    d  .note.GNU-stack    0000000000000000 .note.GNU-stack
 5985 0000000000000000 l    d  .eh_frame  0000000000000000 .eh_frame
 5986 0000000000000000 g     F .text  000000000000009e __printf
 5987 0000000000000000         *UND*  0000000000000000 stdout
 5988 0000000000000000         *UND*  0000000000000000 vfprintf
 5989 0000000000000000 g     F .text  000000000000009e printf
 5990 0000000000000000 g     F .text  000000000000009e _IO_printf
 ```
 
 ## 最小的程式
 
 一般的 Helloworld 使用 main 加 printf
 
不過這邊實際上會用到 C 的函示庫，這裡希望脫離函示庫的概念

但實際上測試後並沒有出現字串，不知是否是平臺問題
  
  
```c
char * str = "Hello world!\n";

void print()
{
    asm( "movl $13,%%edx \n\t"
         "movl $0,%%ecx  \n\t"
         "movl $0,%%ebx  \n\t"
         "movl $4,%%eax  \n\t"
         "int $0x80      \n\t"
         ::"r"(str):"edx","ecx","ebx");    
}

void exit()
{
    asm( "movl $42,%ebx  \n\t"
         "movl $1,%eax   \n\t"
         "int $0x80      \n\t");    
}

void nomain()
{
    print();
    exit();
}
```
