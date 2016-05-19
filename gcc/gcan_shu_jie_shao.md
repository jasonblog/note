# g++參數介紹


##[介紹] 


gcc and g++分別是gnu的c & c++編譯器   
gcc/g++在執行編譯工作的時候，總共需要4步   

```
1.預處理,生成.i的文件  

    預處理器cpp
2.將預處理後的文件不轉換成彙編語言,生成文件.s  

    編譯器egcs
3.有彙編變為目標代碼(機器代碼)生成.o的文件

    彙編器as
4.連接目標代碼,生成可執行程序

    連接器ld
```


###1.總體選項

-E   
只激活預處理,這個不生成文件,你需要把它重定向到一個輸出文件裡   
面.   
例子用法:   
```sh
gcc -E hello.c > pianoapan.txt   
gcc -E hello.c | more   
```
慢慢看吧,一個hello word 也要與處理成800行的代碼      
-S   
只激活預處理和編譯，就是指把文件編譯成為彙編代碼。   
例子用法   
```sh
gcc -S hello.c   
```

他將生成.s的彙編代碼，你可以用文本編輯器察看     
-c    
只激活預處理,編譯,和彙編,也就是他只把程序做成obj文件   
例子用法:   
```sh
gcc -c hello.c
```

他將生成.o的obj文件  

###2.目錄選項

-Idir   
在你是用#include"file"的時候,gcc/g++會先在當前目錄查找你所制定的頭   
文件,如果沒有找到,他回到缺省的頭文件目錄找,如果使用-I制定了目錄,他   
回先在你所制定的目錄查找,然後再按常規的順序去找.   
對於#include,gcc/g++會到-I制定的目錄查找,查找不到,然後將到系   
統的缺省的頭文件目錄查找    

-include file

-i

相當於「#include」
包含某個代碼,簡單來說,就是便以某個文件,需要另一個文件的時候,就可以   
用它設定,功能就相當於在代碼中使用#include   
例子用法:   
```sh
gcc hello.c -include /root/pianopan.h   
```

-I-   
就是取消前一個參數的功能,所以一般在-Idir之後使用   
-idirafter dir   
在-I的目錄裡面查找失敗,講到這個目錄裡面查找.   
-iprefix prefix   
-iwithprefix dir   
一般一起使用,當-I的目錄查找失敗,會到prefix+dir下查找    
-Ldir   
制定編譯的時候，搜索庫的路徑。比如你自己的庫，可以用它制定目錄，不然   
編譯器將只在標準庫的目錄找。這個dir就是目錄的名稱。  
-llibrary    
制定編譯的時候使用的庫   
例子用法   

```
gcc -lcurses hello.c   
```

使用ncurses庫編譯程序   

###3.調試選項

-g   
只是編譯器，在編譯的時候，產生調試信息。   

-gstabs   
此選項以stabs格式聲稱調試信息,但是不包括gdb調試信息.   

-gstabs+   
此選項以stabs格式聲稱調試信息,並且包含僅供gdb使用的額外調試信息.   

-ggdb    
此選項將儘可能的生成gdb的可以使用的調試信息.

-glevel
請求生成調試信息，同時用level指出需要多少信息，默認的level值是2

###4.鏈接方式選項：

- -static 此選項將禁止使用動態庫。

優點：程序運行不依賴於其他庫

缺點：文件比較大

- -shared (-G) 此選項將儘量使用動態庫，為`默認選項`

優點：生成文件比較小

缺點：運行時需要系統提供動態庫

-symbolic 建立共享目標文件的時候,把引用綁定到全局符號上.

