# 目的檔

## 目的檔格式

可執行檔格式(Executable)

```sh
Windows : PE  (Portable Executable)
Linux   : ELF (Executable Linkable Format)
Intel   : OMF (Object Module Format)
Unix    : .out
DOS     : .COM
```

以上都是 COFF(Common object file format) 格式的變化

動態連結程式庫

```sh
Windows : .dll
Linux   : .so
```

靜態連結程式庫

```sh
Windows : .lib
Linux   : .a
```


動態/靜態連結程式庫也是按照可執行檔格式儲存

##ELF 格式

```sh
Relocatable     File : 程式碼和資料，可連結成可執行檔(.o/.obj)
Executable      File : 可執行的程式(/bin/bash/.exe)
Shared Object   File : 可變為目的檔或連結其他共用目的檔和可執行檔(.so/.dll)
Core Dump       File : 當行程意外終止時，可將行程資訊 dump(core dump)
```

在 Linux 可用 file 指令看檔案格式

```sh
$ file hello.c
hello.c: C source, ASCII text

$ file hello.i
hello.i: C source, ASCII text

$ file hello.o  //目的檔也是可執行檔格式(relocatable)
hello.o: ELF 64-bit LSB  relocatable, x86-64, version 1 (SYSV), not stripped

$ file hello.s
hello.s: assembler source text

$ file /bin/bash  //可執行檔格式(executable)
/bin/bash: ELF 64-bit LSB  executable, x86-64, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24

$ file /lib32/ld-2.19.so  //動態連結程式庫(shared object)
/lib32/ld-2.19.so: ELF 32-bit LSB  shared object, Intel 80386, version 1 (SYSV), dynamically linked
```

## 目的檔內容

目的檔不外乎儲存編譯後的機器碼指令

還有連結需要的資訊(符號表、除錯資訊)

這些不同資訊按照屬性存在 Section 或稱為 Segment 以下稱段

機器碼放在程式碼區段，此段稱為 `.code 或 .text`

`全域變數和區域靜態變數`在資料區段，稱為`.data`

```c
int g_init_var  = 84;     // .data
int g_uninit_var;         // .bss

void func1(int i)         ---+
{                            |
    printf("%d\n",i);        |--> .text
}                            | 
                             |
int main(void)               |
{                         ---+
    static int static_var1 = 85;  // .data
    static int static_var2;       // .bss
     
    int a = 1;                    ---+
    int b;                           |
    func1(static_var1 +              |
          static_var2 + a + b);      |--> .text
                                     |
    return 0;                        |
}                                 ---+
```

Executable File / Object File

```sh
// 分成 4 個區塊
File Header
.text
.data
.bss
```

目的檔可用 objdump 看

```sh
Usage: objdump <option(s)> <file(s)>
 Display information from object <file(s)>.
 At least one of the following switches must be given:
  -a, --archive-headers    Display archive header information
  -f, --file-headers       Display the contents of the overall file header
  -p, --private-headers    Display object format specific file header contents
  -P, --private=OPT,OPT... Display object format specific contents
  -h, --[section-]headers  Display the contents of the section headers
  -x, --all-headers        Display the contents of all headers
  -d, --disassemble        Display assembler contents of executable sections
  -D, --disassemble-all    Display assembler contents of all sections
  -S, --source             Intermix source code with disassembly
  -s, --full-contents      Display the full contents of all sections requested
  -g, --debugging          Display debug information in object file
  -e, --debugging-tags     Display debug information using ctags style
  -G, --stabs              Display (in raw form) any STABS info in the file
  -W[lLiaprmfFsoRt] or
  --dwarf[=rawline,=decodedline,=info,=abbrev,=pubnames,=aranges,=macro,=frames,
          =frames-interp,=str,=loc,=Ranges,=pubtypes,
          =gdb_index,=trace_info,=trace_abbrev,=trace_aranges,
          =addr,=cu_index]
                           Display DWARF info in the file
  -t, --syms               Display the contents of the symbol table(s)
  -T, --dynamic-syms       Display the contents of the dynamic symbol table
  -r, --reloc              Display the relocation entries in the file
  -R, --dynamic-reloc      Display the dynamic relocation entries in the file
  @<file>                  Read options from <file>
  -v, --version            Display this program's version number
  -i, --info               List object formats and architectures supported
  -H, --help               Display this information
 ```
 
 編譯不同階段檔案格式 script
 
 ```sh
 #!/bin/sh
export file_name="objectTest"
gcc -E $file_name.c -o $file_name.i
gcc -S $file_name.i -o $file_name.s
gcc -c $file_name.s -o $file_name.o
```

