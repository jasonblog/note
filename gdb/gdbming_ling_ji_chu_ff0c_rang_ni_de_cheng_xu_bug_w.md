# GDB命令基礎，讓你的程序bug無處躲藏

軟件開發，或多或少會走上調試這條路。調試工具可以幫你更加深入瞭解整個程序的運行狀態，對程序運行有更多的主動權。你可以隨心所欲的改變程序運行流程，如：有變量a，你需要不斷改變該a的值，讓程序運行出理想的結果，那麼你可以在調試工具中輕易的實現。
- 運行你的程序，設置所有的能影響程序運行的東西。
- 保證你的程序在指定的條件下停止。
- 當你程序停止時，讓你檢查發生了什麼。
- 改變你的程序。那樣你可以試著修正某個bug引起的問題，然後繼續查找另一 個bug


今天給大家帶來的gdb調試工具。GDB是GNU開源組織發佈的一個強大的UNIX下的程序調試工具。GDB可以調試C、C++、D、Go、python、pascal、assemble(ANSI 彙編標準)等等語言。


##使用GDB
###啟動

```sh
$ gdb program           # program是你的可執行文件，一般在當前目錄
$ gdb program core      # gdb同時調試運行程序和core文件，core是程序非法執行產生的文件
$ gdb program pid       # 如果你的程序是一個服務程序，那麼你可以指定這個服務程序運行時的進程ID。gdb會自動attach上去，並調試他。program應該在PATH環境變量中搜索得到。
```
### 運行

```sh
(gdb) r/run             # 開始運行程序
(gdb) c/continue        # 繼續運行
(gdb) n/next            # 下一行，不進入函數調用
(gdb) s/step            # 下一行，進入函數調用
(gdb) ni/si             # 嚇一跳指令，ni和si區別同上
(gdb) fini/finish       # 繼續運行至函數退出/當前棧幀
(gdb) u/util            # 繼續運行至某一行，在循環中，u可以實現運行至循環剛剛退出，但這取決於循環的實現

(gdb) set args          # 設置程序啟動參數，如：set args 10 20 30
(gdb) show args         # 查看程序啟動參數
(gdb) path <dir>        # 設置程序的運行路徑
(gdb) show paths        # 查看程序的運行路徑
(gdb) set env <name=val># 設置環境變量，如：set env USER=chen
(gdb) show env [name]   # 查看環境變量
(gdb) cd <dir>          # 相當於shell的cd命令
(gdb) pwd               # 顯示當前所在目錄

(gdb) shell <commond>   # 執行shell命令
```
### 設置/查看斷點(breakpoint)

```sh
(gdb) b/break linenum/func      # 在第linenum行或function處停住
(gdb) b/break +/-offset         # 在當前行號後/前offset行停住
(gdb) b/break filename:linenum  # 在源文件filename的linenum行停住
(gdb) b/break filename:func     # 在源文件的function入口停住
(gdb) b/break *address          # 在內存地址address處停住
(gdb) b/break                   # 沒有參數，表示下一跳指令處停住
(gdb) b/break if <condition>    # 條件成立是停住，如在循環中：break if i=100

(gdb) info break [n]            # 查看斷點， n表示斷點號
```

### 設置/查看觀察點(watchpoint)

```sh
# 觀察點一搬來觀察某個表達式或變量的值是否有變化了，有：程序停住
(gdb) watch <expr>              # 觀察值是否有變化
(gdb) rwatch <expr>             # 當expr被讀取時，停住
(gdb) awatch <expr>             # 當expr被讀取或寫入時，停住

(gdb) info watchpoints          # 查看所有觀察點
```

### 設置/查看捕捉點(catchpoint)

```sh
# 你可設置捕捉點來補捉程序運行時的一些事件。如：載入共享庫（動態鏈接庫）或是C++的異常。
(gdb) tcatch <event>            # 只設置一次捕捉點
(gdb) catch <event>             # 當event發生時，停住程序，如下：
# throw             一個c++拋出的異常（throw為關鍵字）
# catch             一個C++捕捉到的異常（catch為關鍵字）
# exec              調用系統調用exec時（只在HP-UX下有用）
# fork              調用系統調用fork時（只在HP-UX下有用）
# vfork             調用系統調用vfork時（只在HP-UX下有用）
# load [file]       載入共享庫（動態鏈接庫）時（只在HP-UX下有用）
# unload [libname]  卸載共享庫（動態鏈接庫）時（只在HP-UX下有用）
```

