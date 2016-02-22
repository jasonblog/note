# gdb常用指令


###1. 啟動
```sh
gdb 應用程序名
gdb 應用程序名 core文件名
gdb 應用程序名 pid
gdb --args 應用程序名 應用程序的運行參數
```

###幫助:
```sh
help 顯示幫助
info 顯示程序狀態
set 修改
show 顯示gdb狀態
```
###運行及運行環境設置:
```sh
set args # 設置運行參數
show args # 顯示運行參數

set env 變量名 = 值 # 設置環境變量
unset env [變量名] # 取消環境變量
show env [變量名] # 顯示環境變量

path 目錄名 # 把目錄添加到查找路徑中
show paths # 顯示當前查找路徑

cd 目錄 # 切換工作目錄
pwd # 顯示當前工作目錄

tty /dev/pts/1 # 修改程序的輸入輸出到指定的tty
set inferior-tty /dev/pts/1 # 修改程序的輸出到指定的tty
show inferior-tty
show tty

run 參數 # 運行
start 參數 # 開始運行，但是會在main函數停止
attach pid
detach

kill # 退出
Ctrl-C # 中斷(SIGINT)
Ctrl-]
```

### 線程操作:
```sh
info threads # 查看所有線程信息
thread 線程id # 切換到指定線程
thread apply [threadno | all ] 參數 # 對所有線程都應用某個命令
```

###子進程調試:
```sh
set follow-fork-mode child|parent # fork後，需要跟蹤誰
show follow-fork-mode
set detach-on-flow on|off # fork後，需要兩個都跟蹤嗎

info forks # 顯示所有進程信息
fork 進程id # 切換到某個進程
detach-fork 進程id # 不再跟蹤某個進程
delete fork 進程id # kill某個進程並停止對它的跟蹤
```
###檢查點: 
```sh
checkpoint/restart
```

### 查看停止原因:
```c
info program
```

###斷點(breakpoint): 程序運行到某處就會中斷
```sh 
break(b) 行號|函數名|程序地址 | +/-offset | filenam:func [if 條件] # 在指定位置設置斷點
tbreak ... # 與break相似，只是設置一次斷點
hbreak ... # 與break相似，只是設置硬件斷點，需要硬件支持
thbreak ... # 與break相似，只是設置一次性硬件斷點，需要硬件支持
rbreak 正則表達式 # 給一批滿足條件的函數打上斷點
info break [斷點號] # 查看某個斷點或所有斷點信息

set breadpoint pending auto|on|off # 查看如果斷點位置沒有找到時行為
show breakpoint pending
```

###觀察點(watchpoint): 表達式的值修改時會被中斷

```sh
watch 表達式 # 當表達式被寫入，並且值被改變時中斷
rwatch 表達式 # 當表達式被讀時中斷
awatch 表達式 # 當表達式被讀或寫時中斷
info watchpoints

set can-use-hw-watchpoints 值 # 設置使用的硬件觀察點的數
show can-use-hw-watchpoints

rwatch與awatch需要有硬件支持，另外如果是對局部變量使用watchpoint，那退出作用域時觀察點會自動被刪除
另外在多線程情況下，gdb的watchpoint只對一個線程有效
```

###捕獲點(catchpoint): 程序發生某個事件時停止，如產生異常時
```sh
catch 事件名
事件包括:
throw # 產生c++異常
catch # 捕獲到c++異常
exec/fork/vfork # 一個exec/fork/vfork函數調用,只對HP-UX
load/unload [庫名] # 加載/卸載共享庫事件，對只HP-UX
tcatch 事件名 # 一次性catch
info break
```


###斷點操作:
```sh
clear [函數名|行號] # 刪除斷點，無參數表示刪衛當前位置
delete [斷點號] # 刪除斷點，無參數表示刪所有斷點
disable [斷點號]
enable [斷點號]


condition 斷點號 條件 # 增加斷點條件
condition 斷點號 # 刪除斷點條件

ignore 斷點號 數目 # 忽略斷點n次

commands 斷點號 # 當某個斷點中斷時打印條件
條件
end

```

