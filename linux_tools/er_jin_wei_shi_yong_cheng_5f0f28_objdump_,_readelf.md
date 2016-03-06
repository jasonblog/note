# 二進位實用程式(objdump, readelf,ar, nm等)


GNU 二進位實用程式Binutils (GNU binary utilities)包括：`objdump、readelf、addr2line、strip、ar、nm、ldd、ngprof、gcov`等。
##一。 objdump - 顯示二進位檔案資訊
objdump可以根據目的檔案來生成可讀性比較好的彙編檔。常用的命令如下：

```sh
gcc -g3 test.c -o test.o 
objdump -Slz test.o 
```

objdump命令介紹，可以通過`man objdump`在linux環境下獲取到。 

```sh
  [-a] [-b bfdname | 
       --target=bfdname] [-C] [--debugging] 
       [-d] [-D] 
       [--disassemble-zeroes] 
       [-EB|-EL|--endian={big|little}] [-f] 
       [-h] [-i|--info] 
       [-j section | --section=section] 
       [-l] [-m machine ] [--prefix-addresses] 
       [-r] [-R] 
       [-s|--full-contents] [-S|--source] 
       [--[no-]show-raw-insn] [--stabs] [-t] 
       [-T] [-x] 
       [--start-address=address] [--stop-address=address] 
       [--adjust-vma=offset] [--version] [--help] 
       objfile... 
  -a, --archive-headers    Display archive header information 
  -f, --file-headers       Display the contents of the overall file header 
  -p, --private-headers    Display object format specific file header contents 
  -h, --[section-]headers  Display the contents of the section headers 
  -x, --all-headers        Display the contents of all headers 
  -d, --disassemble        Display assembler contents of executable sections 
  -D, --disassemble-all    Display assembler contents of all sections 
  -S, --source             Intermix source code with disassembly 
  -s, --full-contents      Display the full contents of all sections requested 
  -g, --debugging          Display debug information in object file 
  -e, --debugging-tags     Display debug information using ctags style 
  -G, --stabs              Display (in raw form) any STABS info in the file 
  -W, --dwarf              Display DWARF info in the file 
  -t, --syms               Display the contents of the symbol table(s) 
  -T, --dynamic-syms       Display the contents of the dynamic symbol table 
  -r, --reloc              Display the relocation entries in the file 
  -R, --dynamic-reloc      Display the dynamic relocation entries in the file 
  @                  Read options from 
  -v, --version            Display this program's version number 
  -i, --info               List object formats and architectures supported 
  -H, --help               Display this information
  
--archive-headers 
-a 顯示檔案庫的成員資訊，與 ar tv 類似
     objdump -a libpcap.a 
     和 ar -tv libpcap.a 顯示結果比較比較 
     顯然這個選項沒有什麼意思。
--adjust-vma=offset 
     When   dumping   information, first add offset to all 
     the section addresses.   This is useful if the   sec- 
     tion   addresses   do   not correspond   to the symbol 
     table, which can happen when   putting   sections   at 
     particular   addresses when using a format which can 
     not represent section addresses, such as a.out.
-b bfdname 
--target=bfdname 
     指定目的碼格式。這不是必須的，objdump能自動識別許多格式， 
     比如：objdump -b oasys -m vax -h fu.o 
     顯示fu.o的頭部摘要資訊，明確指出該檔是Vax系統下用Oasys 
     編譯器生成的目的檔案。objdump -i將給出這裡可以指定的 
     目的碼格式列表
--demangle 
-C 將底層的符號名解碼成使用者級名字，除了去掉所有開頭 
    的底線之外，還使得C++函數名以可理解的方式顯示出來。
--debugging 
     顯示調試資訊。企圖解析保存在檔中的調試資訊並以C語言 
     的語法顯示出來。僅僅支援某些類型的調試資訊。
--disassemble 
-d 反彙編那些應該還有指令機器碼的section
--disassemble-all 
-D 與 -d 類似，但反彙編所有section
--prefix-addresses 
     反彙編的時候，顯示每一行的完整位址。這是一種比較老的反彙編格式。 
     顯示效果並不理想，但可能會用到其中的某些顯示，自己可以對比。
--disassemble-zeroes 
     一般反彙編輸出將省略大塊的零，該選項使得這些零塊也被反彙編。
-EB 
-EL 
--endian={big|little} 
     這個選項將影響反彙編出來的指令。 
     little-endian就是我們當年在dos下玩彙編的時候常說的高位在高位址， 
     x86都是這種。
--file-headers 
-f 顯示objfile中每個檔的整體頭部摘要資訊。
--section-headers 
--headers 
-h 顯示目的檔案各個section的頭部摘要資訊。
--help 簡短的説明資訊。
--info 
-i 顯示對於 -b 或者 -m 選項可用的架構和目標格式清單。
--section=name 
-j name 僅僅顯示指定section的資訊
--line-numbers 
-l 用檔案名和行號標注相應的目標代碼，僅僅和-d、-D或者-r一起使用 
    使用-ld和使用-d的區別不是很大，在源碼級調試的時候有用，要求 
    編譯時使用了-g之類的調試編譯選項。
--architecture=machine 
-m machine 
     指定反彙編目的檔案時使用的架構，當待反彙編檔本身沒有描述 
     架構資訊的時候(比如S-records)，這個選項很有用。可以用-i選項 
     列出這裡能夠指定的架構
--reloc 
-r 顯示檔的重定位入口。如果和-d或者-D一起使用，重定位部分以反匯 
    編後的格式顯示出來。
--dynamic-reloc 
-R 顯示檔的動態重定位入口，僅僅對於動態目的檔案有意義，比如某些 
    共用庫。
--full-contents 
-s 顯示指定section的完整內容。
     objdump --section=.text -s inet.o | more
--source 
-S 盡可能反彙編出原始程式碼，尤其當編譯的時候指定了-g這種調試參數時， 
    效果比較明顯。隱含了-d參數。
--show-raw-insn 
     反彙編的時候，顯示每條彙編指令對應的機器碼，除非指定了 
     --prefix-addresses，這將是缺省選項。
--no-show-raw-insn 
     反彙編時，不顯示彙編指令的機器碼，這是指定 --prefix-addresses 
     選項時的缺省設置。
--stabs 
     Display the contents of the .stab, .stab.index, and 
     .stab.excl sections from an ELF file.   This is only 
     useful   on   systems   (such as Solaris 2.0) in which 
     .stab debugging symbol-table entries are carried in 
     an ELF section. &
```