### 維護停止點

```sh
# 上面說了三種如何設置停止點的方法。在gdb中如果你覺得已經定義好的停止點沒有用，那麼你可以delete、clear、disable、enable進行維護
(gdb) clear                     # 清除所有已定義的停止點。如果程序運行，清除當前行之後的
(gdb) clear <fuction>           # 清除所有設置在函數上的停止點
(gdb) clear <file:line>         # 清除所有設置在指定行上的停止點
(gdb) d/delete [n]/[m-n]        # 刪除斷點號，不設置則刪除全部，也可以範圍m-n

# 比刪除更好的一種方法是disable停止點，disable了的停止點，GDB不會刪除，當你還需要時，enable即可，就好像回收站一樣。
(gdb) disable [n]/[m-n]         # disable指定斷點號n，不指定則disable所有，也可以範圍m-n

(gdb) enable [n]/[m-n]          # enable斷點n，也可以範圍m-n
(gdb) enable once [n]/[m-n]     # enable斷點n一次，程序停止後自動disable，也可以範圍m-n
(gdb) enable delete [n]/[m-n]   # enable斷點，程序結束自動刪除，也可以範圍m-n
```

### 維護條件停止點

```sh
# 前面說到設置breakpoint可以設置成一個條件，這裡列出相關的維護命令
(gdb) condition <bunm> <expr>   # 修改斷掉號bnum的停止條件
(gdb) condition <bnum>          # 清除斷點號bnum的停止條件

# ignore 可以指定程序運行時，忽略停止條件幾次
(gdb) ignore <bnum> <count>     # 忽略斷點號hnum的停止條件count次
```

### 停止點設置運行命令

```sh
# 當程序停住時，我們可以通過command設置其自動執行的命令，這很利於自動化調試。
(gdb) commands [bnum]
> ... commands list ...
> end                   # 這裡為斷點號bnum設置一個命令列表

如：
(gdb) break foo if x>0
(gdb) commands
> printf "x is %dn",x
> continue
> end
# 斷點設置在函數foo中，斷點條件是x>0，如果程序被斷住後，也就是，一旦x的值在foo函數中大於0，GDB會自動打印出x的值，並繼續運行程序。
# 如果你要清除斷點上的命令序列，那麼只要簡單的執行一下commands命令，並直接在打個end就行了。
```

### 斷點菜單

```sh
# 如果你使用c++，有可能下斷點時遇到相同名字的函數，gdb會為你列出該函數菜單供你選擇。
如：
(gdb) b String::after
[0] cancel
[1] all
[2] file:String.cc; line number:867
[3] file:String.cc; line number:860
[4] file:String.cc; line number:875
[5] file:String.cc; line number:853
[6] file:String.cc; line number:846
[7] file
> 2 4 6
Breakpoint 1 at 0xb26c: file String.cc, line 867.
Breakpoint 2 at 0xb344: file String.cc, line 875.
Breakpoint 3 at 0xafcc: file String.cc, line 846.
```

### 恢復程序運行和單步調試

```sh
# 當程序被停住了，你可以用c/continue恢復運行，或下一個斷點到來。也可以使用step或next命令單步跟蹤程序。
(gdb) c/continue [ignore-count]     # 恢復程序運行，ignore-count忽略後面斷點數

# 單步跟蹤，如果有函數調用，他會進入該函數。進入函數的前提是，此函數被編譯有debug信息。很像VC等工具中的stepin。後面可以加count也可以不加，不加表示一條條地執行，加表示執行後面的count條指令，然後再停住。
(gdb) step <count>

# 打開step-mode模式，於是，在進行單步跟蹤時，程序不會因為沒有debug信息而不停住。這個參數有很利於查看機器碼。
(gdb) set step-mode on

# 當你厭倦了在一個循環體內單步跟蹤時，這個命令可以運行程序直到退出循環體。
(gdb) u/until

# 單步跟蹤一條機器指令！一條程序代碼有可能由數條機器指令完成，stepi和nexti可以單步執行機器指令。與之一樣有相同功能的命令是「display/i $pc」 ，當運行完這個命令後，單步跟蹤會在打出程序代碼的同時打出機器指令（也就是彙編代碼）
(gdb) si/stepi
(gdb) ni/stepi
```

