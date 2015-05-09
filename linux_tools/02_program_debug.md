# 程序調試
目錄

程序調試
進程調試
gdb 程序交互調試
pstack 跟蹤棧空間
strace 分析系統調用
目標文件分析
nm
objdump
readelf
size 查看程序內存佔用
file 文件類型查詢
strings 查詢數據中的文本信息
fuser 顯示文件使用者
xxd 十六進制顯示數據
od
2.1. 進程調試
gdb 程序交互調試
GDB是一個由GNU開源組織發佈的、UNIX/LINUX操作系統下的、基於命令行的、功能強大的程序調試工具。

對於一名Linux下工作的c++程序員，gdb是必不可少的工具；

GDB中的命令固然很多，但我們只需掌握其中十個左右的命令，就大致可以完成日常的基本的程序調試工作。

以下從一個完整的調試過程簡單說明最基本的幾個命令;
```
$gdb programmer     # 啓動gdb
>break main         # 設置斷點
>run                # 運行調試程序
>next               # 單步調試
>print var1         # 在調試過程中，我們需要查看當前某個變量值的時候，使用print 命令打印該值
>list               # 顯示當前調試處的源代碼
>info b             # 顯示當前斷點設置情況
```

當你完成了第一個程序調試之後，你當然會需要更多的命令：關於gdb常用命令及各種調試方法詳見 gdb 調試利器 ;

同時，你需要更高效的調試：常用的調試命令都會有單字符的縮寫，使用縮寫更方便；同時，直接敲回車表示重複執行上一步命令；這在單步調試時非常有用；

pstack 跟蹤棧空間
pstack是一個腳本工具，可顯示每個進程的棧跟蹤。pstack 命令必須由相應進程的屬主或 root 運行。其核心實現就是使用了gdb以及thread apply all bt命令;

語法:
```
$pstrack <program-pid>
```

示例:
```
$ pstack 4551
Thread 7 (Thread 1084229984 (LWP 4552)):
#0  0x000000302afc63dc in epoll_wait () from /lib64/tls/libc.so.6
#1  0x00000000006f0730 in ub::EPollEx::poll ()
#2  0x00000000006f172a in ub::NetReactor::callback ()
#3  0x00000000006fbbbb in ub::UBTask::CALLBACK ()
#4  0x000000302b80610a in start_thread () from /lib64/tls/libpthread.so.0
#5  0x000000302afc6003 in clone () from /lib64/tls/libc.so.6
#6  0x0000000000000000 in ?? ()
```

strace 分析系統調用
strace常用來跟蹤進程執行時的系統調用和所接收的信號。在Linux世界，進程不能直接訪問硬件設備，當進程需要訪問硬件設備(比如讀取磁盤文件，接收網絡數據等等)時，必須由用戶態模式切換至內核態模式，通過系統調用訪問硬件設備。strace可以跟蹤到一個進程產生的系統調用,包括參數，返回值，執行消耗的時間。

完整程序:
```
strace -o output.txt -T -tt -e trace=all -p 28979
```

跟蹤28979進程的所有系統調用（-e trace=all），並統計系統調用的花費時間，以及開始時間（以可視化的時分秒格式顯示），最後將記錄結果存在output.txt文件裏面。

查看進程正在做什麼(實時輸出進程執行系統調用的情況):
```
$strace -p <process-pid>
```

關於strace的詳細介紹，詳見 strace 跟蹤進程中的系統調用 ;

2.2. 目標文件分析
nm
nm用來列出目標文件的符號清單。
```
$nm myProgrammer
08049f28 d _DYNAMIC
08049ff4 d _GLOBAL_OFFSET_TABLE_
080484dc R _IO_stdin_used
         w _Jv_RegisterClasses
08049f18 d __CTOR_END__
08049f14 d __CTOR_LIST__
08049f20 D __DTOR_END__
08049f1c d __DTOR_LIST__
080485e0 r __FRAME_END__
08049f24 d __JCR_END__
08049f24 d __JCR_LIST__
0804a014 A __bss_start
0804a00c D __data_start
08048490 t __do_global_ctors_aux
08048360 t __do_global_dtors_aux
0804a010 D __dso_handle
         w __gmon_start__
08048482 T __i686.get_pc_thunk.bx
08049f14 d __init_array_end
08049f14 d __init_array_start
08048480 T __libc_csu_fini
08048410 T __libc_csu_init
         U __libc_start_main@@GLIBC_2.0
0804a014 A _edata
0804a01c A _end
080484bc T _fini
080484d8 R _fp_hw
080482b4 T _init
08048330 T _start
0804a014 b completed.6086
0804a00c W data_start
0804a018 b dtor_idx.6088
080483c0 t frame_dummy
080483e4 T main
         U printf@@GLIBC_2.0
```