不同程式碼編譯後存放的位址如下

```sh
機器碼                          -> .text
全域變數/區域靜態變數(初始化)   -> .data
全域變數/區域靜態變數(未初始化) -> .bss
```
bss 只是為初始化的全域變數/區域靜態變數預留位置，他沒有內容

所以在檔案中不占空間

程式指令和資料分開放比較好，主要優點如下


```sh
1.程式載入後，指令和資料對應 2 個虛擬記憶體空間
資料區段是 R/W，指令區段是 R，所以這樣可以保護指令不會被惡意改寫

2.分開放可以加強 CPU 快取命中率

3.如果執行相同程式，因為指令是一樣的所以可以共用，當然資料區段是各自擁有的
```

## 挖掘 Section


```sh
// 使用 objdump 來看 headers，還有個工具 readelf 也可使用
objdump -h SimpleSection.o

SimpleSection.o:     file format elf64-x86-64

Sections:
Idx Name            Size      VMA               LMA               File off  Algn
  0 .text           00000056  0000000000000000  0000000000000000  00000040  2**0
                    CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data           00000008  0000000000000000  0000000000000000  00000098  2**2
                    CONTENTS, ALLOC, LOAD, DATA
  2 .bss            00000004  0000000000000000  0000000000000000  000000a0  2**2
                    ALLOC
  3 .rodata         00000004  0000000000000000  0000000000000000  000000a0  2**0
                    CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment        0000002c  0000000000000000  0000000000000000  000000a4  2**0
                    CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000d0  2**0
                    CONTENTS, READONLY
  6 .eh_frame       00000058  0000000000000000  0000000000000000  000000d0  2**3
                    CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
                    
// readelf Header                  
readelf -h SimpleSection.o
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          400 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes)
  Number of section headers:         13
  Section header string table index: 10

// readelf Section 
readelf -S SimpleSection.o
There are 13 section headers, starting at offset 0x190:

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .text             PROGBITS         0000000000000000  00000040
       0000000000000056  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  000006b0
       0000000000000078  0000000000000018          11     1     8
  [ 3] .data             PROGBITS         0000000000000000  00000098
       0000000000000008  0000000000000000  WA       0     0     4
  [ 4] .bss              NOBITS           0000000000000000  000000a0
       0000000000000004  0000000000000000  WA       0     0     4
  [ 5] .rodata           PROGBITS         0000000000000000  000000a0
       0000000000000004  0000000000000000   A       0     0     1
  [ 6] .comment          PROGBITS         0000000000000000  000000a4
       000000000000002c  0000000000000001  MS       0     0     1
  [ 7] .note.GNU-stack   PROGBITS         0000000000000000  000000d0
       0000000000000000  0000000000000000           0     0     1
  [ 8] .eh_frame         PROGBITS         0000000000000000  000000d0
       0000000000000058  0000000000000000   A       0     0     8
  [ 9] .rela.eh_frame    RELA             0000000000000000  00000728
       0000000000000030  0000000000000018          11     8     8
  [10] .shstrtab         STRTAB           0000000000000000  00000128
       0000000000000061  0000000000000000           0     0     1
  [11] .symtab           SYMTAB           0000000000000000  000004d0
       0000000000000180  0000000000000018          12    11     8
  [12] .strtab           STRTAB           0000000000000000  00000650
       000000000000005d  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)
```

我們可以發現有許多不同的 Section

除了原本的 .text、.data、.bss，還多了 .rodata(唯讀資料區段)

.coment(註釋區段)、note.GUN-stack(堆疊提示區段)

再來看區段屬性，CONTENTS 表示此區段在檔案中存在

BSS 沒有 CONTENTS，表示他在 ELF 不存在內容

note.GUN-stack 雖然有 CONTENTS，但長度是 0

根據 Header 結構如下

```sh
$ ls -al
-rw-rw-r--  1 jason.cc.chiu jason.cc.chiu  1880  9月 22 13:22 SimpleSection.o
$ size SimpleSection.o
   text    data     bss     dec     hex filename
    178       8       4     190      be SimpleSection.o

        +----------------+ 0x0000_0758
  0x688 |   Other data   |        
        +----------------+ 0x0000_00d0
  0x2c  |   .commnt      |                     
        +----------------+ 0x0000_00a4
  0x04  |   .rodata      | 
        +----------------+ 0x0000_00a0
  0x08  |   .data        |                                           
        +----------------+ 0x0000_0098
  0x56  |   .text        |        
        +----------------+ 0x0000_0040
  0x40  |   ELF Header   |
        +----------------+ 0x0000_0000
```

