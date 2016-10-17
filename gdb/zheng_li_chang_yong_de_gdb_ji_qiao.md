# 整理常用的gdb技巧


##常用命令

常用的gdb命令...

##啟動gdb

###直接運行
```sh 
gdb --args prog arg1 arg2
```

###運行gdb後使用run命令
```
gdb prog
run arg1 arg2
```

###attach到已運行的程序
```
gdb --pid ${PID_OF_PROG}
```

##ptype

ptype用於顯示Symbol的類型，示例源碼為:

```c
struct ABC {
    int val;
}

int main() {
    ABC abc;
    return 0;
}
```

運行gdb：

```sh
(gdb) b 7
(gdb) r
(gdb) ptype abc

type = struct XXX {
    int val;
}
```

ptype可以輸出表達式的返回類型，具體介紹可參考Examining the Symbol Table。

##print {type} variable

print(p)可以按照某種類型輸出變量的值，示例源碼如下:

```c
struct ABC {
    double val;
    int val2;
}

int main() {
    ABC abc;
    abc.val = 1.5;
    abc.val2 = 10;

    void *pAbc = &abc;

    return 0;
}
```

運行gdb:

```sh
(gdb) b 13
(gdb) r

(gdb) p pAbc
$1 = (void *) 0x7fffffffe710

(gdb) p {ABC} 0x7fffffffe710
$2 = {val = 1.5, val2 = 10}

(gdb) p {ABC} pAbc
$3 = {val = 1.5, val2 = 10}

(gdb) p * (ABC*) pAbc
$4 = {val = 1.5, val2 = 10}

(gdb) p {double} pAbc
$5 = 1.5

(gdb) p * (double*) pAbc
$6 = 1.5

(gdb) p {int} (pAbc + sizeof (double))
$7 = 10

(gdb) p * (int*) (pAbc + sizeof (double))
$8 = 10
```

有時候，如果字符串太長，gdb可能只打印一部分，這時候如下設置來打印全部內容：
```sh
(gdb) set print elements 0
```

##examine

examine(x)可以按照一定的格式打印內存地址處的數據，詳細文檔可參考這裡。

```sh
(gdb) x/{COUNT}{FMT}{SIZE} {ADDRESS}
```

- {COUNT}: 打印的數目，默認為1。
- {FMT}: 打印的格式[1]，默認為上次使用的{FMT}:
    - o(octal): 8進制整數
    - x(hex): 16進制整數
    - d(decimal): 10進制整數
    - u(unsigned decimal): 10進制非負整數
    - t(binary): 2進制整數
    - f(float): 浮點數
    - a(address): 輸出相對於前面最近的符號的偏移
    - i(instruction): 輸出地址處的彙編指令
    - c(char): 字符
    - s(string): c字符串(null-terminated string)
    - z(hex, zero padded on the left): 見說明
- {SIZE}: 打印的字節數目，默認為上次使用的{SIZE}:
    - b(byte): 1個字節
    - h(halfword): 2個字節
    - w(word): 4個字節
    - g(giant, 8 bytes): 8個字節
- {ADDRESS}: 目標地址

幾個例子:

```sh
(gdb) x/a 0x401419
0x401419 <main()+113>:  0x55c3c900000000b8

(gdb) x/i 0x40138d
=> 0x40138d <crash(int, double)+41>:    mov    -0x10(%rbp),%eax

(gdb) x/1fg 140737488346064
0x7fffffffdbd0: 10.125
```


##設置源碼目錄

參考Specifying Source Directories，使用`dir /path/to/your/sources`可在調試時添加一個源碼目錄。

##設置字符編碼

gdb默認使用utf-8編碼，可以使用如下命令修改編碼。

```
set charset GBK
```

也可直接在~/.gdbinit裡設置。

##線程相關的命令

下面是一些調試多線程程序時常用的命令:

- info threads: 查看線程列表
- thread 2: 切換到2號線程，線程編號可由info threads得知
- thread apply all bt: 打印所有線程的堆棧


##高級技巧

一些不太廣為人知的技巧...

加載獨立的調試信息

gdb調試的時候可以從單獨的符號文件中加載調試信息。

```sh
(gdb) exec-file test
(gdb) symbol-file test.debug
```

test是移除了調試信息的可執行文件, test.debug是被移除後單獨存儲的調試信息。參考stackoverflow上的一個問題，可以如下分離調試信息:

```sh
# 編譯程序，帶調試信息(-g)
gcc -g -o test main.c

# 拷貝調試信息到test.debug
objcopy --only-keep-debug test test.debug

# 移除test中的調試信息
strip --strip-debug --strip-unneeded test

# 然後啟動gdb
gdb -s test.debug -e test

# 或這樣啟動gdb
gdb
(gdb) exec-file test
(gdb) symbol-file test.debug
```

分離出的調試信息test.debug還可以鏈接回可執行文件test中

```sh
objcopy --add-gnu-debuglink test.debug test
```

然後就可以正常用addr2line等需要讀取調試信息的程序了

```sh
addr2line -e test 0x401c23
```

更多內容可閱讀GDB: Debugging Information in Separate Files。

##在內存和文件系統之間拷貝數據

- 將內存數據拷貝到文件裡

```sh
dump binary value file_name variable_name
dump binary memory file_name begin_addr end_addr 
```

- 改變內存數據

使用set命令

##執行gdb腳本

常用的gdb操作，比如打斷點等可以放在一個gdb腳本里，然後使用時導入即可。例如:

```sh
b main.cpp:15
b test.cpp:18
```

gdb運行時，使用source命令即可導入

```sh
(gdb) source /path/to/breakpoints.txt
```

或gdb運行時導入

```sh
gdb -x /path/to/breakpoints.txt prog
```

對於每次gdb運行都要調用的腳本，比如設置字符集等，可以放在~/.gdbinit初始文件裡，這樣每次gdb啟動時都會自動調用。

##輸出到文件

可以通過set logging on將命令的輸出保存到默認的gdb.txt文件中。當然也可以通過set logging file my_log.txt來設置輸出文件的路徑。

##執行命令並退出

有時候需要gdb執行若干條命令後就立即退出，而不是進入交互界面，這時可以使用`-batch`選項。

```sh
gdb -ex "set pagination 0" -ex "thread apply all bt" `-batch` -p $pid
```

上面的命令打印$pid進程所有線程的堆棧並退出。

##自定義命令

參考gdb/Define，可以在gdb中自定義命令，比如：

```sh
(gdb) define hello
(gdb) print "welcome"
(gdb) print "hello $arg0"
(gdb) end
```

然後如此調用

```sh
(gdb) hello world
```

即可輸出

```sh
(gdb) $1 = "welcome"
(gdb) $2 = "hello world"
```

##條件斷點

在條件斷點裡可以調用標準庫的函數，比如下面這個:

```sh
# 如果strA == strB，則在斷點處暫停
(gdb) b main.cpp:255 if strcmp(strA.c_str(), strB.c_str()) == 0

# 還是上面的場景，直接用string類的compare函數
(gdb) b main.cpp:255 if strA.compare(strB) != 0
```

##捕獲exception

gdb遇到未處理的exception時，並不會捕獲處理。但是參考Set Catchpoints，可以使用catch catch命令來捕獲exception。

##閱讀資料

-Copy between memory and a file
- HowTo: Writing into process memory with GDB
- Specifying Source Directories
- Examining the Symbol Table
