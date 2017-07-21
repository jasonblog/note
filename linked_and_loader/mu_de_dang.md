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


```sh
$ readelf -h SimpleSection.o
ELF Header:
  # 魔數
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00   
  # 檔案機器位元長度
  Class:                             ELF64  
  # 資料儲存方式
  Data:                              2's complement, little endian  
  # 版本
  Version:                           1 (current)  
  # 執行平台
  OS/ABI:                            UNIX - System V
  # ABI 版本
  ABI Version:                       0
  # ELF 重定類型
  Type:                              REL (Relocatable file)
  # 硬體平台
  Machine:                           Advanced Micro Devices X86-64
  # 硬體平台版本
  Version:                           0x1
  # 入口位址
  Entry point address:               0x0
  # 程式標頭入口和長度、段頭表的位址和長度、區段數量
  Start of program headers:          0 (bytes into file)
  Start of section headers:          400 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes)
  Number of section headers:         13
  Section header string table index: 10
 ```
 
 ELF 檔案在各平台下通用，ELF 分為 32 和 64 位元兩種版本
 
可參考/usr/include/elf.h

例如如果是 32 位元，Elf32_Addr = uint32_t = 4 byte


```c
30 /* Type for a 16-bit quantity.  */
 31 typedef uint16_t Elf32_Half;
 32 typedef uint16_t Elf64_Half;
 33
 34 /* Types for signed and unsigned 32-bit quantities.  */
 35 typedef uint32_t Elf32_Word;
 36 typedef int32_t  Elf32_Sword;
 37 typedef uint32_t Elf64_Word;
 38 typedef int32_t  Elf64_Sword;
 39
 40 /* Types for signed and unsigned 64-bit quantities.  */
 41 typedef uint64_t Elf32_Xword;
 42 typedef int64_t  Elf32_Sxword;
 43 typedef uint64_t Elf64_Xword;
 44 typedef int64_t  Elf64_Sxword;
 45
 46 /* Type of addresses.  */
 47 typedef uint32_t Elf32_Addr;
 48 typedef uint64_t Elf64_Addr;
 49
 50 /* Type of file offsets.  */
 51 typedef uint32_t Elf32_Off;
 52 typedef uint64_t Elf64_Off;
 53
 54 /* Type for section indices, which are 16-bit quantities.  */
 55 typedef uint16_t Elf32_Section;
 56 typedef uint16_t Elf64_Section;
 57
 58 /* Type for version symbol information.  */
 59 typedef Elf32_Half Elf32_Versym;
 60 typedef Elf64_Half Elf64_Versym;

 67 typedef struct
 68 {
 69   unsigned char e_ident[EI_NIDENT];    /* Magic number and other info */
 70   Elf32_Half    e_type;                /* Object file type */
 71   Elf32_Half    e_machine;             /* Architecture */
 72   Elf32_Word    e_version;             /* Object file version */
 73   Elf32_Addr    e_entry;               /* Entry point virtual address */
 74   Elf32_Off e_phoff;                   /* Program header table file offset */
 75   Elf32_Off e_shoff;                   /* Section header table file offset */
 76   Elf32_Word    e_flags;               /* Processor-specific flags */
 77   Elf32_Half    e_ehsize;              /* ELF header size in bytes */
 78   Elf32_Half    e_phentsize;           /* Program header table entry size */
 79   Elf32_Half    e_phnum;               /* Program header table entry count */
 80   Elf32_Half    e_shentsize;           /* Section header table entry size */
 81   Elf32_Half    e_shnum;               /* Section header table entry count */
 82   Elf32_Half    e_shstrndx;            /* Section header string table index */
 83 } Elf32_Ehdr;  
 84
 85 typedef struct
 86 {
 87   unsigned char e_ident[EI_NIDENT];    /* Magic number and other info */
 88   Elf64_Half    e_type;                /* Object file type */
 89   Elf64_Half    e_machine;             /* Architecture */
 90   Elf64_Word    e_version;             /* Object file version */
 91   Elf64_Addr    e_entry;               /* Entry point virtual address */
 92   Elf64_Off e_phoff;                   /* Program header table file offset */
 93   Elf64_Off e_shoff;                   /* Section header table file offset */
 94   Elf64_Word    e_flags;               /* Processor-specific flags */
 95   Elf64_Half    e_ehsize;              /* ELF header size in bytes */
 96   Elf64_Half    e_phentsize;           /* Program header table entry size */
 97   Elf64_Half    e_phnum;               /* Program header table entry count */
 98   Elf64_Half    e_shentsize;           /* Section header table entry size */
 99   Elf64_Half    e_shnum;               /* Section header table entry count */
100   Elf64_Half    e_shstrndx;            /* Section header string table index */
101 } Elf64_Ehdr;
```

