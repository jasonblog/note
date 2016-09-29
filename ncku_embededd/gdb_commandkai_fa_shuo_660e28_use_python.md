# GDB command開發說明(use Python)


﻿# GDB command開發說明(use Python)

如果你問我為何想自幹個自動化測試系統

我只會回答這句：

## Coding without tears !!!!!!!!!!!!!!!!

(參考出處： [快快樂樂學線代 (上篇)](https://ccjou.wordpress.com/2009/03/18/%e5%bf%ab%e5%bf%ab%e6%a8%82%e6%a8%82%e5%ad%b8%e7%b7%9a%e4%bb%a3-%e4%b8%8a%e7%af%87/)中  Learning linear algebra without tears)

好  

閒話不多說

來說說我目前gdb  開發python  command的情況

我會以[test_](https://github.com/KevinKu/sort_by_Disruptor/blob/Disruptor/test_tool.py)

[tool.py](https://github.com/KevinKu/sort_by_Disruptor/blob/Disruptor/test_tool.py)為例子  說明如何撰寫

（也實際動手操作一次吧）

*

[](https://gist.github.com/KevinKu/f9d859eade4d848dd084e2d0bc828c8e)https://gist.github.com/KevinKu/f9d859eade4d848dd084e2d0bc828c8e

先從最基礎開始.....

這是把test_tool.py刪減   只剩剛好可以使用的

**import gdb**

這是最重要的   想要使用gdb的API一定要import

 ** class test_gdb_python_command(gdb.Command):**

*   command用宣告成class的方式  然後再弄個物件來使用.........

*   這邊如果用   """   """

*   裡面可以寫這個command的敘述   

*   在gdb  可用help   command_name來查詢敘述

*   **  def __init__(self):**

*   物件初始化的函數  我是用super  基本上不了解也沒關係

*   沒用super函數  個人試過是無法初始化物件  

*   這樣command就不能用了.......

*   **def invoke(self,arg,from_tty):**

*   這邊就是command真正要撰寫的功能拉

*   常用的參數就只有arg   

**test_gdb_python_command()**

整個.py的文件  需要個物件才能使用command

基本上在class下面補一行就可以拉

好拉  我們來看怎麼使用.........

（先用source把該.py檔案弄進來）

(gdb) 

(gdb) source command.py 

這個我把它存成 command.py

(gdb) help te

test_gdb_python_command  teval

(gdb) help test_gdb_python_command 

test help

這邊就是上面所說  command敘述可以用help查詢的部份  對於使用來說很好用

(gdb) test_gdb_python_command 

gdb test

(gdb) 

以上是測試的結果

然後arg的部份

用splite(" ")

去分開command的參數

gdb.execute(c,True,True)

這個很重要拉

後面兩個參數基本上我習慣是用True  詳細的意義去看官方文件......

c是在GDB使用的指令  基本上是個字串

例如

c = "p a"

之類的

這個函數使用

就可以讓GDB執行c的指令

算是用來控制GDB的

要建立測試環境

監測某些變數  通通可以靠這個作到..........

剩下的就自己看test_tool.py的內容拉拉拉

然後   印出array不需要寫while拉

用@就可以了

gdb的指令

p  array_name@length

接下來玩些好玩的

source除了把指令初始化

還能不能跑一段code勒？

如果不能的話  那這樣自動化測試還是需要個額外的啟動腳本

可以的話

就直接  gdb -x script_name  test_elf_file就好了

好拉

先在 當前目錄下  打開一個檔案

檔案名為test_result

在這簡單的.py下

test_gdb_python_command()後面

補上這段

*

[](https://gist.github.com/KevinKu/131333914f3fa9e282deec2217e544f9)https://gist.github.com/KevinKu/131333914f3fa9e282deec2217e544f9

以上的code會在test_result內紀錄斷點的資訊

接著就是測試的結果拉

**mtath@Inspiron-5559:~/sort_by_Disruptor$ cat test_result **

**mtath@Inspiron-5559:~/sort_by_Disruptor$ gdb -x test_tool.py  test_Disruptor>>/dev/null**

**mtath@Inspiron-5559:~/sort_by_Disruptor$ cat test_result **

**Num     Type           Disp Enb Address    What**

**1       breakpoint     keep y   0x08048936 in main at test_main.c:348**

**        breakpoint already hit 1 time**

**mtath@Inspiron-5559:~/sort_by_Disruptor$ **

可以不用bash拉

寫到這要做點補充了

雖然說gdb.execute說去官網看

（[gdb.execute](https://sourceware.org/gdb/onlinedocs/gdb/Basic-Python.html#Basic-Python)）

以我習慣的用法  這個函數會return回指令執行的結果

另外  某些情況  它會丟出例外

需要用try去處理....

ref:[23.2 Extending gdb using Python](https://sourceware.org/gdb/onlinedocs/gdb/Python.html#Python)

ref:[Disruptor  sort 開發紀錄](https://hackpad.com/Disruptor-sort--27u4VrkgYZ6)

## 改進的空間

1.gdb指令的字串拼接：

想寫個lib來處理這問題

2.效能測試遇到的改函數命名  改source code.......：

這問題麻煩.........

為了效能

經常會改資料結構   函數命名

可是改完後

還是要保證業務邏輯是對的

可是這樣會令之前的command比較難複用

現在在想要怎麼處理..........

當然  自然的  測資檔可能格式也得改

目前是考慮用sed  awk之類的程式來處理.......

3.組合測試複用一些Unit Test所寫command的測資問題：

習慣上  UT時  會自定義測資檔格式

可是如果一個函數是許多個小函數組成

一堆檔案不好管理  外加目前我所定義的測資檔

格式太死

目前考慮是用re  設計一個filter  看能不能讓測資檔案的格式更靈活  

順便做到測資檔可以自解釋 

這東西還可以順便解決上面效能測試的一些問題

（或許  sed awk可以不需要用）

## 一些對測試用code的想法

這邊看看就好

個人以為

測試用的code  (bash   python  .....)

能不維護就不維護

為何  主要還是因為人力

另一方面是

這些code的用途專一   所以實際上code的品質也不需要很高

（它只需要在特定情況下能動就好了）

當然  有人會疑問  那效能勒

就我自己的想法是   如果弄出一個自動化測試系統

實際上開發  修改source code與測試是可以同時進行的

效能低一些可以忍受

（當然太低仍然不行  不過我覺得要看需求而定

看雲風的blog  有時我蠻認同這句話的：

能不去解問題就不去解...........

）

然後不用把自動化測試想是多高級的東西

它就只是替代掉gdb輸入指令的command而已

**主要重點在於減少重複的勞動**

我想社群選Python大概是看上膠水語言的特性了

好寫好改.....

又有現成好用的lib.......

(實際還有另一個語言也可以 Guile  )

測試是用來檢查自己實做的code到底有沒有達到目的（有些需求是不能用GDB檢查的 比如說  速度）

例如我寫個了swap函數好了

我想知道  這swap是不是真的能swap兩個node

所以我去寫測試

實做個排序

甚至是大數目的排序

（排10000個.......）

絕對不想自己一個一個打指令的........

當然就想知道這是不是真的能作到排序

（排10000個也要檢查很久的  所以這測試要後面做.....測試先從簡單  短小的開始）

GDB只是更快讓我們找到出問題的地方在那

解決問題還是要實做的人要去想的

自動化測試什麼時候使用？

這是個好問題..........

甚至某些需求下  沒有自動化測試才是最好的解法.........

有一個是需要的  就是這軟體  未來會新增功能  

而自動化測試可以輔助不會新增後出現新的bug.......

但我個人以為   這實際上是防止糟糕coder的破壞力用的

更好的應用是

實際運行發生bug

迅速定位code用的

要不要寫個自動化測試系統

一樣要評估效益的

## 問題與回答

沒人要問阿

*   你好，不好意思小弟才疏學淺，我想知道這東西具體可以使用在什麼地方上呢?

*   真是好問題阿
*   如果是像一次性的作業之類的  確實用不到
*   這東西是用在需要  高穩定性  需要持久維護 的軟體開發用的
*   比如說OS    遊戲的server  (有些遊戲本身也是)   lib  給別人用的framework......
*   跨國企業IT部門寫給企業自己用的會議系統......
*   工廠的機器人控制......
*   喔喔喔瞭解，謝謝
*   補充一下好了  有時  不寫自動化測試  而直接用GDB會更快

## 一些文章

[测试的道理](http://www.yinwang.org/blog-cn/2016/09/14/tests)