##二。 readelf -- 顯示elf檔資訊
LINUX 平臺下三種主要的可執行檔格式：a.out（assembler and link editor output 彙編器和連結編輯器的輸出）、COFF（Common Object File Format 通用物件檔案格式）、ELF（Executable and Linking Format 可執行和連結格式）。
a.out 檔包含 7 個 section，格式如下：
```sh
exec header（執行頭部，也可理解為檔頭部）
text segment（文本段）
data segment(資料段)
text relocations(文本重定位段)
data relocations（資料重定位段）
symbol table（符號表）
string table（字串表）
```
執行頭部的資料結構：

```c
struct exec {
    unsigned long a_midmag; /* 魔數和其它資訊 */
    unsigned long a_text; /* 文本段的長度 */
    unsigned long a_data; /* 資料段的長度 */
    unsigned long a_bss; /* BSS段的長度 */
    unsigned long a_syms; /* 符號表的長度 */
    unsigned long a_entry; /* 程式進入點 */
    unsigned long a_trsize; /* 文本重定位表的長度 */
    unsigned long a_drsize; /* 資料重定位表的長度 */

};
```

a.out 的格式非常緊湊，只包含了程式運行所必須的資訊（文本、資料、BSS），而且每個 section 的順序是固定的。這種結構缺乏擴展性，a.out 檔中包含符號表和兩個重定位表，這三個表的內容在連接目的檔案以生成可執行檔時起作用。在最終可執行的 a.out 文件中，這三個表的長度都為 0。a.out 檔在連接時就把所有外部定義包含在可執行程式中，如果從程式設計的角度來看，這是一種硬編碼方式，或者可稱為模組之間是強藕和的。a.out 是早期UNIX系統使用的可執行檔格式，由 AT&T 設計，現在基本上已被 ELF 檔案格式代替。a.out 的設計比較簡單，但其設計思想明顯的被後續的可執行檔格式所繼承和發揚。這裡我們著重介紹elf格式。


### 1. elf格式介紹
Executable and linking format(ELF)檔是Linux系統下的一種常用目的檔案(object file)格式，有三種主要類型:

- (1)適於連接的可重定位文件(relocatable file)，可與其它目的檔案一起創建可執行檔和共用目的檔案 （.obj or .o）