### 信號

```sh
# 信號是一種軟中斷，是一種處理異步事件的方法。一般來說，操作系統都支持許多信號。尤其是UNIX，比較重要應用程序一般都會處理信號。UNIX定義了許多信號，比如SIGINT表示中斷字符信號，也就是Ctrl+C的信號，SIGBUS表示硬件故障的信號；SIGCHLD表示子進程狀態改變信號；SIGKILL表示終止程序運行的信號，等等。信號量編程是UNIX下非常重要的一種技術。
# GDB有能力在你調試程序的時候處理任何一種信號，你可以告訴GDB需要處理哪一種信號。你可以要求GDB收到你所指定的信號時，馬上停住正在運行的程序，以供你進行調試。你可以用GDB的handle命令來完成這一功能。
(gdb) handle <signal> <keywords...>
# 在GDB中定義一個信號處理。信號<signal>可以以SIG開頭或不以SIG開頭，可以用定義一個要處理信號的範圍（如：SIGIO-SIGKILL，表示處理從SIGIO信號到SIGKILL的信號，其中包括SIGIO，SIGIOT，SIGKILL三個信號），也可以使用關鍵字all來標明要處理所有的信號。一旦被調試的程序接收到信號，運行程序馬上會被GDB停住，以供調試。其<keywords>可以是以下幾種關鍵字的一個或多個。
  nostop            # 當被調試的程序收到信號時，GDB不會停住程序的運行，但會打出消息告訴你收到這種信號。
  stop          # 當被調試的程序收到信號時，GDB會停住你的程序。
  print         # 當被調試的程序收到信號時，GDB會顯示出一條信息。
  noprint           # 當被調試的程序收到信號時，GDB不會告訴你收到信號的信息。
  pass/noignore # 當被調試的程序收到信號時，GDB不處理信號。這表示，GDB會把這個信號交給被調試程序會處理。
  nopass/ignore # 當被調試的程序收到信號時，GDB不會讓被調試程序來處理這個信號。

# 查看有哪些信號在被GDB檢測中。
(gdb) info signals
(gdb) info handle
```


### 產生信號量

```sh
# 使用singal命令，可以產生一個信號量給被調試的程序。如：中斷信號Ctrl+C。這非常方便於程序的調試，可以在程序運行的任意位置設置斷點，並在該斷點用GDB產生一個信號量，這種精確地在某處產生信號非常有利程序的調試。語法是：
(gdb) signal <singal>
# UNIX的系統信號量通常從1到15。所以<singal>取值也在這個範圍。
# single命令和shell的kill命令不同，系統的kill命令發信號給被調試程序時，是由GDB截獲的，而single命令所發出一信號則是直接發給被調試程序的。
```

### 線程

```sh
# 當你的程序時多線程的，你可以定義斷點是否在所有線程或某個線程
(gdb) info threads                          # 查看線程
(gdb) break <line> thread <threadno>        # 指定源程序line行，線程threadno停住
(gdb) break <line> thread <threadno> if...  # 指定源程序line行，線程threadno停住，跟上條件

如：
(gdb) break frik.c:13 thread 28 if bartab > lim
```

### 查看棧信息

