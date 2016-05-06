# Makefile範例教學


Makefile和GNU make可能是linux世界裡最重要的檔案跟指令了。編譯一個小程式，可以用簡單的command來進行編譯；稍微複雜一點的程式，可以用shell script來幫忙進行編譯。如今的程式(如Apache, Linux Kernel)可能動輒數百萬行程式碼，數萬個標頭檔(headers)、函式庫(libraries)以及程式碼(source code)，如果只是針對幾個檔案進行修改，卻要用shell script整個程式重新編譯，不但浪費時間也相當沒有效率。GNU make或是其他make程式的用途就在這裡： 當程式有些許變動時，我們需要一個程式幫助我們判斷哪些需要重新編譯，哪些不用；因此，撰寫一個好的Makefile便是相當重要的能力。

不過話說回來，是不是每一隻程式都需要一個Makefile呢？其實撰寫Makefile是有益無害的，只是如果你的程式就只有兩三個source codes需要編譯，其實忘掉Makefile也沒關係。本文的目的是希望以範例的方式能夠讓讀者能看得懂，並且有能力撰寫並修改Makefile，也順便當作自己的筆記。

## 傳統的編譯：

```sh
gcc foo1.c -o foo1
```

事實上，上面的這個編譯方式可以拆解成：
```sh
gcc foo1.c -c
gcc foo1.o -o foo1
```

編譯的過程是將原始碼(foo1.c)先利用-c參數編譯成.o(object file)，然後再鏈結函式庫成為一個binary。-c即compile之意。

```sg
gcc foo1.c $SACLIB/sacio.a -O3 -g -Wall -ansi -o foo1
```

開始有趣了。編譯的參數開始變多：
- -c :編譯但不進行鏈結，會產生一個跟原始碼相同名稱但副檔名為.o的目的檔。

- -O ：表示最佳化的程度
-O預設就是-O1，你可以指定成-O2或-O3，數字越大表示最佳化程度越好，但是會增加編譯時間。

- -g ：把偵錯資訊也編譯進去
當你有需要使用GDB軟體進行偵錯，必須加入-g使GDB能夠讀取。一般情況下可以不用-g，因為他也會增加binary大小。

- -Wall ：顯示警告訊息
使用這個參數會在編譯時顯示更多的警告訊息。這個參數相當有用，特別是找不到libs/headers之類的問題。

- -ansi ：使用相容ANSI標準的編譯方式
ANSI是American National Standards Institute，即美國國家標準學會的簡稱。-ansi可以增加程式的可移植性。

- 其中的$SACLIB就是一個變數名稱，她必須被指定正確的值。
執行這個命令前必須先確定這個變數是有被指派正確的值才行。.a檔是一個靜態函式(static library)，關於靜態跟共享的觀念稍候解釋。


再來更多吧！假設你今天要編譯main這隻程式，他的source codes有main.c, foo.c, target.h，並且需要/usr/local/moreFoo/lib/libpthread.so這個共享函式，以及/usr/local/moreFoo/include裡面的headers；這麼複雜的情況又該怎麼作呢？


```sh
gcc main.c foo.c -I /usr/local/moreFoo/include -lpthread -L /usr/local/moreFoo/lib -O3 -ansi -o main
```

新的參數意義如下：
- -I ：需要include某些headers所在的目錄
通常include目錄都放置headers，利用-I使編譯器知道去哪裡找原始碼裡宣告的header。gcc預設會去尋找headers的目錄大致有：
    - /usr/include
    - /usr/local/include
    - /usr/src/linux-headers-`uname -r`/include
    - /usr/lib/gcc/i486-linux-gnu/UR_GCC_VERSION/include
當前目錄

因此，當原始碼內有宣告
```c
#include <fakeFoo.h> 
```
但fakeFoo.h並不在上述的資料夾內，就需要利用-I引導gcc找到她。至於target.h因為在當前目錄，因此不必額外宣告。
當然，可以利用多個-I來指定多個headers的路徑。

- -l :表示編譯過程需要一個library。
-lpthread代表需要一個名為libpthread.so的函式。

- -L :需要額外鏈結函式庫所在的目錄
有時候程式碼經常會呼叫一些函數(methods, functions或是subroutines)，而這些函數是使用其他人預先寫好的、已經編譯成函式(例如libpthread.so)供人使用的話，我們就不必自己從頭寫過。gcc預設會去找函式的目錄大致有：