- (2)適於執行的可執行檔(executable file)，用於提供程式的進程映射，載入的記憶體執行。

- (3)共用目的檔案(shared object file),連接器可將它與其它可重定位文件和共用目的檔案連接成其它的目的檔案，動態連接器又可將它與可執行檔和其它共用目的檔案結合起來創建一個進 程映射。

ELF檔內容有兩個平行的視角:一個是程式連接角度，另一個是程式運行角度。
ELF header在檔開始處描述了整個檔的組織，Section提供了目的檔案的各項資訊（如指令、資料、符號表、重定位資訊等），Program header table指出怎樣創建進程映射，含有每個program header的入口，Section header table包含每一個section的入口，給出名字、大小等資訊。
 
段由若干個節(Section)構成,節頭表對每一個節的資訊有相關描述。對可執行程式而言，節頭表是可選的。ELF頭部是一個關於本檔的路線圖 （road map），從總體上描述檔的結構。下麵是ELF頭部的資料結構：

```c
typedef struct {
    unsigned char e_ident[EI_NIDENT]; /* 魔數和相關資訊 */
    Elf32_Half e_type; /* 目的檔案類型 */
    Elf32_Half e_machine; /* 硬體體系 */
    Elf32_Word e_version; /* 目的檔案版本 */
    Elf32_Addr e_entry; /* 程式進入點 */
    Elf32_Off e_phoff; /* 程式頭部偏移量 */
    Elf32_Off e_shoff; /* 節頭部偏移量 */
    Elf32_Word e_flags; /* 處理器特定標誌 */
    Elf32_Half e_ehsize; /* ELF頭部長度 */
    Elf32_Half e_phentsize; /* 程式頭部中一個條目的長度 */
    Elf32_Half e_phnum; /* 程式頭部條目個數 */
    Elf32_Half e_shentsize; /* 節頭部中一個條目的長度 */
    Elf32_Half e_shnum; /* 節頭部條目個數 */ Elf32_Half
    e_shstrndx; /* 節頭部字元表索引 */

} Elf32_Ehdr;
```

###2.  readelf 命令
readelf命令可以顯示符號、段資訊、二進位檔案格式的資訊等，這在分析編譯器如何工從原始程式碼創建二進位檔案時非常有用。
```sh
$ readelf -h a.out
```
```sh
ELF Header:
Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
Class:
ELF64
Data:                              2's complement, little endian
Version:                           1 (current)
OS/ABI:                            UNIX - System V
ABI Version:                       0
Type:                              EXEC (Executable file)
Machine:                           Advanced Micro Devices X86-64
Version:                           0x1
Entry point address:               0x4003c0
Start of program headers:          64 (bytes into file)
Start of section headers:          2704 (bytes into file)
Flags:                             0x0
Size of this header:               64 (bytes)
Size of program headers:           56 (bytes)
Number of program headers:         8
Size of section headers:           64 (bytes)
Number of section headers:         29
Section header string table index: 26
```

##三。 addr2line -- 將位址對應到檔案名和行號

##四。 as -- GNU彙編器

as工具主要用來將組合語言編寫的來源程式轉換成二進位形式的目標代碼。Linux平臺的標準彙編器是GAS，它是Gnu GCC編譯器所依賴的後臺彙編工具，通常包含在Binutils套裝軟體中。

## 五。 ld -- GNU連結程式
同as一樣，ld也是GNU Binutils工具集中重要的工具，Linux使用ld作為標準的連結程式，由彙編器產生的目標代碼是不能直接在電腦上運行的，它必須經過連結器的處 理才能生成可執行代碼，連結是創建一個可執行程式的最後一個步驟，ld可以將多個目的檔案連結成為可執行程式，同時指定了程式在運行時是如何執行的。

## 六。 ar

ar命令可以用來創建、修改庫，也可以從庫中提出單個模組。庫是一單獨的檔，裡面包含了按照特定的結構組織起來的其它的一些文件（稱做此庫文件的 member）。原始檔的內容、模式、時間戳記、屬主、組等屬性都保留在庫檔中。有關ar命令使用的例子，（引自http://blog.163.com/huang_bp/blog/getBlog.do?bid=fks_083075087083082069083086083095085084082066085095094064083）：

####1. 要創建一個庫，請輸入：