- 下面是一個例子,可以一直打印當前的X值：

```sh
commands 3
printf "X:%d\n",x
cont
end
```

### 斷點後操作:
```sh
continue(c) [忽略次數] # 繼續執行，[忽略前面n次中斷]
fg [忽略次數] # 繼續執行，[忽略前面n次中斷]

step(s) [n步] # 步進,重複n次
next(n) [n步] # 前進,重複n次

finish # 完成當前函數調用，一直執行到返回處，並打印返回值

until(u) [位置] # 一直執行到當前行或指定位置，或是當前函數返回
advance 位置 # 前面到指定位置，如果當前函數返回則停止,與until類似

stepi(si) [n步] # 精確的只執行一個彙編指令,重複n次
nexti(ni) [n步] # 精確的只執行一個彙編指令,碰到函數跳過,重複n次

set step-mode on|off # on時,如果函數沒有調試信息也跟進
show step-mode
```

###信號:
```sh
info signals # 列出所有信號的處理方式
info handle # 同上

handle 信號 方式 # 改變當前處理某個信號的方式
方式包括:
nostop # 當信號發生時不停止，只打印信號曾經發生過
stop # 停止並打印信號
print # 信號發生時打印
noprint # 信號發生時不打印
pass/noignore # gdb充許應用程序看到這個信號
nopass/ignore # gdb不充許應用程序看到這個信號
```

###線程斷點:
```sh
break 行號信息 thread 線程號 [if 條件] # 只在某個線程內加斷點
```

###線程調度鎖:
```sh
set scheduler-locking on|off # off時所有線程都可以得到調度,on時只有當前
show scheduler-locking
```

###幀:
```sh
frame(f) [幀號] # 不帶參數時顯示所有幀信息，帶參數時切換到指定幀
frame 地址 # 切換到指定地址的幀
up [n] # 向上n幀
down [n] # 向下n幀

select-frame 幀號 # 切換到指定幀並且不打印被轉換到的幀的信息
up-silently [n] # 向上n幀,不顯示幀信息
down-silently [n] # 向下n幀,不顯示幀信息

```

### 調用鏈:
```sh 
backtrace(bt) [n|-n|full] # 顯示當前調用鏈,n限制顯示的數目,-n表示顯示後n個,n表示顯示前n個，full的話還會顯示變量信息
使用 thread apply all bt 就可以顯示所有線程的調用信息

set backtrace past-main on|off
show backtrace past-main

set backtrace past-entry on|off
show backtrace past-entry

set backtrace limit n # 限制調用信息的顯示層數
show backtrace limit
```

### 顯示幀信息:
```sh
info frame # 顯示當前幀信息
info frame addr # 顯示指定地址的幀信息
info args # 顯示幀的參數
info locals # 顯示局部變量信息
info catch # 顯示本幀異常信息
```

### 顯示行號:

```sh
list(l) [行號|函數|文件:行號] # 顯示指定位置的信息,無參數為當前位置
list - # 顯示當前行之前的信息

list first,last # 從frist顯示到last行
list ,last # 從當前行顯示到last行
list frist, # 從指定行顯示
list + # 顯示上次list後顯示的內容
list - # 顯示上次list前面的內容

在上面，first和last可以是下面類型:
行號
+偏移
-偏移
文件名:行號
函數名
函數名:行號

set listsize n # 修改每次顯示的行數
show listsize
```

### 編輯:
```sh
edit [行號|函數|函數名:行號|文件名:函數名] # 編輯指定位置
```

###查找:
```sh
search 表示式 # 向前查找表達式
reverse-search 表示式 # 向後查找表達式
```

###指定源碼目錄:

```sh
directory(dir) [目錄名] # 指定源文件查找目錄
show directories
```

###源碼與機器碼：

```sh
info line [函數名|行號] # 顯示指定位置對應的機器碼地址範圍
disassemble [函數名 | 起始地址 結束地址] # 對指定範圍進行反彙編
set disassembly-flavor att|intel # 指定彙編代碼形式
show disassembly-flavor
```