```sh
# 當程序被停住了，你需要做的第一件事就是查看程序是在哪裡停住的。當你的程序調用了一個函數，函數的地址，函數參數，函數內的局部變量都會被壓入「棧」（Stack）中。你可以用GDB命令來查看當前的棧中的信息。
(gdb) bt/backtrace          # 打印當前的啊還能輸調用棧的所有信息
(gdb) bt/backtrace <n>      # 當n為正數，打印棧頂n層。為負數，打印棧低n層

# 如果你要查看某一層的信息，你需要在切換當前的棧，一般來說，程序停止時，最頂層的棧就是當前棧，如果你要查看棧下面層的詳細信息，首先要做的是切換當前棧。
(gdb) f/frame <n>           # n從0開始，是棧中的編號
(gdb) up <n>                # 向棧的上面移動n層。如無n，向上移動一層
(gdb) down <n>              # 向棧的下面移動n層。如無n，向下移動一層

# 棧的層編號，當前的函數名，函數參數值，函數所在文件及行號，函數執行到的語句。
(gdb) f/frame

# 這個命令會打印出更為詳細的當前棧層的信息，只不過，大多數都是運行時的內內地址。比如：函數地址，調用函數的地址，被調用函數的地址，目前的函數是由什麼樣的程序語言寫成的、函數參數地址及值、局部變量的地址等等。
(gdb) info f/frame

(gdb) info args             # 打印當前函數的參數名及值
(gdb) info locals           # 打印當前函數中所有局部變量及值
(gdb) info catch            # 打印當前函數中的異常處理信息
```

### 查看源碼

```sh
# 在程序編譯時一定要加上-g的參數，把源程序信息編譯到執行文件中。不然就看不到源程序了。
(gdb) l                         # 顯示當前行前後的源碼
(gdb) l -                       # 顯示當前行前的源碼
(gdb) l +                       # 顯示當前行後的源碼
(gdb) l/list <linuenum/func>    # 查看第linenum行或者function所在行附近的10行
(gdb) l/list                    # 查看上一次list命令列出的代碼後面的10行
(gdb) l/list m,n                # 查看從第m行到第n行的源碼。m不填，則從當前行到n
(gdb) l/list -/+offset          # 查看想對當前行偏移offset源碼
(gdb) l/list <file:line>        # 查看文件file的line行的源碼
(gdb) l/list <func>             # 查看函數名func源碼
(gdb) l/list <file:func>        # 查看文件file的函數func源碼
(gdb) l/list <*address>         # 查看運行時內存地址address的源碼

(gdb) set listsize              # 設置一次顯示源碼的行數
(gdb) show listsize             # 查看listsize的值
```

### 搜索源代碼

```sh
(gdb) forward-search <regexp>   # 向前搜索，regexp是一個正則表達式
(gdb) search <regexp>           # 向前搜索
(gdb) reverse-search <regexp>   # 全局搜索
```

### 指定源文件路徑

```sh
# 某些時候，用-g編譯過後的執行程序中只是包括了源文件的名字，沒有路徑名。GDB提供了可以讓你指定源文件的路徑的命令，以便GDB進行搜索。
(gdb) dir/directory <dirname ... >  # 加一個源文件路徑到當前路徑的前面。如果你要指定多個路徑，UNIX下你可以使用「:」，Windows下你可以使用「;」。
(gdb) dir/directory                 # 清除所有的自定義的源文件搜索路徑信息。
(gdb) show directories              # 顯示定義了的源文件搜索路徑。
```

### 源代碼內存

```sh
(gdb) info line                     # 查看源代碼在內存中的地址，還可以:
(gdb) info line <num>
(gdb) info line <file:num>
(gdb) info line <func>
(gdb) info line <file:func>

# 還有一個命令（disassemble）你可以查看源程序的當前執行時的機器碼，這個命令會把目前內存中的指令dump出來。如下面的示例表示查看函數func的彙編代碼。
(gdb) disassemble func
```

### 查看運行時數據

```sh
# <expr>是表達式，是你所調試的程序的語言的表達式（GDB可以調試多種編程語言），<f>是輸出的格式，比如，如果要把表達式按16進制的格式輸出，那麼就是/x。
(gdb) p/print <expr>            # expr可以是const常量、變量、函數等內容
(gdb) p/print /<f> <expr>

# 在表達式中，有幾種GDB所支持的操作符，它們可以用在任何一種語言中。
@                   是一個和數組有關的操作符，在後面會有更詳細的說明。
::                  指定一個在文件或是一個函數中的變量。
{<type>} <addr>     表示一個指向內存地址<addr>的類型為type的一個對象。
# 需要注意的是，如果你的程序編譯時開啟了優化選項，那麼在用GDB調試被優化過的程序時，可能會發生某些變量不能訪問，或是取值錯誤碼的情況。
SH
# 輸出格式
# 一般來說，GDB會根據變量的類型輸出變量的值。但你也可以自定義GDB的輸出的格式。例如，你想輸出一個整數的十六進制，或是二進制來查看這個整型變量的中的位的情況。要做到這樣，你可以使用GDB的數據顯示格式：
  x 按十六進制格式顯示變量。
  d 按十進制格式顯示變量。
  u 按十六進制格式顯示無符號整型。
  o 按八進制格式顯示變量。
  t 按二進制格式顯示變量。
  a 按十六進制格式顯示變量。 
  c 按字符格式顯示變量。 
  f 按浮點數格式顯示變量。
  
如：
(gdb) p i
$21 = 101
(gdb) p/a i
$22 = 0x65
```