- /lib
- /usr/lib
- /lib/modules/`uname -r`/kernel/lib
- /usr/src/linux-headers-`uname -r`/lib
- /usr/local/lib

當前目錄

因此編譯時，利用-L指定目錄告訴編譯器可以該路徑下尋找libpthread.so。因此，若使用了-l，則必須確定所使用的lib有在預設尋找的目錄中，否則就必須利用-L來指定路徑給編譯器。
當然，可以利用多個-L來指定多個lib路徑。


# 靜態、共享與動態鏈結函式庫

我們已經知道：輪子不必重複發明 -- 人家寫好的方法我們可以直接拿來用。不過很多時候，這些方法可能因為某些因素，希望提供給別人使用卻又不希望公佈原始碼，這時候編譯成libraries是最好的選擇


##靜態函式(static libraries)
靜態函式其實就是將一系列.o檔打包起來，因此她可以直接視為一個巨大的.o檔。打造出一個靜態函式的方法很簡單：

```sh
gcc operator.c -c
ar crsv liboperator.a operator.o
```

或者
```sh
gcc -static operator.c -loperator
```

兩種方法皆能產生liboperator.a。假設這個靜態函式在/usr/local/foo/lib/裡，編譯時要與靜態函式作鏈結也很容易：
```sh
gcc main.c /usr/local/foo/lib/liboperator.a -o main
```

把靜態函式當成一般的.o檔一起納入binary，也可以像這樣：
```sh
gcc main.c -L /usr/local/foo/lib -loperator -o main
```

靜態函式將所有的功能全部打包在一起，因此binary會變得很巨大，但是執行這個程式的所有功能都已滿足，不會再有libraries相依性的問題。但是缺點在於當某些libraries的功能有所更新時，這個程式就必須重新編譯，無法享受到即時更新的優點。通常商業軟體以及嵌入式系統等功能異動較少的程式，會傾向使用靜態函式。

##共享函式(shared libraries)
共享函式跟靜態函式的觀念剛好相反，程式在執行時必須能夠找到相依的函式，否則執行時會出現錯誤訊息。製作一個共享函式的方法也很簡單：
```sh
gcc -shared operator.c -o liboperator.so
```

或是先編譯出目的檔再進行鏈結：
```sh
gcc -c operator.c
gcc -shared operator.o -o liboperator.so
```

產生出liboperator.so。假設這個共享函式在/usr/local/foo/lib/裡，使用共享函式進行鏈結也很容易：
```sh
gcc main.c /usr/local/foo/lib/liboperator.so -o main
```

也可以像這樣：
```sh
gcc main.c -L /usr/local/foo/lib -loperator -o main
```

共享函式在`程式啟動時期會檢查是否存在`。以一個分別鏈結了靜態函式與共享函式的binary而言，執行的結果大有差別。以靜態函式鏈結的main程式可以順利執行，但是假設系統預設尋找函式庫的路徑裡找不到liboperator.so，以共享函式鏈結的main程式則會出現錯誤訊息：

`./main: error while loading shared libraries: liboperator.so: cannot open shared object file: No such file or directory`

這時解決的方法有四種：
- 把liboperator.so複製或是作一個連結到/usr/lib裡。

- 修改/etc/ld.so.conf，把/usr/local/foo/lib加進系統libraries的搜尋範圍內。

- 設定LD_LIBRARY_PATH變數，累加該路徑進來：
如果你不是系統管理員，前兩個方法根本沒辦法執行。我們只好自己加到~/.profile裡：

- export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/foo/lib
改用靜態函式進行鏈結。

共享函式經常出現在開放原始碼的linux世界裡，`由於使用所有函式皆是共享的`，因此許多程式都可以重複利用既有的功能；有新功能或是bug也能簡單的替換掉該函式，所有程式都可以即時享受到這樣的改變，也是最為常見的函式型態。

##動態函式(dynamic libraries)
動態函式跟共享函式非常類似，`唯一的差別在於程式執行時期並不會去檢查該函式是否存在，而是程式執行到某功能時才進行檢查`。這種動態載入的技術最常用在瀏覽器或是大型程式的外掛程式，當有需要用到這個功能時才載入進來。
製作一個動態函式比較麻煩。