可以看出 ELF Header 符合 Elf(32/64)_Ehdr 結構的定義


```c
# EI_NIDENT (16)

ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00          /* e_ident[EI_NIDENT];*/
  Class:                             ELF64                          /* e_ident[EI_NIDENT];*/
  Data:                              2's complement, little endian  /* e_ident[EI_NIDENT];*/
  Version:                           1 (current)                    /* e_ident[EI_NIDENT];*/
  OS/ABI:                            UNIX - System V                /* e_ident[EI_NIDENT];*/
  ABI Version:                       0                              /* e_ident[EI_NIDENT];*/
  Type:                              REL (Relocatable file)         /* e_type */
  Machine:                           Advanced Micro Devices X86-64  /* e_machine */
  Version:                           0x1                            /* e_version */
  Entry point address:               0x0                            /* e_entry */
  Start of program headers:          0 (bytes into file)            /* e_phoff */
  Start of section headers:          400 (bytes into file)          /* e_shoff */
  Flags:                             0x0                            /* e_flags */
  Size of this header:               64 (bytes)                     /* e_ehsize */
  Size of program headers:           0 (bytes)                      /* e_phentsize */
  Number of program headers:         0                              /* e_phnum */
  Size of section headers:           64 (bytes)                     /* e_shentsize */
  Number of section headers:         13                             /* e_shnum */
  Section header string table index: 10                             /* e_shstrndx */
 ```
 
 ## 魔數(Magic)
 
 Magic 為 16 byte，用來標示 ELF 平台屬性
 
 
 ```sh
# 一般可執行檔前面幾個 byte 為魔數
# OS 在載入執行檔前會先檢查魔數是否正確
7f = DEL
45 = E
4c = L
46 = F

# ELF 檔案格式
# 0 = 無效
# 1 = 32 位元 
# 2 = 64 位元
02 

# Byte Order
# 0 = 無效
# 1 = 小端 
# 2 = 大端
01 

# ELF 版本
01 

# 後面目前無定義
00 00 00 00 00 00 00 00 00
```
Byte Order 為位元組順序，Endian 源自格列佛遊記

小人國對吃水煮蛋分兩派，一派為要從大頭先敲開(Big Endian)

一派為要從小頭先敲開(Little Endian)，為此常常發生內戰

MSB 是 Most Significant Bit/Byte

代表最重要的位元/位元組，他表示在一個 bit/byte 中對順序取值影響最大的那個 bit/byte

LSB 是 Least Significant Bit/Byte 和 MSB 相反

也就是在一個 bit/byte 中對順序取值影響最小的那個 bit/byte

```sh
例如: 0x12345678
0x12 0x34 0x56 0x78
0x12 = MSB
0X78 = LSB

如果單看 0x78 = 0111 1000
最左邊 bit = 0 = MSB
最又邊 bit = 0 = LSB
```

bit-endian 規定

MSB 儲存時放在低位址，傳輸時放在資料流開始

LSB 儲存時放在高位址，傳輸時放在資料流結尾

```sh
            Big     Little
0 byte      0x12    0x78
1 byte      0x34    0x56
2 byte      0x56    0x34
3 byte      0x78    0x21
```