### 查看內存

```sh
# 你可以使用examine命令（簡寫是x）來查看內存地址中的值。x命令的語法如下所示：
(gdb) x/<n/f/u> <addr>          # n, f, u可選

n       是一個正整數，表示顯示內存的長度，也就是說從當前地址向後顯示幾個地址的內容。
f       表示顯示的格式，參見上面。如果地址所指的是字符串，那麼格式可以是s，如果地十是指令地址，那麼格式可以是i。
u       表示從當前地址往後請求的字節數，如果不指定的話，GDB默認是4個bytes。u參數可以用下面的字符來代替，b表示單字節，h表示雙字節，w表示四字節，g表示八字節。當我們指定了字節長度後，GDB會從指內存定的內存地址開始，讀寫指定字節，並把其當作一個值取出來。
<addr>  表示一個內存地址。

# n/f/u三個參數可以一起使用。例如：
(gdb) x/3uh 0x54320             #表示，從內存地址0x54320讀取內容，h表示以雙字節為一個單位，3表示三個單位，u表示按十六進制顯示。
```

### 自動顯示

```sh
# 你可以設置一些自動顯示的變量，當程序停住時，或是在你單步跟蹤時，這些變量會自動顯示。相關的GDB命令是display。
(gdb) display <expr>
(gdb) display/<fmt> <expr>
(gdb) display/<fmt> <addr>
# expr是一個表達式，fmt表示顯示的格式，addr表示內存地址，當你用display設定好了一個或多個表達式後，只要你的程序被停下來，GDB會自動顯示你所設置的這些表達式的值。

# 格式i和s同樣被display支持，一個非常有用的命令是：
(gdb) display/i $pc
# $pc是GDB的環境變量，表示著指令的地址，/i則表示輸出格式為機器指令碼，也就是彙編。於是當程序停下後，就會出現源代碼和機器指令碼相對應的情形，這是一個很有意思的功能。
下面是一些和display相關的GDB命令：
(gdb) undisplay <dnums...>
(gdb) delete display <dnums...>
# 刪除自動顯示，dnums意為所設置好了的自動顯式的編號。如果要同時刪除幾個，編號可以用空格分隔，如果要刪除一個
# 範圍內的編號，可以用減號表示（如：2-5）
(gdb) disable display <dnums...>
(gdb) enable display <dnums...>
# disable和enalbe不刪除自動顯示的設置，而只是讓其失效和恢復。
(gdb) info display
# 查看display設置的自動顯示的信息。GDB會打出一張表格，向你報告當然調試中設置了多少個自動顯示設置，其中包括，設置的編號，表達式，是否enable。
```

### 設置顯示選項

