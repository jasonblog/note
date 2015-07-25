# gdb 調試利器
GDB是一個由GNU開源組織發佈的、UNIX/LINUX操作系統下的、基於命令行的、功能強大的程序調試工具。 對於一名Linux下工作的c++程序員，gdb是必不可少的工具；

### 1.1. 啟動gdb
對C/C++程序的調試，需要在編譯前就加上-g選項:
```
$g++ -g hello.cpp -o hello
```
調試可執行文件:
```
$gdb <program>
```
program也就是你的執行文件，一般在當前目錄下。

調試core文件(core是程序非法執行後core dump後產生的文件):
```
$gdb <program> <core dump file>
$gdb program core.11127
```
調試服務程序:
```
$gdb <program> <PID>
$gdb hello 11127
```

如果你的程序是一個服務程序，那麼你可以指定這個服務程序運行時的進程ID。gdb會自動attach上去，並調試他。program應該在PATH環境變量中搜索得到。

### 1.2. gdb交互命令
啟動gdb後，進入到交互模式，通過以下命令完成對程序的調試；注意高頻使用的命令一般都會有縮寫，熟練使用這些縮寫命令能提高調試的效率；

運行
run：簡記為 r ，其作用是運行程序，當遇到斷點後，程序會在斷點處停止運行，等待用戶輸入下一步的命令。
continue （簡寫c ）：繼續執行，到下一個斷點處（或運行結束）
next：（簡寫 n），單步跟蹤程序，當遇到函數調用時，也不進入此函數體；此命令同 step 的主要區別是，step 遇到用戶自定義的函數，將步進到函數中去運行，而 next 則直接調用函數，不會進入到函數體內。
step （簡寫s）：單步調試如果有函數調用，則進入函數；與命令n不同，n是不進入調用的函數的
until：當你厭倦了在一個循環體內單步跟蹤時，這個命令可以運行程序直到退出循環體。
until+行號： 運行至某行，不僅僅用來跳出循環
finish： 運行程序，直到當前函數完成返回，並打印函數返回時的堆棧地址和返回值及參數值等信息。
call 函數(參數)：調用程序中可見的函數，並傳遞“參數”，如：call gdb_test(55)
quit：簡記為 q ，退出gdb
設置斷點
break n （簡寫b n）:在第n行處設置斷點
（可以帶上代碼路徑和代碼名稱： b OAGUPDATE.cpp:578）

b fn1 if a＞b：條件斷點設置
break func（break縮寫為b）：在函數func()的入口處設置斷點，如：break cb_button
delete 斷點號n：刪除第n個斷點
disable 斷點號n：暫停第n個斷點
enable 斷點號n：開啟第n個斷點
clear 行號n：清除第n行的斷點
info b （info breakpoints） ：顯示當前程序的斷點設置情況
delete breakpoints：清除所有斷點：
查看源代碼
list ：簡記為 l ，其作用就是列出程序的源代碼，默認每次顯示10行。
list 行號：將顯示當前文件以“行號”為中心的前後10行代碼，如：list 12
list 函數名：將顯示“函數名”所在函數的源代碼，如：list main
list ：不帶參數，將接著上一次 list 命令的，輸出下邊的內容。
打印表達式
print 表達式：簡記為 p ，其中“表達式”可以是任何當前正在被測試程序的有效表達式，比如當前正在調試C語言的程序，那麼“表達式”可以是任何C語言的有效表達式，包括數字，變量甚至是函數調用。
print a：將顯示整數 a 的值
print ++a：將把 a 中的值加1,並顯示出來
print name：將顯示字符串 name 的值
print gdb_test(22)：將以整數22作為參數調用 gdb_test() 函數
print gdb_test(a)：將以變量 a 作為參數調用 gdb_test() 函數
display 表達式：在單步運行時將非常有用，使用display命令設置一個表達式後，它將在每次單步進行指令後，緊接著輸出被設置的表達式及值。如： display a
watch 表達式：設置一個監視點，一旦被監視的“表達式”的值改變，gdb將強行終止正在被調試的程序。如： watch a
whatis ：查詢變量或函數
info function： 查詢函數
擴展info locals： 顯示當前堆棧頁的所有變量
查詢運行信息
where/bt ：當前運行的堆棧列表；
bt backtrace 顯示當前調用堆棧
up/down 改變堆棧顯示的深度
set args 參數:指定運行時的參數
show args：查看設置好的參數
info program： 來查看程序的是否在運行，進程號，被暫停的原因。
分割窗口
layout：用於分割窗口，可以一邊查看代碼，一邊測試：
layout src：顯示源代碼窗口
layout asm：顯示反彙編窗口
layout regs：顯示源代碼/反彙編和CPU寄存器窗口
layout split：顯示源代碼和反彙編窗口
Ctrl + L：刷新窗口
註解

交互模式下直接回車的作用是重覆上一指令，對於單步調試非常方便；
### 1.3. 更強大的工具
cgdb
cgdb可以看作gdb的界面增強版,用來替代gdb的 gdb -tui。cgdb主要功能是在調試時進行代碼的同步顯示，這無疑增加了調試的方便性，提高了調試效率。界面類似vi，符合unix/linux下開發人員習慣;如果熟悉gdb和vi，幾乎可以立即使用cgdb。