## 檔案類型(Type)

OS 透過此欄位判斷檔案類型，而不是透過副檔名，

檔案類型為常數，以`ET`開頭


```sh
    /* Legal values for e_type (object file type).  */
162 #define ET_NONE     0              /* No file type */
163 #define ET_REL      1              /* Relocatable file */
164 #define ET_EXEC     2              /* Executable file */
165 #define ET_DYN      3              /* Shared object file */
166 #define ET_CORE     4              /* Core file */
167 #define ET_NUM      5              /* Number of defined types */
168 #define ET_LOOS     0xfe00         /* OS-specific range start */
169 #define ET_HIOS     0xfeff         /* OS-specific range end */
170 #define ET_LOPROC   0xff00         /* Processor-specific range start */
171 #define ET_HIPROC   0xffff         /* Processor-specific range end */
```

## 機器類型(Machine)

此欄位表示可在哪個平台使用，例如 Advanced Micro Devices X86-64 就是能在 x86 下使用
相關常數以`EM` 開頭

```sh
173 /* Legal values for e_machine (architecture).  */
174
175 #define EM_NONE      0         /* No machine */
176 #define EM_M32       1         /* AT&T WE 32100 */
177 #define EM_SPARC     2         /* SUN SPARC */
178 #define EM_386       3         /* Intel 80386 */
179 #define EM_68K       4         /* Motorola m68k family */
180 #define EM_88K       5         /* Motorola m88k family */
181 #define EM_860       7         /* Intel 80860 */
182 #define EM_MIPS      8         /* MIPS R3000 big-endian */
183 #define EM_S370      9         /* IBM System/370 */
184 #define EM_MIPS_RS3_LE  10     /* MIPS R3000 little-endian */
186 #define EM_PARISC   15         /* HPPA */
187 #define EM_VPP500   17         /* Fujitsu VPP500 */
188 #define EM_SPARC32PLUS  18     /* Sun's "v8plus" */
189 #define EM_960      19         /* Intel 80960 */
190 #define EM_PPC      20         /* PowerPC */
191 #define EM_PPC64    21         /* PowerPC 64-bit */
192 #define EM_S390     22         /* IBM S390 */
194 #define EM_V800     36         /* NEC V800 series */
195 #define EM_FR20     37         /* Fujitsu FR20 */
196 #define EM_RH32     38         /* TRW RH-32 */
197 #define EM_RCE      39         /* Motorola RCE */
198 #define EM_ARM      40         /* ARM */
省略......
```

## 段頭表(Start of section headers)

段頭表描述 ELF 個區段的資訊，如區段的區段名稱、區段長度

檔案中的偏移、R/W 權限、其他屬性

編譯器、連結器、載入器都是依靠此表決定定位和存取各區段屬性

範例位於 400 = 0x190 的偏移位址