### 查看數據:
```sh
ptype 表達式 # 查看某個表達式的類型

print [/f] [表達式] # 按格式查看錶達式內容,/f是格式化

set print address on|off # 打印時是不是顯示地址信息
show print address

set print symbol-filename on|off # 是不是顯示符號所在文件等信息
show print symbol-filename

set print array on | off # 是不是打印數組
show print array

set print array index on | off # 是不是打印下標
show print array index

...

表達式可以用下面的修飾符:
var@n # 表示把var當成長度為n的數組
filename::var # 表示打印某個函數內的變量,filename可以換成其它範圍符如文件名
{type} var # 表示把var當成type類型

```
### 輸出格式:
```sh
x # 16進制
d # 10進制
u # 無符號
o # 8進制
t # 2進制
a # 地址
c # 字符
f # 浮點
```

###查看內存:

```sh
x /nfu 地址 # 查看內存
n 重複n次
f 顯示格式,為print使用的格式
u 每個單元的大小,為
b byte
h 2 byte
w 4 byte
g 8 byte
```


### 自動顯示:
```sh
display [/fmt] 表達式 # 每次停止時都會顯示錶達式,fmt與print的格式一樣,如果是內存地址，那fmt可像 x的參數一樣
undisplay 顯示編號
delete display 顯示編號 # 這兩個都是刪附某個顯示

disable display 顯示編號 # 禁止某個顯示
enable display 顯示編號 # 重顯示
display # 顯示當前顯示內容
info display # 查看所有display項
```

### 查看變量歷史：

```sh
show values 變量名 [n] # 顯示變量的上次顯示歷史,顯示n條
show values 變量名 + # 繼續上次顯示內容
```

### 便利變量: (聲明變量的別名以方便使用)

```sh
set $foo = *object_ptr # 聲明foo為object_ptr的便利變量
init-if-undefined $var = expression # 如果var還未定義則賦值
show convenience
```

###內部便利變量：
```sh
$_ 上次x查看的地址
$__
$_exitcode 程序垢退出碼
```

### 寄存器:
```sh
into registers # 除了浮點寄存器外所有寄存器
info all-registers # 所有寄存器
into registers 寄存器名 # 指定寄存器內容
info float # 查看浮點寄存器狀態
info vector # 查看向量寄存器狀態

gdb為一些內部寄存器定義了名字，如$pc(指令),$sp(棧指針),$fp(棧幀),$ps(程序狀態)
p /x $pc # 查看pc寄存器當前值
x /i $pc # 查看要執行的下一條指令
set $sp += 4 # 移動棧指針
```

###內核中信息:

```sh
info udot # 查看內核中user struct信息
info auxv # 顯示auxv內容(auxv是協助程序啟動的環境變量的)
```

### 內存區域限制:

```sh
mem 起始地址 結構地址 屬性 # 對[地始地址,結構地址)區域內存進行保護，如果結構地址為0表示地址最大值0xffffffff
delete mem 編號 # 刪除一個內存保護
disable mem 編號 # 禁止一個內存保護
enable mem 編號 # 打開一個內存保護
info mem # 顯示所有內存保護信息
```

### 保護的屬性包括：
```sh
1. 內存訪問模式: ro | wo |rw
2. 內存訪問大小: 8 | 16 | 32 | 64 如果不限制，表示可按任意大小訪問
3. 數據緩存: cache | nocache cache表示充許gdb緩存目標內存
```


### 內存複製到/從文件:

```sh
dump [格式] memory 文件名 起始地址 結構地址 # 把指定內存段寫到文件
dump [格式] value 文件名 表達式 # 把指定值寫到文件
格式包括:
binary 原始二進制格式
ihex intel 16進制格式
srec S-recored格式
tekhex tektronix 16進制格式

append [binary] memory 文件名 起始地址 結構地址 # 按2進制追加到文件
append [binary] value 文件名 表達式 # 按2進制追加到文件

restore 文件名 [binary] bias 起始地址 結構地址 # 恢復文件中內容到內存.如果文件內容是原始二進制，需要指定binary參數，不然會gdb自動識別文件格式
```


###產生core dump文件
```sh
gcore [文件名] # 產生core dump文件
```