這些包含可執行代碼的段稱爲正文段。同樣地，數據段包含了不可執行的信息或數據。另一種類型的段，稱爲 BSS 段，它包含以符號數據開頭的塊。對於 nm 命令列出的每個符號，它們的值使用十六進制來表示（缺省行爲），並且在該符號前面加上了一個表示符號類型的編碼字符。

常見的各種編碼包括：

A 表示絕對 (absolute)，這意味着不能將該值更改爲其他的連接；
B 表示 BSS 段中的符號；
C 表示引用未初始化的數據的一般符號。
可以將目標文件中所包含的不同的部分劃分爲段。段可以包含可執行代碼、符號名稱、初始數據值和許多其他類型的數據。有關這些類型的數據的詳細信息，可以閱讀 UNIX 中 nm 的 man 頁面，其中按照該命令輸出中的字符編碼分別對每種類型進行了描述。

在目標文件階段，即使是一個簡單的 Hello World 程序，其中也包含了大量的細節信息。nm 程序可用於列舉符號及其類型和值，但是，要更仔細地研究目標文件中這些命名段的內容，需要使用功能更強大的工具。

其中兩種功能強大的工具是 objdump 和 readelf 程序。

註解

關於nm工具的參數說明及更多示例詳見 nm 目標文件格式分析 ;
objdump
ogjdump工具用來顯示二進制文件的信息，就是以一種可閱讀的格式讓你更多地瞭解二進制文件可能帶有的附加信息。

```
$objdump -d myprogrammer
a.out:     file format elf32-i386


Disassembly of section .init:

080482b4 <_init>:
 80482b4:   53                      push   %ebx
 80482b5:   83 ec 08                sub    $0x8,%esp
 80482b8:   e8 00 00 00 00          call   80482bd <_init+0x9>
 80482bd:   5b                      pop    %ebx
 80482be:   81 c3 37 1d 00 00       add    $0x1d37,%ebx
 80482c4:   8b 83 fc ff ff ff       mov    -0x4(%ebx),%eax
 80482ca:   85 c0                   test   %eax,%eax
 80482cc:   74 05                   je     80482d3 <_init+0x1f>
 80482ce:   e8 3d 00 00 00          call   8048310 <__gmon_start__@plt>
 80482d3:   e8 e8 00 00 00          call   80483c0 <frame_dummy>
 80482d8:   e8 b3 01 00 00          call   8048490 <__do_global_ctors_aux>
 80482dd:   83 c4 08                add    $0x8,%esp
 80482e0:   5b                      pop    %ebx
 80482e1:   c3                      ret

Disassembly of section .plt:
...
```

每個可執行代碼段將在需要特定的事件時執行，這些事件包括庫的初始化和該程序本身主入口點。

對於那些着迷於底層編程細節的程序員來說，這是一個功能非常強大的工具，可用於研究編譯器和彙編器的輸出。細節信息，比如這段代碼中所顯示的這些信息，可以揭示有關本地處理器本身運行方式的很多內容。對該處理器製造商提供的技術文檔進行深入的研究，您可以收集關於一些有價值的信息，通過這些信息可以深入地瞭解內部的運行機制，因爲功能程序提供了清晰的輸出。

註解