```sh
# objdump 只會顯示關鍵的區段
$ objdump -h SimpleSection.o

SimpleSection.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000056  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000008  0000000000000000  0000000000000000  00000098  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000004  0000000000000000  0000000000000000  000000a0  2**2
                  ALLOC
  3 .rodata       00000004  0000000000000000  0000000000000000  000000a0  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002c  0000000000000000  0000000000000000  000000a4  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000d0  2**0
                  CONTENTS, READONLY
  6 .eh_frame     00000058  0000000000000000  0000000000000000  000000d0  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
                  
# readelf 可以顯示全部的區段                  
$ readelf -S SimpleSection.o
There are 13 section headers, starting at offset 0x190:

Section Headers:
  [Nr] Name              Type             Address           Offset    Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000  0000000000000000  0000000000000000           0     0     0
  [ 1] .text             PROGBITS         0000000000000000  00000040  0000000000000056  0000000000000000  AX       0     0     1
  [ 2] .rela.text        RELA             0000000000000000  000006b0  0000000000000078  0000000000000018          11     1     8
  [ 3] .data             PROGBITS         0000000000000000  00000098  0000000000000008  0000000000000000  WA       0     0     4
  [ 4] .bss              NOBITS           0000000000000000  000000a0  0000000000000004  0000000000000000  WA       0     0     4
  [ 5] .rodata           PROGBITS         0000000000000000  000000a0  0000000000000004  0000000000000000   A       0     0     1
  [ 6] .comment          PROGBITS         0000000000000000  000000a4  000000000000002c  0000000000000001  MS       0     0     1
  [ 7] .note.GNU-stack   PROGBITS         0000000000000000  000000d0  0000000000000000  0000000000000000           0     0     1
  [ 8] .eh_frame         PROGBITS         0000000000000000  000000d0  0000000000000058  0000000000000000   A       0     0     8
  [ 9] .rela.eh_frame    RELA             0000000000000000  00000728  0000000000000030  0000000000000018          11     8     8
  [10] .shstrtab         STRTAB           0000000000000000  00000128  0000000000000061  0000000000000000           0     0     1
  [11] .symtab           SYMTAB           0000000000000000  000004d0  0000000000000180  0000000000000018          12    11     8
  [12] .strtab           STRTAB           0000000000000000  00000650  000000000000005d  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)
  ```
  
  區段表是以Elf(32/64)_Shdr結構的陣列
  
所以陣列內元素個數 = 區段總個數 = There are 13 section headers，共 13 個

但是第一個是無效的，所以有效的有 12 個


```sh
270 /* Section header.  */
 271
 272 typedef struct
 273 {
 274   Elf32_Word    sh_name;        /* Section name (string tbl index) */
 275   Elf32_Word    sh_type;        /* Section type */
 276   Elf32_Word    sh_flags;       /* Section flags */
 277   Elf32_Addr    sh_addr;        /* Section virtual addr at execution */
 278   Elf32_Off     sh_offset;      /* Section file offset */
 279   Elf32_Word    sh_size;        /* Section size in bytes */
 280   Elf32_Word    sh_link;        /* Link to another section */
 281   Elf32_Word    sh_info;        /* Additional section information */
 282   Elf32_Word    sh_addralign;   /* Section alignment */
 283   Elf32_Word    sh_entsize;     /* Entry size if section holds table */
 284 } Elf32_Shdr;
 285
 286 typedef struct
 287 {
 288   Elf64_Word    sh_name;        /* Section name (string tbl index) */
 289   Elf64_Word    sh_type;        /* Section type */
 290   Elf64_Xword   sh_flags;       /* Section flags */
 291   Elf64_Addr    sh_addr;        /* Section virtual addr at execution */
 292   Elf64_Off     sh_offset;      /* Section file offset */
 293   Elf64_Xword   sh_size;        /* Section size in bytes */
 294   Elf64_Word    sh_link;        /* Link to another section */
 295   Elf64_Word    sh_info;        /* Additional section information */
 296   Elf64_Xword   sh_addralign;   /* Section alignment */
 297   Elf64_Xword   sh_entsize;     /* Entry size if section holds table */
 298 } Elf64_Shdr;
 
# 各區段成員作用
sh_name     # 區段名稱，位於 .shstrtab 的字串表中
sh_type     # 區段類型
sh_flags    # 區段旗標
sh_addr     # 區段虛擬位址
sh_offset   # 區段偏移
sh_size     # 區段長度
sh_link     # 區段連結
sh_info     # 區段資訊
sh_addralign# 區段位址對其
sh_entsize  # 區段項目長度
```