```sh
ar -v -q lib.a strlen.o strcpy.o
```
如果 lib.a 庫不存在，則此命令創建它，並將文件 strlen.o 和 strcpy.o 的副本輸入其中。如果 lib.a 庫存在，則此命令在不檢查相同成員的情況下，將新的成員添加到末尾。v 標誌設置詳細方式，在此方式中 ar命令在其進行時顯示進程報告。

####2.     要顯示庫的目錄，請輸入： 
```sh
ar -v -t lib.a
```
此命令列出了 lib.a 庫的目錄，顯示類似於 ls -l 命令的輸出的長列表。要只列出成員檔案名稱，則省略 -v標誌。

####3.     要替換或添加新成員到庫中，請輸入： 
```sh
ar -v -r lib.a strlen.o strcat.o
```
此命令替換成員 strlen.o 和 strcat.o。如果 lib.a 如示例 1 中顯示的那樣創建，則替換 strlen.o 成員。因為不存在名為 strcat.o 的成員，所以它被添加到庫的末尾。

####4.     要指定在何處插入新成員，請輸入： 
```sh
ar -v -r -b strlen.o lib.a strcmp.o
```

此命令添加 strcmp.o 文件，並將該新成員置於 strlen.o 成員之前。
####5.     要更新一個已經更改過的成員，請輸入： 

```sh
ar -v -r -u lib.a strcpy.o
```
此命令替換現有 strcpy.o 成員，但僅當文件 strcpy.o 自從最後一次添加到庫後已經修改時才替換它。
####6.     要更改庫成員的順序，請輸入： 
```sh
ar -v -m -a strcmp.o lib.a strcat.o strcpy.o
```
此命令將成員 strcat.o 和 strcpy.o 移動到緊跟在 strcmp.o 成 員之後的位置。保留 strcat.o 和 strcpy.o 成員的相對順序。換句話說，如果在移動之前 strcpy.o 成員在 strcat.o 成員之前，那麼（移動後）它依舊如此。
####7.     要解壓縮庫成員，請輸入： 
```sh
ar -v -x lib.a strcat.o strcpy.o
```

此命令將成員 strcat.o 和 strcpy.o 分別複製到名為 strcat.o 和 strcpy.o 的文件。
####8.     要解壓縮並重命名一個成員，請輸入： 

```sh
ar -p lib.a strcpy.o >stringcopy.o
```
此命令將成員 strcpy.o 複製到一個名為 stringcopy.o 的文件。
####9.     要刪除一個成員，請輸入： 
```sh
ar -v -d lib.a strlen.o
```
此命令從 lib.a 庫中刪除成員 strlen.o。

####10.   要從多個用 ld 命令創建的共用模組中創建一個壓縮文件庫，請輸入： 
```sh
ar -r -v libshr.a shrsub.o shrsub2.o shrsub3.o ...
```
此命令從名為 shrsub.o、shrsub2.o、shrsub3.o 等等的共用模組中創建名為 libshr.a 的壓縮文件庫。要編譯並連結使用 libshr.a 壓縮文件庫的 main 程式，請使用以下命令：
cc -o main main.c -L/u/sharedlib -lshr
main 程式現在是可執行的。main 程式引用的任何符號（包含在libshr.a 壓縮文件庫中）已經因延遲解析度而作了標記。-l 標誌指定應在 libshr.a 庫中搜索這些符號。
####11.   要列出 lib.a 的內容（忽略任何 32 位目的檔案），請輸入： 
```sh
ar -X64 -t -v lib.a
```
####12.   要從 lib.a 解壓縮所有 32 位的目的檔案，請輸入： 
```sh
ar -X32 -x lib.a
```
####13.   要列出 lib.a 中的所有檔，無論是 32 位、64 位元或非物件，請輸入： 
```sh
ar -X32_64 -t -v lib.a
```

## 七。nm

nm用來列出目的檔案的符號清單。下麵是nm命令的格式：

```sh
nm[-a|--debug-syms][-g|--extern-only][-B] 
[-C|--demangle][-D|--dynamic][-s|--print-armap] 
[-o|--print-file-name][-n|--numeric-sort] 
[-p|--no-sort][-r|--reverse-sort][--size-sort] 
[-u|--undefined-only][-l|--line-numbers][--help] 
[--version][-tradix|--radix=radix] 
[-P|--portability][-fformat|--format=format] 
[--target=bfdname][objfile...] 
```
如果沒有為nm命令指出目的檔案，則nm假定目的檔案是a.out。下面列出該命令的任選項，大部分支援“-”開頭的短格式和“-“開頭的長格 式。 -A、-o或--print-file-name：在找到的各個符號的名字前加上檔案名，而不是在此檔的所有符號前只出現檔案名一次。