```sh
# GDB中關於顯示的選項比較多，這裡我只例舉大多數常用的選項。
# 1、打開地址輸出，當程序顯示函數信息時，GDB會顯出函數的參數地址。系統默認為打開的
(gdb) set print address
(gdb) set print address on 
(gdb) set print address off     # 關閉函數的參數地址顯示
(gdb) show print address        # 查看當前地址顯示選項是否打開。

# 2、打開數組顯示，打開後當數組顯示時，每個元素佔一行，如果不打開的話，每個元素則以逗號分隔。這個選項默認是關閉的。
(gdb) set print array
(gdb) set print array on 
(gdb) set print array off
(gdb) show print array
(gdb) show print elements       # 查看print elements的選項信息。
(gdb) set print elements <number-of-elements>
# 這個選項主要是設置數組的，如果你的數組太大了，那麼就可以指定一個<number-of-elements>來指定數據顯示的最大長度，當到達這個長度時，GDB就不再往下顯示了。如果設置為0，則表示不限制。

# 3、如果打開了這個選項，那麼當顯示字符串時，遇到結束符則停止顯示。這個選項默認為off
(gdb) set print null-stop <on/off>  

# 4、如果打開printf pretty這個選項，那麼當GDB顯示結構體時會比較漂亮。
(gdb) set print pretty on 
(gdb) show print pretty         # 查看GDB是如何顯示結構體的。\

# 5、設置字符顯示，是否按「nnn」的格式顯示，如果打開，則字符串或字符數據按nnn顯示，如「65」。
(gdb) set print sevenbit-strings <on/off>
(gdb) show print sevenbit-strings   # 查看字符顯示開關是否打開。

# 6、設置顯示結構體時，是否顯式其內的聯合體數據。
(gdb) set print union <on/off>
(gdb) show print union              # 查看聯合體數據的顯示方式
如：
$1 = {it = Tree, form = {tree = Acorn, bug = Cocoon}}   # 開
$1 = {it = Tree, form = {...}}                          # 關

# 7、在C++中，如果一個對象指針指向其派生類，如果打開這個選項，GDB會自動按照虛方法調用的規則顯示輸出，如果關閉這個選項的話，GDB就不管虛函數表了。這個選項默認是off。
(gdb) set print object <on/off>
(gdb) show print object             # 查看對象選項的設置。

# 8、這個選項表示，當顯示一個C++對象中的內容是，是否顯示其中的靜態數據成員。默認是on。
(gdb) set print static-members <on/off>
(gdb) show print static-members     # 查看靜態數據成員選項設置。

# 9、當此選項打開時，GDB將用比較規整的格式來顯示虛函數表時。其默認是關閉的。
(gdb) set print vtbl <on/off>
(gdb) show print vtbl               # 查看虛函數顯示格式的選項。
```

### 歷史紀錄

```sh
當你用GDB的print查看程序運行時的數據時，你每一個print都會被GDB記錄下來。GDB會以$1, $2, $3 .....這樣的方式為你每一個print命令編上號。於是，你可以使用這個編號訪問以前的表達式，如$1。這個功能所帶來的好處是，如果你先前輸入了一個比較長的表達式，如果你還想查看這個表達式的值，你可以使用歷史記錄來訪問，省去了重複輸入。
```

### 環境變量

```sh
# 你可以在GDB的調試環境中定義自己的變量，用來保存一些調試程序中的運行數據。要定義一個GDB的變量很簡單只需。 使用GDB的set命令。GDB的環境變量和UNIX一樣，也是以$起頭。如：
(gdb) set $foo = *object_ptr
# 使用環境變量時，GDB會在你第一次使用時創建這個變量，而在以後的使用中，則直接對其賦值。環境變量沒有類型，你可以給環境變量定義任一的類型。包括結構體和數組。
(gdb) show convenience
# 該命令查看當前所設置的所有的環境變量。這是一個比較強大的功能，環境變量和程序變量的交互使用，將使得程序調試更為靈活便捷。例如：
(gdb) set $i = 0
(gdb) print bar[$i++]->contents
# 於是，當你就不必，print bar[0]->contents, print bar[1]->contents地輸入命令了。輸入這樣的命令後，只用敲回車，重複執行上一條語句，環境變量會自動累加，從而完成逐個輸出的功能。
```

### 查看寄存器

```sh
# 寄存器中放置了程序運行時的數據，比如程序當前運行的指令地址（ip），程序的當前堆棧地址（sp）等等。你同樣可以使用print命令來訪問寄存器的情況，只需要在寄存器名字前加一個$符號就可以了。如：p $eip。
(gdb) info registers        # 查看寄存器狀態(除浮點寄存器)
(gdb) info all-registers    # 查看所有寄存器狀態
(gdb) info registers regname# 查看指定寄存器狀態，如：info rbp
```

### 修改程序的執行