```sh
gcc -c -fPIC operator.c
gcc -shared operator.o -o liboperator.so
```

其中的`-fPIC是產生position-independent code`，也可以用-fpic。詳細的用法已經超過筆者的理解範圍，撰寫呼叫動態函式的程式碼也需要傳入相關參數。關於更多dynamic libraries的用法請參考這裡。 

##邁向Makefile之路
說了這麼多，尚未觸碰到Makefile本身。以上的範例如果都夠清楚了，接下來的Makefile才能夠繼續學習下去喔！
以下是Makefile的基本組成：


```sh
# comments
#一些變數宣告
 
target1: dependencies of target1
<TAB>command;
target2: dependencies of target2
<TAB>command;
...
...
clean:
<TAB>rm -rf *.o  
```

需注意command那行前面必須是一個tab鍵，不能是tab鍵以外的任何空格。
用第一個簡單的範例來說明：假設你需要執行
```sh
gcc main.c foo1.c foo2.c -o main
```

才能編譯出main這隻程式，則Makefile會像是：

```sh
#example 1:
#usage: make main OR make
       
main: main.o foo1.o        
    gcc main.o foo1.o -o main  
main.o: main.c             
    gcc main.c -c          
foo1.o: foo1.c             
    gcc foo1.c -c          
clean:                 
    rm -rf main.o foo1.o   
```

make讀取此Makefile的流程如下：
- 由於沒有變數宣告的部份，程式進入點為line 4，target即為main。
main需要main.o跟foo1.o這兩個目的檔；如果gcc找得到這兩個目的檔，才會開始執行line 5的命令。
很不巧，gcc無法找到這兩個檔案(因為還沒有編譯過！)，因此gcc會尋找第一個dependency，也就是main.o，接續line 6。

- 到了line 6找到了main.o，他的dependency是main.c。
main.c就在這個目錄下，因此gcc終於可以執行第一個command(也就是line 7)，產生main.o並回到line 4

- 有了main.o，gcc會回到line 4繼續尋找第二個dependency--foo1.o：

- 於是進入line 8，找到了foo1.c，執行line 9的命令產生了foo1.o。

- 很高興的再次回到line 4，發現此時所有dependencies都滿足了，終於可以開始進行真正的鏈結工作，也就是line 5，把所有的obj鏈結成main這隻程式。

這個例子裡，make的效果等同於make main；可以不用指定main的原因是make會預設讀第一個target。假設你輸入make foo1.o，當然就只會執行line 8這行命令。 如果程式碼稍作修改，則編譯出來的obj檔也會有所不同(這是一個標準的廢話>.<)；此時有必要先清除某些(或全部的obj檔)。
如果我們下一個make clean的指令，則程式會跑到line 10；發現clean這個target並沒有dependency，而且也沒有clean這個檔案，此時這個項目稱為假項目(fake entry)。沒有相依的檔案，因此可以快樂的執行line 11，把main.o跟foo1.o刪除。


第二個範例：假設你需要執行

```sh
gcc main.c foo.c clean.c -I /usr/foo/include -lpthread -L /usr/foo/lib -O3 -ansi -o main
```

且目錄下包含target.h才能編譯出main這隻程式，則Makefile會像是：


```sh
#example 2
#usage: make main OR make
CC = gcc                   #欲使用的C compiler
CFLAGS = -O3 -ansi         #欲使用的參數
INC = -I /usr/foo/include   #include headers的位置
LIB = -L /usr/foo/lib       #include libraries的位置
       
main: main.o foo1.o                    
    ${CC} main.o foo1.o ${CFLAGS} ${INC} ${LIB} -o main
main.o: main.c target.h                    
    ${CC} main.c ${CFLAGS} ${INC} ${LIB} -lpthread -c  
foo1.o: foo1.c target.h                    
    ${CC} foo1.c ${CFLAGS} ${INC} ${LIB} -c        
clean:                             
    @rm -rf *.o   
```

這裡宣告了四個變數，在Makefile裡變數可以用$(VAR)或是${VAR}來表示皆可。但是為了跟shell script視覺上有所區隔，我個人建議盡量使用${VAR}來表示。
跟剛剛的Makefile其實是大同小異，只是利用變數使make更加的靈活；執行的流程可參考上一個範例。唯一值得注意的是在line 15的command前我用了一個@符號，這個意思是用來表示`不把執行命令輸出到螢幕，僅輸出結果的意思`。make預設會把命令跟結果都輸出到螢幕，利用@可簡化輸出，使make的結果更簡潔一點點。