## 八。objcopy
它可以把目的檔案的內容從一種檔案格式複製到另一種格式的目的檔案中。通過objcopy可以實現對elf不同模組的分析，同時實現類似strip 的功能。

##九。ldd
ldd 命令可以用來產看應用程式對庫的依賴關係，在應用程式移植過程中很有用。可以首先確認要移植平時是否支援應用程式所依賴的庫。例如：

```sh
$ ldd aout 
        libc.so.6 => /lib64/tls/libc.so.6 (0x0000002a9566c000) 
        /lib64/ld-linux-x86-64.so.2 (0x0000002a95556000)
```


##十。size
size 列出目的模組或檔的代碼尺寸
```sh
$ size a.out 
   text    data     bss     dec     hex filename 
   1241     512       8    1761     6e1 a.out
```

##十一。ranlib
ranlib 生成索引以加快對歸檔檔的訪問，並將結果保存到這個歸檔檔中，在索引中列出了歸檔檔各個成員所定義的可重分配目的檔案。ar -s可以實現類似的功能。
##十二。strings
strings 列印可列印的目標代碼字元（至少4個字元），列印字元多少可以控制.對於其它各式的檔，列印字串。列印某個檔的可列印字串，這些字串最少4個字 符長，也可以使用選項“-n”設置字串的最小長度。預設情況下，它只列印目的檔案初始化和可載入段中的可列印字元；對於其他類型的的檔它列印整個檔 的可列印字元，這個程式對於瞭解非文字檔內容很有説明。
##十三。strip
刪除目的檔案中的全部或者特定符號，這樣可以減小可執行檔的大小。在嵌入式應用中，可執行檔一般存放在flash中，空間有限，因此在產品 release的過程中採用strip對程式進行裁剪很有必要。


##十三。gprof
gprof是Linux下一個強有力的程式分析工具。能夠以“日誌”的形式記錄程式運行時的統計資訊：程式運行中各個函數消耗的時間和函式呼叫關 系，以及每個函數被調用的次數等等。從而可以説明程式師找出眾多函數中耗時最多的函數，也可以説明程式師分析程式的運行流程。相信這些功能對於分析開源代 碼的程式師來說，有著相當大的誘惑力;同時我們也可以借助gprof進行性能優化和分析。
用gprof對程式進行分析主要分以下三個步驟：

- 用編譯器對程式進行編譯，加上-pg參數。
-  運行編譯後的程式。
-  用gprof命令查看程式的運行時資訊。

先以一個簡單的例子演示一下吧。隨便找一個能夠運行的程式的原始程式碼，比如下面的檔test.c：

```c
int IsEven(int x)
{
    return 0 == x & 1;
}
int main(int argc, char* argv[] {
    int i = 0;

    while (++i < 1000) IsEven(i);
}
```

首先，用以下命令進行編譯：
```sh
[root@localhost]#gcc –o test –pg test.c
```

然後，運行可執行檔test
```sh
[root@localhost]#./test
```

運行後，在目前的目錄下將生成一個檔`gmon.out`，這就是gprof生成的檔，保存有程式運行期間函式呼叫等資訊。
最後，用gprof命令查看gmon.out保存的資訊：
```sh
[root@localhost]#gprof test gmon.out –b
```
這樣就有一大堆資訊輸出到螢幕上，有函數執行單間，函式呼叫關係圖等等，如下：

```sh
Flat profile:
Each sample counts as 0.01 seconds.
no time accumulated
  %   cumulative   self              self     total          
time   seconds   seconds    calls  Ts/call  Ts/call  name   
  0.00      0.00     0.00     1000     0.00     0.00  IsEven(int)
                     Call graph
granularity: each sample hit covers 2 byte(s) no time propagated
index % time    self  children    called     name
                0.00    0.00    1000/1000        main [7]
[8]      0.0    0.00    0.00    1000         IsEven(int) [8]
-----------------------------------------------
Index by function name
   [8] IsEven(int)
```