所以 ELF 結構如下


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
|     Section TLB     |
|  用來描述 Section   |
+---------------------+
|      .symtab        |
+---------------------+
|      .rel.text      |
+---------------------+
```

再來看 Type(sh_type) 和 Flags(sh_flags)


```sh
317 /* Legal values for sh_type (section type).  */
318
319 #define SHT_NULL           0           /* Section header table entry unused */
320 #define SHT_PROGBITS       1           /* Program data 程式碼區段 */
321 #define SHT_SYMTAB         2           /* Symbol table 符號表 */
322 #define SHT_STRTAB         3           /* String table 字串表 */
323 #define SHT_RELA           4           /* Relocation entries with addends 重定表 */
324 #define SHT_HASH           5           /* Symbol hash table 符號表的雜湊 */
325 #define SHT_DYNAMIC        6           /* Dynamic linking information 動態連結資訊 */
326 #define SHT_NOTE           7           /* Notes 提示資訊*/
327 #define SHT_NOBITS         8           /* Program space with no data (bss) 沒有內容*/
328 #define SHT_REL            9           /* Relocation entries, no addends 重定資訊 */
329 #define SHT_SHLIB          10          /* Reserved 保留*/
330 #define SHT_DYNSYM         11          /* Dynamic linker symbol table 動態連結符號表*/
331 #define SHT_INIT_ARRAY     14          /* Array of constructors */
332 #define SHT_FINI_ARRAY     15          /* Array of destructors */
333 #define SHT_PREINIT_ARRAY  16          /* Array of pre-constructors */
334 #define SHT_GROUP          17          /* Section group */
335 #define SHT_SYMTAB_SHNDX   18          /* Extended section indeces */
336 #define SHT_NUM            19          /* Number of defined types.  */
337 #define SHT_LOOS           0x60000000  /* Start OS-specific.  */
338 #define SHT_GNU_ATTRIBUTES 0x6ffffff5  /* Object attributes.  */
339 #define SHT_GNU_HASH       0x6ffffff6  /* GNU-style hash table.  */
340 #define SHT_GNU_LIBLIST    0x6ffffff7  /* Prelink library list */
341 #define SHT_CHECKSUM       0x6ffffff8  /* Checksum for DSO content.  */
342 #define SHT_LOSUNW         0x6ffffffa  /* Sun-specific low bound.  */
343 #define SHT_SUNW_move      0x6ffffffa
344 #define SHT_SUNW_COMDAT    0x6ffffffb
345 #define SHT_SUNW_syminfo   0x6ffffffc
346 #define SHT_GNU_verdef     0x6ffffffd  /* Version definition section.  */
347 #define SHT_GNU_verneed    0x6ffffffe  /* Version needs section.  */
348 #define SHT_GNU_versym     0x6fffffff  /* Version symbol table.  */
349 #define SHT_HISUNW         0x6fffffff  /* Sun-specific high bound.  */
350 #define SHT_HIOS           0x6fffffff  /* End OS-specific type */
351 #define SHT_LOPROC         0x70000000  /* Start of processor-specific */
352 #define SHT_HIPROC         0x7fffffff  /* End of processor-specific */
353 #define SHT_LOUSER         0x80000000  /* Start of application-specific */
354 #define SHT_HIUSER         0x8fffffff  /* End of application-specific */

356 /* Legal values for sh_flags (section flags).  */
358 #define SHF_WRITE              (1 << 0)   /* Writable 此區段可寫*/
359 #define SHF_ALLOC              (1 << 1)   /* Occupies memory during execution 此區段在 process 需要分配空間*/
360 #define SHF_EXECINSTR          (1 << 2)   /* Executable 此區段可執行 */
361 #define SHF_MERGE              (1 << 4)   /* Might be merged */
362 #define SHF_STRINGS            (1 << 5)   /* Contains nul-terminated strings */
363 #define SHF_INFO_LINK          (1 << 6)   /* `sh_info' contains SHT index */
364 #define SHF_LINK_ORDER         (1 << 7)   /* Preserve order after combining */
365 #define SHF_OS_NONCONFORMING   (1 << 8)   /* Non-standard OS specific handling required */
367 #define SHF_GROUP              (1 << 9)   /* Section is member of a group.  */
368 #define SHF_TLS                (1 << 10)  /* Section hold thread-local data.  */
369 #define SHF_MASKOS             0x0ff00000 /* OS-specific.  */
370 #define SHF_MASKPROC           0xf0000000 /* Processor-specific */
371 #define SHF_ORDERED            (1 << 30)  /* Special ordering requirement (Solaris).  */
373 #define SHF_EXCLUDE            (1 << 31)  /* Section is excluded unless referenced or allocated (Solaris).*/
```

## 重定表
有一個區段叫做.rela.text，類型為sh_type，代表對 .text 的重定

##連結的介面(符號)

兩個目的檔要連結實際上之間對位址的引用

假設 B 要用 A 中的函式，則 A 定義此函式

B 引用此函式，函式和變數為符號，函式名稱和變數名稱為符號名稱

每一個目的檔都有對應的符號表，紀錄目的檔用的所有符號，符號分類如下

```c
#include <stdio.h>