### 字符集:

```sh
set target-charset 字符集 # 聲明目標機器的locale,如gdbserver所在機器
set host-charset 字符集 # 聲明本機的locale
set charset 字符集 # 聲明目標機和本機的locale
show charset
show host-charset
show target-charset
```

###緩存遠程目標的數據:為提高性能可以使用數據緩存，不過gdb不知道volatile變量，緩存可能會顯示不正確的結構

```sh 
set remotecache on | off
show remotecache
info dcache # 顯示數據緩存的性能
```

###C預處理宏:
```sh
macro expand(exp) 表達式 # 顯示宏展開結果
macro expand-once(expl) 表達式 # 顯示宏一次展開結果
macro info 宏名 # 查看宏定義
```

###追蹤(tracepoint): 就是在某個點設置採樣信息，每次經過這個點時只執行已經定義的採樣動作但並不停止，最後再根據採樣結果進行分析。

###採樣點定義:
```sh
trace 位置 # 定義採樣點
info tracepoints # 查看採樣點列表
delete trace 採樣點編號 # 刪除採傑點
disable trace 採樣點編號 # 禁止採傑點
enable trace 採樣點編號 # 使用採傑點
passcount 採樣點編號 [n] # 當通過採樣點 n次後停止,不指定n則在下一個斷點停止
```

### 預定義動作：預定義動作以actions開始,後面是一系列的動作
```sh

actions [num] # 對採樣點num定義動作
行為:
collect 表達式 # 採樣表達式信息
一些表達式有特殊意義，如$regs(所有寄存器),$args(所有函數參數),$locals(所有局部變量)
while-steping n # 當執行第n次時的動作,下面跟自己的collect操作
```

###採樣控制:
```sh
tstart # 開始採樣
tstop # 停止採樣
tstatus # 顯示當前採樣的數據
```

###使用收集到的數據:
```sh
tfind start # 查找第一個記錄
tfind end | none # 停止查找
tfind # 查找下一個記錄
tfind - # 查找上一個記錄
tfind tracepoint N # 查找 追蹤編號為N 的下一個記錄
tfind pc 地址 # 查找代碼在指定地址的下一個記錄
tfind range 起始,結束
tfind outside 起始，結構
tfind line [文件名:]行號

tdump # 顯示當前記錄中追蹤信息
save-tracepoints 文件名 # 保存追蹤信息到指定文件,後面使用source命令讀
```

###追蹤中的便利變量:
```sh
$trace_frame # 當前幀編號, -1表示沒有, INT
$tracepoint # 當前追蹤,INT
$trace_line # 位置 INT
$trace_file # 追蹤文件 string, 需要使用output輸出，不應用printf
$trace_func # 函數名 string
```

### 覆蓋技術(overray): 用於調試過大的文件

### gdb文件:
```sh
file 文件名 # 加載文件,此文件為可執行文件，並且從這裡讀取符號
core 文件名 # 加載core dump文件
exec-file 文件名 # 加載可執行文件
symbol-file 文件名 # 加載符號文件
add-symbol-file 文件名 地址 # 加載其它符號文件到指定地址
add-symbol-file-from-memory 地址 # 從指定地址中加載符號
add-share-symbol-files 庫文件 # 只適用於cygwin
session 段 地址 # 修改段信息
info files | target # 打開當前目標信息
maint info sections # 查看程序段信息
set truct-readonly-sections on | off # 加快速度
show truct-readonly-sections

set auto-solib-add on | off # 修改自動加載動態庫的模式
show auto-solib-add

info share # 打印當前加載的共享庫的地址信息
share [正則表達式] # 從符合的文件中加載共享庫的正則表達式

set stop-on-solib-events on | off # 設置當加載共享庫時是不是要停止
show stop-on-solib-events

set solib-absolute-prefix 路徑 # 設置共享庫的絕對路矩，當加載共享庫時會以此路徑下查找(類似chroot)
show solib-absolute-prefix

set solib-search-path 路徑 # 如果solib-absolute-prefix查找失敗，那將使用這個目錄查找共享庫
show solib-search-path
```