使用 objdump 來看區段內容

```sh
// -d 是將指令區段顯示出來
objdump -s -d SimpleSection.o

SimpleSection.o:     file format elf64-x86-64

Contents of section .text: // Size = 0x56
 0000 554889e5 4883ec10 897dfc8b 45fc89c6  UH..H....}..E...
 0010 bf000000 00b80000 0000e800 000000c9  ................
 0020 c3554889 e54883ec 10c745f8 01000000  .UH..H....E.....
 0030 8b150000 00008b05 00000000 01c28b45  ...............E
 0040 f801c28b 45fc01d0 89c7e800 000000b8  ....E...........
 0050 00000000 c9c3                        ......
Contents of section .data:
 0000 54000000 55000000                    T...U...
Contents of section .rodata:
 0000 25640a00                             %d..
Contents of section .comment:
 0000 00474343 3a202855 62756e74 7520342e  .GCC: (Ubuntu 4.
 0010 382e342d 32756275 6e747531 7e31342e  8.4-2ubuntu1~14.
 0020 30342e33 2920342e 382e3400           04.3) 4.8.4.
Contents of section .eh_frame:
 0000 14000000 00000000 017a5200 01781001  .........zR..x..
 0010 1b0c0708 90010000 1c000000 1c000000  ................
 0020 00000000 21000000 00410e10 8602430d  ....!....A....C.
 0030 065c0c07 08000000 1c000000 3c000000  .\..........<...
 0040 00000000 35000000 00410e10 8602430d  ....5....A....C.
 0050 06700c07 08000000                    .p......

Disassembly of section .text:

0000000000000000 <func1>:
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
   4:   48 83 ec 10             sub    $0x10,%rsp
   8:   89 7d fc                mov    %edi,-0x4(%rbp)
   b:   8b 45 fc                mov    -0x4(%rbp),%eax
   e:   89 c6                   mov    %eax,%esi
  10:   bf 00 00 00 00          mov    $0x0,%edi
  15:   b8 00 00 00 00          mov    $0x0,%eax
  1a:   e8 00 00 00 00          callq  1f <func1+0x1f>
  1f:   c9                      leaveq
  20:   c3                      retq

0000000000000021 <main>:
  21:   55                      push   %rbp
  22:   48 89 e5                mov    %rsp,%rbp
  25:   48 83 ec 10             sub    $0x10,%rsp
  29:   c7 45 f8 01 00 00 00    movl   $0x1,-0x8(%rbp)
  30:   8b 15 00 00 00 00       mov    0x0(%rip),%edx        # 36 <main+0x15>
  36:   8b 05 00 00 00 00       mov    0x0(%rip),%eax        # 3c <main+0x1b>
  3c:   01 c2                   add    %eax,%edx
  3e:   8b 45 f8                mov    -0x8(%rbp),%eax
  41:   01 c2                   add    %eax,%edx
  43:   8b 45 fc                mov    -0x4(%rbp),%eax
  46:   01 d0                   add    %edx,%eax
  48:   89 c7                   mov    %eax,%edi
  4a:   e8 00 00 00 00          callq  4f <main+0x2e>
  4f:   b8 00 00 00 00          mov    $0x0,%eax
  54:   c9                      leaveq
  55:   c3                      retq

  
 /*  
 0000 55 ......
 0050 ...... c3    
 可以看出 55 是 func1 第 1 個 byte
 最後面 c3 是 main 最後 1 個 byte
 */
```

##資料區段和唯讀資料區段
資料區段 .data 儲存初始化的全域變數和靜態區域變數
前面程式碼中有兩個存在此區段


int 類型大小為 4 byte，所以 2 個變數為 8 byte

所以 .data 的 size 為 8 byte

printf 用到 "%d\n" 為字串常數，是唯讀資料

放在 .rodata，4 個 byte

.rodata 放唯讀資料，如 const 變數和字串常數


```sh
Contents of section .rodata:
 0000 25640a00                             %d..
 
 0x25 = %
 0x64 = d
 0x0a
 0x00 = 0
```

再來用 objdump 看看