int g_init_var  = 84;     
int g_uninit_var;         

void func1(int i)        
{                            
    printf("%d\n",i);
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


# 全域符號
func1、main、g_init_var

# 外部符號
printf

# 區段名稱(由編譯器產生)
.text、.data

# 區域符號
static_var1、static_var2

# 行號資訊
```

符號表可以使用`nm`或`readelf`
連結過程只關心全域符號和外部符號


```sh
$ nm SimpleSection.o
0000000000000000 T func1
0000000000000000 D g_init_var
0000000000000004 C g_uninit_var
0000000000000021 T main
                 U printf
0000000000000004 d static_var1.2184
0000000000000000 b static_var2.2185


$ readelf -s SimpleSection.o

Symbol table '.symtab' contains 16 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS SimpleSection.c
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    3
     4: 0000000000000000     0 SECTION LOCAL  DEFAULT    4
     5: 0000000000000000     0 SECTION LOCAL  DEFAULT    5
     6: 0000000000000004     4 OBJECT  LOCAL  DEFAULT    3 static_var1.2184
     7: 0000000000000000     4 OBJECT  LOCAL  DEFAULT    4 static_var2.2185
     8: 0000000000000000     0 SECTION LOCAL  DEFAULT    7
     9: 0000000000000000     0 SECTION LOCAL  DEFAULT    8
    10: 0000000000000000     0 SECTION LOCAL  DEFAULT    6
    11: 0000000000000000     4 OBJECT  GLOBAL DEFAULT    3 g_init_var
    12: 0000000000000004     4 OBJECT  GLOBAL DEFAULT  COM g_uninit_var
    13: 0000000000000000    33 FUNC    GLOBAL DEFAULT    1 func1
    14: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND printf
    15: 0000000000000021    53 FUNC    GLOBAL DEFAULT    1 main
```

符號表也是區段之一，一般叫做`.symtab`


```sh
Section Headers:
  [Nr] Name              Type             Address           Offset    Size              EntSize          Flags  Link  Info  Align
  [11] .symtab           SYMTAB           0000000000000000  000004d0  0000000000000180  0000000000000018          12    11     8
```

符號表的結構如下，對應上面的 Num、Value、Size、Type、Bind、Vis、Ndx Name


```sh
379 /* Symbol table entry.  */
380
381 typedef struct
382 {
383   Elf32_Word    st_name;        /* Symbol name (string tbl index) */
384   Elf32_Addr    st_value;       /* Symbol value */
385   Elf32_Word    st_size;        /* Symbol size */
386   unsigned char st_info;        /* Symbol type and binding */
387   unsigned char st_other;       /* Symbol visibility */
388   Elf32_Section st_shndx;       /* Section index */
389 } Elf32_Sym;
390
391 typedef struct
392 {
393   Elf64_Word    st_name;        /* Symbol name (string tbl index) */
394   unsigned char st_info;        /* Symbol type and binding */
395   unsigned char st_other;       /* Symbol visibility */
396   Elf64_Section st_shndx;       /* Section index */
397   Elf64_Addr    st_value;       /* Symbol value */
398   Elf64_Xword   st_size;        /* Symbol size */
399 } Elf64_Sym;
```