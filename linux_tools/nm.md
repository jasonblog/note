# nm 目標文件格式分析

nm 命令顯示關於指定 File 中符號的信息，文件可以是對象文件、可執行文件或對象文件庫。如果文件沒有包含符號信息，nm 命令報告該情況，但不把它解釋爲出錯條件。 nm 命令缺省情況下報告十進制符號表示法下的數字值。
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

可以將目標文件中所包含的不同的部分劃分爲段。段可以包含可執行代碼、符號名稱、初始數據值和許多其他類型的數據。有關這些類型的數據的詳細信息，可以閱讀 UNIX 中 nm 的 man 頁面，其中按照該命令輸出中的字符編碼分別對每種類型進行了描述。

### 15.1. 選項說明
- -a或–debug-syms：顯示所有的符號，包括debugger-only symbols。
- -B：等同於–format=bsd，用來兼容MIPS的nm。
- -C或–demangle：將低級符號名解析(demangle)成用戶級名字。這樣可以使得C++函數名具有可讀性。
- –no-demangle：默認的選項，不需要將低級符號名解析成用戶級名。
- -D或–dynamic：顯示動態符號。該任選項僅對於動態目標(例如特定類型的共享庫)有意義。
- -f format：使用format格式輸出。format可以選取bsd、sysv或posix，該選項在GNU的nm中有用。默認爲bsd。
- -g或–extern-only：僅顯示外部符號。
- -n、-v或–numeric-sort：按符號對應地址的順序排序，而非按符號名的字符順序。
- -p或–no-sort：按目標文件中遇到的符號順序顯示，不排序。
- -P或–portability：使用POSIX.2標準輸出格式代替默認的輸出格式。等同於使用任選項-f posix。
- -s或–print-armap：當列出庫中成員的符號時，包含索引。索引的內容包含：哪些模塊包含哪些名字的映射。
- -r或–reverse-sort：反轉排序的順序(例如，升序變爲降序)。
- –size-sort：按大小排列符號順序。該大小是按照一個符號的值與它下一個符號的值進行計算的。
- –target=bfdname：指定一個目標代碼的格式，而非使用系統的默認格式。
- -u或–undefined-only：僅顯示沒有定義的符號(那些外部符號)。
- –defined-only:僅顯示定義的符號。
- -l或–line-numbers：對每個符號，使用調試信息來試圖找到文件名和行號。
- -V或–version：顯示nm的版本號。
- –help：顯示nm的選項。


### 15.2. 符號說明
對於每一個符號來說，其類型如果是小寫的，則表明該符號是local的；大寫則表明該符號是global(external)的。

- A 該符號的值是絕對的，在以後的鏈接過程中，不允許進行改變。這樣的符號值，常常出現在中斷向量表中，例如用符號來表示各箇中斷向量函數在中斷向量表中的位置。
- B 該符號的值出現在非初始化數據段(bss)中。例如，在一個文件中定義全局static int test。則該符號test的類型爲b，位於bss section中。其值表示該符號在bss段中的偏移。一般而言，bss段分配於RAM中。
- C 該符號爲common。common symbol是未初始話數據段。該符號沒有包含於一個普通section中。只有在鏈接過程中才進行分配。符號的值表示該符號需要的字節數。例如在一個c文件中，定義int test，並且該符號在別的地方會被引用，則該符號類型即爲C。否則其類型爲B。
- D 該符號位於初始化數據段中。一般來說，分配到data section中。
例如：定義全局int baud_table[5] = {9600, 19200, 38400, 57600, 115200}，會分配到初始化數據段中。

- G 該符號也位於初始化數據段中。主要用於small object提高訪問small data object的一種方式。
- I 該符號是對另一個符號的間接引用。
- N 該符號是一個debugging符號。
- R 該符號位於只讀數據區。
例如定義全局const int test[] = {123, 123};則test就是一個只讀數據區的符號。
值得注意的是，如果在一個函數中定義const char *test = “abc”, const char test_int = 3。使用nm都不會得到符號信息，但是字符串”abc”分配於只讀存儲器中，test在rodata section中，大小爲4。
- S 符號位於非初始化數據區，用於small object。
- T 該符號位於代碼區text section。
- U 該符號在當前文件中是未定義的，即該符號的定義在別的文件中。
例如，當前文件調用另一個文件中定義的函數，在這個被調用的函數在當前就是未定義的；但是在定義它的文件中類型是T。但是對於全局變量來說，在定義它的文件中，其符號類型爲C，在使用它的文件中，其類型爲U。

- V 該符號是一個weak object。
- W The symbol is a weak symbol that has not been specifically tagged as a weak object symbol.
? 該符號類型沒有定義
庫或對象名 如果您指定了 -A 選項，則 nm 命令只報告與該文件有關的或者庫或者對象名。

### 15.3. 示例
尋找特殊標識
有時會碰到一個編譯了但沒有鏈接的代碼，那是因爲它缺失了標識符；這種情況，可以用nm和objdump、readelf命令來查看程序的符號表；所有這些命令做的工作基本一樣；

比如連接器報錯有未定義的標識符；大多數情況下，會發生在庫的缺失或企圖鏈接一個錯誤版本的庫的時候；瀏覽目標代碼來尋找一個特殊標識符的引用:

nm -uCA *.o | grep foo
-u選項限制了每個目標文件中未定義標識符的輸出。-A選項用於顯示每個標識符的文件名信息；對於C++代碼，常用的還有-C選項，它也爲解碼這些標識符；

註解

objdump、readld命令可以完成同樣的任務。等效命令爲： $objdump -t $readelf -s
列出 a.out 對象文件的靜態和外部符:
```
$nm -e a.out
```

以十六進制顯示符號大小和值並且按值排序符號:
```
$nm -xv a.out
```
顯示 libc.a 中所有 64 位對象符號，忽略所有 32 位對象:
```
$nm -X64 /usr/lib/libc.a
```