```sh
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  1 .data         00000008  0000000000000000  0000000000000000  00000098  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  3 .rodata       00000004  0000000000000000  0000000000000000  000000a0  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
                  
Contents of section .data:
 0000 54000000 55000000                    T...U...
Contents of section .rodata:
 0000 25640a00                             %d..
 ```
 
 .data 前 4 byte = 54 00 00 00 = 84(10進位)
 
對應 int g_init_var = 84;

.data 後 4 byte = 55 00 00 00 = 85(10進位)

對應 static int static_var1 = 85;

而為什麼順序是 54 00 00 00 不是 00 00 00 54 ?

是關係到 CPU 的 byte order

##BSS
BSS 存放位初始化的全域變數和靜態區域變數

```c
int g_uninit_var;
static int static_var2;
```

兩個變數理論要 8 byte size 但 bss 只有 4 byte


```sh
2 .bss            00000004
```

我們可以透過符號表看到，只有 static_var2 有存入 bss

g_uninit_var 為未定義的 COMMON 符號


```sh
SYMBOL TABLE:
......
0000000000000004 l     O .data  0000000000000004 static_var1.2184
0000000000000000 l     O .bss   0000000000000004 static_var2.2185
0000000000000000 l    d  .note.GNU-stack        0000000000000000 .note.GNU-stack
......
0000000000000004       O *COM*  0000000000000004 g_uninit_var
......
```

要注意的是初始化指派為 0 的變數還是會放在 BSS ，例如

```sh
sataic int x1 = 0;
sataic int x2 = 1;
```

x1 放 .bss 而 x2 放 .data，因為指派 0 會被認定還未初始化

最佳化後會放在 .bss，這樣可以節省空間

##其他區段

```sh
.rodatel    : 唯讀資料
.comment    : 編譯器資訊
.debug      : 除錯資訊
.dynamic    : 動態連結資訊
.hash       : 符號雜湊表
.line       : 除錯行號表
.note       : 額外編譯器資訊
.strtab 
.symtab
.shstrtab
.plt
.got
.init
.fini
```

## 將檔案作為目的檔區段

我們要將一個 size 為 0x48D 的 image.jpg 作為目的檔中的一個區段


```sh
# 將 image.jpg 轉為 image.o
$ objcopy -I binary -O elf64-x86-64 -B i386 image.jpg image.o
$ objdump -ht image.o
image.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .data         0000048d  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, DATA
SYMBOL TABLE:
0000000000000000 l    d  .data  0000000000000000 .data
0000000000000000 g       .data  0000000000000000 _binary_image_jpg_start
000000000000048d g       .data  0000000000000000 _binary_image_jpg_end
000000000000048d g       *ABS*  0000000000000000 _binary_image_jpg_size
```

現在可以在程式中直接宣告並使用

一般 GCC 編譯原始檔後，會把程式碼放到 .text，全域變數和靜態變數放 .data 和 .bss

GCC 提供一中方法可以讓你將變數、方法放到指定的區段

`__attribute__((section("SectionName")))`


```c
#include <stdio.h>

int g_init_var  = 84;     
int g_uninit_var;         
__attribute__((section("FOO"))) int g_foo  = 42;


void func1(int i)        
{                            
    printf("%d\n",i);
}

__attribute__((section("BAR"))) void func_bar()
{                            
    printf("func_bar\n");
}

int main(void)
{                        
    static int static_var1 = 85;  // .data
    static int static_var2;       // .bss
     
    int a = 1;
    int b;  
    func1(static_var1 + static_var2 + a + b);
    return 0;                        
}  

objdump -ht SimpleSection.o

SimpleSection.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  3 FOO           00000004  0000000000000000  0000000000000000  000000a0  2**2
                  CONTENTS, ALLOC, LOAD, DATA

  5 BAR           00000010  0000000000000000  0000000000000000  000000b1  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE

0000000000000000 l    d  FOO    0000000000000000 FOO
0000000000000000 l    d  BAR    0000000000000000 BAR
```

##ELF 結構描述
目前 ELF 結構如下


```sh
+---------------------+
|     ELF Header      |
+---------------------+
|       .text         |
+---------------------+
|       .data         |
+---------------------+
|       .bss          |
+---------------------+
|       ....          |
+---------------------+
|  Section Header TLB |
+---------------------+
|     String TLB      |
+---------------------+
|     Symbol TLB      |
+---------------------+
```

先看ELF Header