以上介紹了gprof最簡單的使用方法，下面針對其使用過程中的三個步驟詳細說明。
編譯和連結
上面的例子中，程式比較簡單，只有一個檔。如果原始程式碼有多個檔，或者代碼結構比較複雜，編譯過程中先生成若干個目的檔案，然後又由連結器將這些 目的檔案連結到一起，這時該怎麼使用gprof呢？
對於由多個原始檔案組成的程式，編譯時需要在生成每個.o檔的時候加上-pg參數，同時在連結的時候也要加上-pg參數。對於連結器不是GCC的情 況，如ld，又有特殊的要求。
同時，-pg參數只能記錄原始程式碼中各個函數的調用關係，而不能記錄庫函數的調用情況。要想記錄每個庫函數的調用情況，連結的時候必須指定庫函數的動 態（或者靜態）程式庫libc_p.a，即加上-lc_p，而不是-lc。
還要說明的是，如果有一部分代碼在編譯時指定了-pg參數，而另一部分代碼沒有指定，則生成的gmon.out檔中將缺少一部分函數，也沒有那些 函數的調用關係。但是並不影響gprof對其它函數進行記錄。
運行
編譯好的程式運行時和運行一般的程式沒有什麼不同，只是比正常的程式多生成了一個檔gmon.out。注意，這個檔案名是固定的，沒法通過參數的 設置進行改變。如果程式目錄中已經有一個gmon.out，則它會被新的gmon.out覆蓋掉。
關於生成的gmon.out檔所在的目錄，也有以下約定：程式退出時所運行的檔所在目錄就是生成的gmon.out檔所在的目錄。如果一個程 序執行過程中調用了另一個程式，並在另一個程式的運行中終止，則gmon.out會在另一個程式所在的目錄中生成。
還有一點要注意的就是當程式非正常終止時不會生成gmon.out檔，也因此就沒法查看程式運行時的資訊。只有當程式從main函數中正常退出， 或者通過系統調用exit()函數而退出時，才會生成gmon.out文件。而通過底層調用如_exit()等退出時不會生成gmon.out。
查看
查看程式運行資訊的命令是gprof，它以gmon.out檔作為輸入，也就是將gmon.out檔翻譯成可讀的形式展現給用戶。其命令格式如 下：
```sh
gprof [可執行檔] [gmon.out文件] [其它參數]
```

方括號中的內容可以省略。如果省略了“可執行檔”，gprof會在目前的目錄下搜索a.out檔作為可執行檔，而如果省略了gmon.out文 件，gprof也會在目前的目錄下尋找gmon.out。其它參數可以控制gprof輸出內容的格式等資訊。最常用的參數如下：

```sh
-b 不再輸出統計圖表中每個欄位的詳細描述。
-p 只輸出函數的調用圖（Call graph的那部分資訊）。
-q 只輸出函數的時間消耗清單。
-e Name 不再輸出函數Name 
```

及其子函數的調用圖（除非它們有未被限制的其它父函數）。可以給定多個 -e 標誌。<br>
一個 -e 標誌只能指定一個函數。<br>
-E Name 不再輸出函數Name 及其子函數的調用圖，此標誌類似於 -e 標誌，但它在總時間和百分比時間的計算中排除了由函數Name 及其子函數所用的時間。<br>
-f Name 輸出函數Name 及其子函數的調用圖。可以指定多個 -f 標誌。<br>
一個 -f 標誌只能指定一個函數。<br>
-F Name 輸出函數Name 及其子函數的調用圖，它類似於 -f 標誌，但它在總時間和百分比時間計算中僅使用所列印的常式的時間。<br>
可以指定多個 -F 標誌。一個 -F 標誌只能指定一個函數。<br>
-F 標誌覆蓋 -E 標誌。<br>
-z 顯示使用次數為零的常式（按照調用計數和累積時間計算）。<br>
不過,gprof不能顯示物件之間的繼承關係,這也是它的弱點.

##參考文獻：
1. http://www.diybl.com/course/6_system/linux/Linuxjs/2008813/135859.html
2. http://www.diybl.com/course/3_program/c++/cppjs/2008624/128167.html
3. http://www.diybl.com/course/3_program/vc/vc_js/20090307/159174.html
4. 程式分析工具gprof介紹 http://www.cnblogs.com/huangpeng/archive/2009/02/17/1392456.html
5. 熟悉binutils工具集 http://yunli.blog.51cto.com/831344/186727 （推薦）