如果你仔細觀察這兩個範例，會發現其實這個檔案本身有太多東西是重複的了。例如line {8,9}重複了main.o, foo1.o；line {10,11}重複了mian.c，而line {12,13}重複了foo1.c。想想這只是一個極小的程式，他的Makefile就要如此巨大，往後如果開發出數百個方法的中型程式，那麼Makefile可能會寫到手軟；更可怕的是程式的架構如果一改變，Makefile寫錯得機會會非常高。

在繼續第三個範例之前，我們來思考一個問題。如果你有foo{1,2,3...100}.c，要把他們寫進Makefile裡，定義targer: depencency然後定義command，加起來總共要兩百行，這實在不是絕妙的方法；make的開發者也想到了這點，因此make有隱含規則(implicit rules)：


```sh
main.o: main.c
    gcc main.c -c 
```
可以隱含簡化成
```sh
main.o: main.c
```

或者是當你根本沒有定義main.o這個target時，make會自動找main.c來編譯。這是個好消息，但是我們編譯程式通常會夾帶大量參數，光是使用隱含規則是不夠用的；因此我們有需要去自訂一個隱含規則。

第三個範例：

```sh
#example 3
#usage: make main OR make
SHELL = /usr/bin/bash         #宣告command所使用的shell環境為bash
CC = gcc                               #欲使用的C compiler
CFLAGS = -O3 -ansi              #欲使用的參數
INC = -I /usr/foo/include  
LIB = -L /usr/foo/lib      
       
.SUFFIXS: .c .cpp .f77 .f       #加入所列副檔名到隱含規則裡
main: main.o foo1.o foo2.o                                    
    ${CC} main.o foo1.o ${CFLAGS} ${INC} ${LIB} -o $@
%.o: %.c target.h                      
    ${CC} $< ${CFLAGS} ${INC} ${LIB} -lpthread -c          
.PHONY: clean
clean:                             
    @rm -rf *.o  
```

這個Makefile看起來開始嚇人了！首先宣告這個Makefile所使用到的command是bash的語法。如果不需告則預設是sh，但是linux的sh就是bash，因此如果你是csh的擁護者，請你一定要宣告她。並且要注意的是，在GNU make裡，變數與變數值之間可以有空格(VAR = value，這個習慣跟csh一樣)也可以沒有空格(VAR=vlaue，這個習慣跟bash一樣)；不過如果在其他平台，如Solaris、HPUX或是AIX，很可能要使用具有空格的形式宣告才行。為了Makefile的可移植性，建議使用具有空格的表示方法。

SUFFIXS與PHONY都是變數，代表隱含、內定的target。例如宣告了.c, .cpp, .f77, .f這些副檔名到SUFFIXS變數，是告訴make這些副檔名也要加入隱含規則的行列。事實上，.c, .cpp, .o都已經在make的隱含規則裡了，再次宣告只是為了讓閱讀者更加明確知道這些檔案會被隱含規則處理。而PHONY變數則是讓make知道該target不是某個檔案，只是一個標記。假設make跑到line 15，發現沒有dependency，而工作目錄內恰好有一個clean的檔案，make會認為無條件需求而不去執行我們所要求的clean的動作；為了解決這個極少發生的窘境，細心的開發者還是會把PHONY變數加進Makefile裡。

line 11所出現的$@以及line 13出現的$<稱為自動變數，$@代表target本身，$<代表第一個dependency。line 12大量出現的%則是樣式規則，她就是幫助我們簡化Makefile最好的朋友。

- 缺少main.o時，make跳到line 12進行我們所自訂的隱含規則進行編譯：
此時的%.o就是main.o，%.c就是main.c。line 13的$<代表main.c，執行完畢會產生main.o。

- 發現還是缺少foo1.o，make再次跳到line 12：
此時的%.o就是foo1.o，%.c就是foo1.c。line 13的$<代表foo1.c，執行完畢會產生foo1.o。

- 發現還是缺少foo2.o，make再次跳到line 12：
此時的%.o就是foo2.o，%.c就是foo2.c。line 13的$<代表foo2.c，執行完畢會產生foo2.o。