關於objdump工具的參數說明及更多示例詳見 objdump 二進制文件分析 ;
readelf
這個工具和objdump命令提供的功能類似，但是它顯示的信息更爲具體，並且它不依賴BFD庫(BFD庫是一個GNU項目，它的目標就是希望通過一種統一的接口來處理不同的目標文件）；

```
$readelf -all a.out
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Intel 80386
  Version:                           0x1
  Entry point address:               0x8048330
  Start of program headers:          52 (bytes into file)
  Start of section headers:          4412 (bytes into file)
  Flags:                             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         9
  Size of section headers:           40 (bytes)
  Number of section headers:         30
  Section header string table index: 27

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        08048154 000154 000013 00   A  0   0  1
  [ 2] .note.ABI-tag     NOTE            08048168 000168 000020 00   A  0   0  4
  [ 3] .note.gnu.build-i NOTE            08048188 000188 000024 00   A  0   0  4
  [ 4] .gnu.hash         GNU_HASH        080481ac 0001ac 000020 04   A  5   0  4
  [ 5] .dynsym           DYNSYM          080481cc 0001cc 000050 10   A  6   1  4
  [ 6] .dynstr           STRTAB          0804821c 00021c 00004c 00   A  0   0  1
  [ 7] .gnu.version      VERSYM          08048268 000268 00000a 02   A  5   0  2
  [ 8] .gnu.version_r    VERNEED         08048274 000274 000020 00   A  6   1  4
  [ 9] .rel.dyn          REL             08048294 000294 000008 08   A  5   0  4
  [10] .rel.plt          REL             0804829c 00029c 000018 08   A  5  12  4
  [11] .init             PROGBITS        080482b4 0002b4 00002e 00  AX  0   0  4
  [12] .plt              PROGBITS        080482f0 0002f0 000040 04  AX  0   0 16
  [13] .text             PROGBITS        08048330 000330 00018c 00  AX  0   0 16
  [14] .fini             PROGBITS        080484bc 0004bc 00001a 00  AX  0   0  4
  [15] .rodata           PROGBITS        080484d8 0004d8 000011 00   A  0   0  4
  [16] .eh_frame_hdr     PROGBITS        080484ec 0004ec 000034 00   A  0   0  4
  [17] .eh_frame         PROGBITS        08048520 000520 0000c4 00   A  0   0  4
  [18] .ctors            PROGBITS        08049f14 000f14 000008 00  WA  0   0  4
  [19] .dtors            PROGBITS        08049f1c 000f1c 000008 00  WA  0   0  4
  [20] .jcr              PROGBITS        08049f24 000f24 000004 00  WA  0   0  4
  [21] .dynamic          DYNAMIC         08049f28 000f28 0000c8 08  WA  6   0  4
  [22] .got              PROGBITS        08049ff0 000ff0 000004 04  WA  0   0  4
  [23] .got.plt          PROGBITS        08049ff4 000ff4 000018 04  WA  0   0  4
  [24] .data             PROGBITS        0804a00c 00100c 000008 00  WA  0   0  4
  [25] .bss              NOBITS          0804a014 001014 000008 00  WA  0   0  4
  [26] .comment          PROGBITS        00000000 001014 00002a 01  MS  0   0  1
  [27] .shstrtab         STRTAB          00000000 00103e 0000fc 00      0   0  1
  [28] .symtab           SYMTAB          00000000 0015ec 000410 10     29  45  4
  [29] .strtab           STRTAB          00000000 0019fc 0001f9 00      0   0  1
  ...
  ```
ELF Header 爲該文件中所有段入口顯示了詳細的摘要。在列舉出這些 Header 中的內容之前，您可以看到 Header 的具體數目。在研究一個較大的目標文件時，該信息可能非常有用。

除了所有這些段之外，編譯器可以將調試信息放入到目標文件中，並且還可以顯示這些信息。輸入下面的命令，仔細分析編譯器的輸出（假設您扮演了調試程序的角色）:
```
$readelf --debug-dump a.out | more
```

調試工具，如 GDB，可以讀取這些調試信息，並且當程序在調試器中運行的同時，您可以使用該工具顯示更具描述性的標記，而不是對代碼進行反彙編時的原始地址值。

註解

關於readelf工具的參數說明及更多示例詳見 readelf elf文件格式分析 ;
size 查看程序內存佔用
size這個工具用來查看程序運行時各個段的實際內存佔用:

```
$size a.out
text           data     bss     dec     hex filename
1146            256       8    1410     582 a.out
```

file 文件類型查詢
這個工具用於查看文件的類型；

比如我們在64位機器上發現了一個32位的庫，鏈接不上，這就有問題了：
```
$file a.out
a.out: ELF 64-bit LSB executable, AMD x86-64, version 1 (SYSV), for GNU/Linux 2.6.9, dynamically linked (uses shared libs), for GNU/Linux 2.6.9, not stripped
```

也可以查看Core文件是由哪個程序生成:
```
$file core.22355
```

strings 查詢數據中的文本信息
一個文件中包含二進制數據和文本數據，如果只需要查看其文本信息，使用這個命令就很方便；過濾掉非字符數據，將文本信息輸出:
```
$strings <objfile>
```
fuser 顯示文件使用者
顯示所有正在使用着指定的file, file system 或者 sockets的進程信息;

```
$fuser -m -u redis-server
redis-server: 11552rce(weber) 22912rce(weber) 25501rce(weber)
```

使用了-m和-u選項，用來查找所有正在使用redis-server的所有進程的PID以及該進程的OWNER；

fuser通常被用在診斷系統的”resource busy”問題。如果你希望kill所有正在使用某一指定的file, file system or sockets的進程的時候，你可以使用-k選項:
```
$fuser –k /path/to/your/filename
```

xxd 十六進制顯示數據
以十六進制方式顯示文件，只顯示文本信息:
```
$xxd a.out
0000000: 7f45 4c46 0101 0100 0000 0000 0000 0000  .ELF............
0000010: 0200 0300 0100 0000 3083 0408 3400 0000  ........0...4...
0000020: 3c11 0000 0000 0000 3400 2000 0900 2800  <.......4. ...(.
0000030: 1e00 1b00 0600 0000 3400 0000 3480 0408  ........4...4...
0000040: 3480 0408 2001 0000 2001 0000 0500 0000  4... ... .......
0000050: 0400 0000 0300 0000 5401 0000 5481 0408  ........T...T...
...
```

od
通常使用od命令查看特殊格式的文件內容。通過指定該命令的不同選項可以以十進制、八進制、十六進制和ASCII碼來顯示文件。

參數說明：

-A 指定地址基數，包括：

d 十進制
o 八進制（系統默認值）
x 十六進制
n 不打印位移值
-t 指定數據的顯示格式，主要的參數有：

c ASCII字符或反斜槓序列
d 有符號十進制數
f 浮點數
o 八進制（系統默認值爲02）
u 無符號十進制數
x 十六進制數
除了選項c以外的其他選項後面都可以跟一個十進制數n，指定每個顯示值所包含的字節數。

說明：od命令系統默認的顯示方式是八進制，這也是該命令的名稱由來（Octal Dump）。但這不是最有用的顯示方式，用ASCII碼和十六進制組合的方式能提供更有價值的信息輸出。

以十六進制和字符同時顯示:
```
$od -Ax -tcx4 a.c
000000   #   i   n   c   l   u   d   e       <   s   t   d   i   o   .
              636e6923        6564756c        74733c20        2e6f6964
000010   h   >  \n  \n   v   o   i   d       m   a   i   n   (   )  \n
              0a0a3e68        64696f76        69616d20        0a29286e
000020   {  \n  \t   i   n   t       i       =       5   ;  \n  \t   p
              69090a7b        6920746e        35203d20        70090a3b
000030   r   i   n   t   f   (   "   h   e   l   l   o   ,   %   d   "
              746e6972        68222866        6f6c6c65        2264252c
000040   ,   i   )   ;  \n   }  \n
              3b29692c        000a7d0a
000047
```

以字符方式顯示:
```
$od -c a.c
0000000   #   i   n   c   l   u   d   e       <   s   t   d   i   o   .
0000020   h   >  \n  \n   v   o   i   d       m   a   i   n   (   )  \n
0000040   {  \n  \t   i   n   t       i       =       5   ;  \n  \t   p
0000060   r   i   n   t   f   (   "   h   e   l   l   o   ,   %   d   "
0000100   ,   i   )   ;  \n   }  \n
0000107
```

注：類似命令還有hexdump（十六進制輸出）