對所有無法解析的引用作出警告(除非用連接編輯選項 `-Xlinker -z -Xlinker defs'取代)。

註：只有部分系統支持該選項.

###5.錯誤與告警選項

-Wall 一般使用該選項，允許發出GCC能夠提供的所有有用的警告。也可以用-W{warning}來標記指定的警告。

-pedantic 允許發出ANSI/ISO C標準所列出的所有警告

-pedantic-errors 允許發出ANSI/ISO C標準所列出的錯誤

-werror 把所有警告轉換為錯誤，以在警告發生時中止編譯過程

-w 關閉所有警告,建議不要使用此項

###6.預處理選項

-Dmacro   
相當於C語言中的#define macro   
-Dmacro=defn   
相當於C語言中的#define macro=defn   
-Umacro   
相當於C語言中的#undef macro   
-undef   
取消對任何非標準宏的定義 

###7.其他選項
-o

制定目標名稱,缺省的時候,gcc 編譯出來的文件是a.out,很難聽,如果你和我有同感，改掉它,哈哈

例子用法
```sh
gcc -o hello.exe hello.c (哦,windows用習慣了)   
gcc -o hello.asm -S hello.c   
-O0   
-O1   
-O2
-O3
```

編譯器的優化選項的4個級別，-O0表示沒有優化,-O1為缺省值，-O3優化級別最高
```sh
-fpic 編譯器就生成位置無關目標碼.適用於共享庫(shared library).
-fPIC 編譯器就輸出位置無關目標碼.適用於動態連接(dynamic linking),即使分支需要大範圍轉移.
```

-v 顯示詳細的編譯、彙編、連接命令

-pipe   
使用管道代替編譯中臨時文件,在使用非gnu彙編工具的時候,可能有些問題   
```sh
gcc -pipe -o hello.exe hello.c   
```

-ansi   
關閉gnu c中與ansi c不兼容的特性,激活ansi c的專有特性(包括禁止一些asm inline typeof關鍵字,以及UNIX,vax等預處理宏,   

-fno-asm   
此選項實現ansi選項的功能的一部分，它禁止將asm,inline和typeof用作關鍵字。   

-fno-strict-prototype   
只對g++起作用,使用這個選項,g++將對不帶參數的函數,都認為是沒有顯式的對參數的個數和類型說明,而不是沒有參數.而gcc無論是否使用這個參數,都將對沒有帶參數的函數,認為城沒有顯式說明的類型   

-fthis-is-varialble   
就是向傳統c++看齊,可以使用this當一般變量使用.   

-fcond-mismatch   
允許條件表達式的第二和第三參數類型不匹配,表達式的值將為void類型   
```sh
-funsigned-char   
-fno-signed-char   
-fsigned-char   
-fno-unsigned-char   
```
這四個參數是對char類型進行設置,決定將char類型設置成unsigned char(前   
兩個參數)或者 signed char(後兩個參數) 

-imacros file   
將file文件的宏,擴展到gcc/g++的輸入文件,宏定義本身並不出現在輸入文件中       

-nostdinc   
使編譯器不再系統缺省的頭文件目錄裡面找頭文件,一般和-I聯合使用,明確限定頭文件的位置   

-nostdin C++   
規定不在g++指定的標準路經中搜索,但仍在其他路徑中搜索,.此選項在創建libg++庫使用   

-C   
在預處理的時候,不刪除註釋信息,一般和-E使用,有時候分析程序，用這個很方便的   

-M   
生成文件關聯的信息。包含目標文件所依賴的所有源代碼你可以用gcc -M hello.c來測試一下，很簡單。   

-MM   
和上面的那個一樣，但是它將忽略由#include造成的依賴關係。   

-MD   
和-M相同，但是輸出將導入到.d的文件裡面   

-MMD   
和-MM相同，但是輸出將導入到.d的文件裡面   

-Wa,option   
此選項傳遞option給彙編程序;如果option中間有逗號,就將option分成多個選項,然後傳遞給會彙編程序   

-Wl.option   
此選項傳遞option給連接程序;如果option中間有逗號,就將option分成多個選項,然後傳遞給會連接程序.


-x language filename   
設定文件所使用的語言,使後綴名無效,對以後的多個有效.也就是根   
據約定C語言的後綴名稱是.c的，而C++的後綴名是.C或者.cpp,如果   
你很個性，決定你的C代碼文件的後綴名是.pig 哈哈，那你就要用這   
個參數,這個參數對他後面的文件名都起作用，除非到了下一個參數   
的使用。   
可以使用的參數嗎有下面的這些

```
`c', `objective-c', `c-header', `c++』, `cpp-output',    
`assembler', and `assembler-with-cpp'.
```

看到英文，應該可以理解的。   
例子用法:   
```sh
gcc -x c hello.pig   
```

-x none filename   
關掉上一個選項，也就是讓gcc根據文件名後綴，自動識別文件類型   
例子用法:   
```sh
gcc -x c hello.pig -x none hello2.c
```