因此，利用隱含規則，不但可以應付更複雜的架構，也可以使Makefile更容易閱讀、維護。我們來看一個真實的Makefile：


```sh
#-----------------------------------------------------------------------------
UP_CC := g++
 
BIN = $(PWD)/../../bin
 
COPT =-g
 
CINC = -I. -I $(PWD)/../PPP -I $(PWD)/../include
 
CLIB = -L $(PWD)/../lib -largtable2 -lfftw3
 
OBJS = gwlCreateAxis.o gwlSignalGen.o gwlCft.o gwlCwt.o gwlConvert.o\
      gwlIwt.o gwlDiffeoLin.o gwlSignalSum.o gwlSignalRead.o gwlCwtMaxLine.o\
      gwlET2D.o gwlET2DFilter.o gwlET3D.o gwlET3DFilter.o gwlDispModel.o\
      gwlDiffeoDisp.o gwlAutoCorr.o gwlTransFK.o gwlOptiSP.o gwlOptiSI.o\
      gwlSignalFilter.o gwlNNpred.o gwlWavelets.o
 
EXE = $(OBJS:.o=)
 
#-----------------------------------------------------------------------------
# clear suffix list and set new one
.SUFFIXES:
.SUFFIXES: .cpp .o
 
#-----------------------------------------------------------------------------
all : shell qwtplot installshell installqwtplot
 
shell : $(OBJS) $(EXE)
 
installshell :
    cp -f $(EXE) $(BIN)
 
qwtplot : gwlPlot.o gwlPlot
 
installqwtplot :
    cp -f gwlPlot $(BIN)
 
gwlPlot.o : gwlPlot.cpp
    $(QTDIR)/bin/moc gwlPlot.cpp -o gwlPlot_moc.cpp;
    $(UP_CC) -c -g -Wno-deprecated gwlPlot.cpp $(CINC) -I $(QTDIR)/include
 
gwlPlot : gwlPlot.o
    $(UP_CC) -g  $@.o $(COPT) -L $(PWD)/../lib -L $(QTDIR)/lib -largtable2 -lfftw3 -lqwt -o $@
 
.cpp.o :
    $(UP_CC) -c -Wno-deprecated $< $(CINC)
 
$(EXE) : $(OBJS)
    $(UP_CC)  $@.o $(COPT) $(CLIB) -o $@
 
clear :
    rm -f $(OBJS) gwlPlot.o
 
clean : clear
    rm -f $(EXE) gwlPlot
 
dependencies.make:
    touch dependencies.make
 
dep:
    $(UP_CC) $(COPT) $(CINC) -MM *.cpp > dependencies.make   
```


這個Makefile寫得相當的完整且嚴謹，是個很值得我們學習的好範例。首先，作者使用了變數替換(Variable substitution)，也就是UP_CC := g++。這個替換的條件是：如果UP_CC沒有被設定值，則自動設定變數值為g++。其次，作者使用了巨集以及巨集變數替換(Macro Variables Substitution)的語法：以此範例來說，是設定一個OBJS的巨集，包含了所有的.o檔。

巨集變數替換的語法巧妙的使用在定義EXE變數這一行。請注意:與=之間的字串：假設我定義了一個巨集JX = foo1.jx foo2.jx foo3.jx foo4.jx，然後我可以利用JAVA = ${JX:.jx=.java}定義foo{1,2,3,4}.java，利用CPP = ${JX:.jx=.cpp}定義foo{1,2,3,4}.cpp這些檔案。因此本例的$(OBJ:.o=)表示EXE巨集的所有檔案都是沒有副檔名的，可以預見他會利用這個技巧來產生binaries -- gwlCreateAxis.o產生gwlCreateAxis，gwlSignalGen.o產生gwlSignalGen，以此類推。

line 45的.cpp.o :是%.cpp: %.o的縮寫，千萬別誤會這是一個標記或假項目喔！line 48作者自訂了一個隱含方法，與前例的line 12是相同的意義。