```sh
# 一旦使用GDB掛上被調試程序，當程序運行起來後，你可以根據自己的調試思路來動態地在GDB中更改當前被調試程序的運行線路或是其變量的值，這個強大的功能能夠讓你更好的調試你的程序，比如，你可以在程序的一次運行中走遍程序的所有分支。

# 一、修改變量值
# 修改被調試程序運行時的變量值，在GDB中很容易實現，使用GDB的print命令即可完成。如：
(gdb) print x=4
# x=4這個表達式是C/C++的語法，意為把變量x的值修改為4，如果你當前調試的語言是Pascal，那麼你可以使用Pascal的語法：x:=4。
# 在某些時候，很有可能你的變量和GDB中的參數衝突，如：
(gdb) whatis width
type = double
(gdb) p width
$4 = 13
(gdb) set width=47
Invalid syntax in expression.
# 因為，set width是GDB的命令，所以，出現了「Invalid syntax in expression」的設置錯誤，此時，你可以使用set
# var命令來告訴GDB，width不是你GDB的參數，而是程序的變量名，如：
(gdb) set var width=47
# 另外，還可能有些情況，GDB並不報告這種錯誤，所以保險起見，在你改變程序變量取值時，最好都使用set var格式的GDB命令。
```

### 跳轉執行

```sh
# 一般來說，被調試程序會按照程序代碼的運行順序依次執行。GDB提供了亂序執行的功能，也就是說，GDB可以修改程序的執行順序，可以讓程序執行隨意跳躍。這個功能可以由GDB的jump命令來完：
(gdb) jump <linespec>
# 指定下一條語句的運行點。<linespce>可以是文件的行號，可以是file:line格式，可以是+num這種偏移量格式。表式著下一條運行語句從哪裡開始。
(gdb) jump <address>
# 這裡的<address>是代碼行的內存地址。注意，jump命令不會改變當前的程序棧中的內容，所以，當你從一個函數跳到另一個函數時，當函數運行完返回時進行彈棧操作時必然會發生錯誤，可能結果還是非常奇怪的，甚至於產生程序Core Dump。所以最好是同一個函數中進行跳轉。
# 熟悉彙編的人都知道，程序運行時，有一個寄存器用於保存當前代碼所在的內存地址。所以，jump命令也就是改變了這個寄存器中的值。於是，你可以使用「set $pc」來更改跳轉執行的地址。如：
(gdb) set $pc = 0x485
```

### 強制函數返回

```sh
# 如果你的調試斷點在某個函數中，並還有語句沒有執行完。你可以使用return命令強制函數忽略還沒有執行的語句並返回。
(gdb) return
(gdb) return <expression>
# 使用return命令取消當前函數的執行，並立即返回，如果指定了<expression>，那麼該表達式的值會被認作函數的返回值。
```

### 強制調用函數

```sh
(gdb) call <expr>
# 表達式中可以一是函數，以此達到強制調用函數的目的。並顯示函數的返回值，如果函數返回值是void，那麼就不顯示。
# 另一個相似的命令也可以完成這一功能——print，print後面可以跟表達式，所以也可以用他來調用函數，print和call的不同是，如果函數返回void，call則不顯示，print則顯示函數返回值，並把該值存入歷史數據中。
```

### GDB語言環境

```sh
(gdb) show language
# 查看當前的語言環境。如果GDB不能識為你所調試的編程語言，那麼，C語言被認為是默認的環境。
(gdb) info frame
# 查看當前函數的程序語言。
(gdb) info source
# 查看當前文件的程序語言。如果GDB沒有檢測出當前的程序語言，那麼你也可以手動設置當前的程序語言。使用set language命令即可做到。當set language命令後什麼也不跟的話，你可以查看GDB所支持的語言種類：
(gdb) set language
The currently understood settings are:
local or auto Automatic setting based on source file
c Use the C language
c++ Use the C++ language
asm Use the Asm language
chill Use the Chill language
fortran Use the Fortran language
java Use the Java language
modula-2 Use the Modula-2 language
pascal Use the Pascal language
scheme Use the Scheme language
# 於是你可以在set language後跟上被列出來的程序語言名，來設置當前的語言環境。
```

### 退出

```sh
(gdb) q/quit                # 退出GDB調試
```

###技巧

```sh
(gdb) b                     # 敲入b按兩次TAB鍵
backtrace break bt
```

本文鏈接：https://deepzz.com/post/gdb-debug.html，參與評論 »