作者很謹慎的利用SUFFIXES(SUFFIXS也適用在GNU make)來定義隱含規則的list。第一個.SUFFIXES:後面沒有接任何東西，表示清空suffix清單；第二個.SUFFIXES:接了.cpp .o兩個副檔名，告訴make只需要關心這兩者就好，其他不必花費心思隱含編譯。只是gcc在編譯時，-I以及-L預設都會把當前目錄包含進去，其實-I. 是多餘的；但是整體的撰寫方式算是嚴謹清晰且流暢！
了解了target與dependency的關係以及隱含規則的符號之後，還剩下一個大重點：command。每行command都是一次獨立的command，彼此毫無關聯性。如果要具有連貫性的command必須寫成連續的一行，例如：


```sh
%.o: %.c
    ${CC} $< -c; cd /somewhere; ./configure --prefix $(PWD); rm -rf .libs ${PWD}/share ${PWD}/bin;
```
如果你寫成

```sh
%.o: %.c
    ${CC} $< -c; cd /somewhere;
    ./configure --prefix $(PWD);
    rm -rf .libs ${PWD}/share ${PWD}/bin;
```


會變成進入到/somewhere之後，整個shell就結束並重新執行./configure --prefix $(PWD)。但是根據原意，$(PWD)是/somewhere而非其他任何路徑，因此整個command會造成預期外的結果。較恰當的寫法應該是：


```sh
%.o: %.c
    ${CC} $< -c; cd /somewhere; \
    ./configure --prefix $(PWD); \
    rm -rf .libs ${PWD}/share ${PWD}/bin;
```

利用跳脫字元把所有command當成同一行，在閱讀上也具有排版的效果。是否發現在Makefile裡我們並沒有定義PWD這個變數值？那是因為Makefile可以直接存取環境變數。但是每個人針對某個程式所設定環境變數不盡相同，如果有使用到非系統面的環境變數，則還是要宣告在Makefile裡比較恰當。例如以下這個不好的真實範例：

```sh
CFLAGS = -O ${GMT_INC}
    pssac: pssac.o sacio.o
    $(LINK.c) -o $@ $@.o sacio.o $(GMT_LIBS)
 
clean:
    rm -f pssac *.o
```

首先，將${VAR}跟$(VAR)混合使用是容易造成混淆的；第二，使用者並不知道什麼是LINK.c，從這個名稱實在無法猜出到底該給她什麼變數值。第三，GMT_INC與GMT_LIBS看起來像是某個程式的headers與libraries路徑，但是要碰巧有個使用者跟作者一樣有相同的環境變數名稱，是很困難的。第四，其實可以利用$<來代替$@.o，因為他是第一個dependency；第五，sacio.o是作者給的，並不是使用者可以自己編譯的。這會造成不同平台的使用者極大的困擾 --即使她所有的變數都辛苦的解決了，但是她的硬體是sparc，若這個sacio.o是linux x86或其他平台上編譯的，那麼這個程式根本就不可能編譯成功。
非常感謝我的老友焜銘給我許多指導、建議以及內容的指正，又提供我一個製作程式碼highlight以及行數的網站，讓我寫範例時能更輕鬆！

[quickhighlighter.com](http://quickhighlighter.com/)

[Advanced Syntax Highlighting](http://seotools.devshed.com/tools/advanced-syntax-highlighting/)

本文利用三個虛擬範例以及兩個真實範例來對Makefile作最簡單的介紹，實際上make的工具跟功能實在太多，筆者只能就自己的理解以及能力範圍作簡單的筆記跟介紹。真實範例的作者筆者都懷著十分尊敬的態度，將他們的Makefile作為優劣的教學範例也僅僅只是就事論事，絲毫不減我對這些貢獻自己程式碼的老師與前輩的尊敬之意。當然，這篇文章參考了非常多人的智慧結晶；如果您的Makefile難度超過本篇筆記更是理所當然，也許參考筆者所參考過得文章，對您而言會是個比較省時的選擇：


## 參考
GNU make使用手冊 中譯版

如何寫一個簡單的Makefile
裡面包含許多章節，由王森所翻譯，其翻譯的品質非常的高。

用Open Source工具開發軟體: 新軟體開發觀念
黃郁熙前輩所撰寫，非常有系統的教學。

Using GCC to create static and shared library
PTT版友cole945所撰寫，針對各種libraries作詳盡的介紹。

Makefile 語法簡介
Tetralet前輩所撰寫的一篇好文！

Makefile教學(一) 、(二)、(三)
ytha用非常輕鬆簡單的口吻解釋Makefile。

makefile 的基本語法與變數
鳥哥學長所寫的Makefile介紹！