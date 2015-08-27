<a name="top"></a>

<h1 align="center"><b>樸素linux</b></h1>

　　大學裡我堅持的最久的一項任務就是自學 linux 內核，
雖然以後可能也沒機會從事 linux 內核方面的工作，
但是至少提升了自己的編程水平。

　　linux 最新內核的源代碼已經沒有進行全面研究的可能了，
我看的是 linux0.01 的內核源碼。
沒有指導直接看源代碼是不太容易看懂的，
因為其中涉及到不少硬件操作的規範，
《linux內核完全註釋2.01》——趙炯 著 
對早期linux內核的分析是最詳細的，真的達到了完全註釋的地步，
雖然書裡分析的是 linux0.11 的源代碼，
但相對於 0.01 的改動不多。

　　而最新的內核由於巨大的代碼量，
要達到源代碼的完全註釋應該是不可能的了，
但是從大粒度上進行的分析也是很有價值的。
《Linux內核設計與實現》英文名為 
*Linux kernerl Development* ，
是 *Robert Love* 所著，陳莉君、康華、張波 翻譯的，
我從這本書中瞭解了最新內核的進程調度思想。

　　還有一本書，書名是《LINUX內核源代碼情景分析》 
毛德操 胡希明 著，這本書對於 PCI 
總線操作規範的介紹可謂完全註釋。為什麼我會看 PCI 
總線的操作？因為現在的電腦都是用 PCI 而非早期的 ISA 
總線了，linux0.01 對硬盤的操作使用 ISA 規定的固定端口，
而 PCI 總線中的硬盤的端口是動態設定的，
與 ISA 時的端口不一致了，所以如果想用 linux0.01 
讀寫我本機的硬盤的話就得加入 PCI 的功能，
所以我才看關於最新內核的書，我是被逼的。

　　就快要畢業去工作了，想著寫幾篇文章同大家分享一下 
linux 和 C 語言方面的底層知識。那些既想了解底層
又不願意系統地看源代碼或操作系統方面書籍的同學可以來看看，
就當是看一部小說吧。
這個系列的名字叫<b>樸素linux</b>。

　　以下是目錄，目錄隨著進度變更，
還有可能被重新分類整理，請見諒。

<a name="content"></a>

* 解剖C語言
	1. [照妖鏡和火眼金睛](https://github.com/1184893257/simplelinux/blob/master/gcc.md#top) \[2012/11/8更新](3)  
	怎麼獲得C語言翻譯後的彙編代碼，怎麼獲得消除宏的C源程序
	2. [局部變量](https://github.com/1184893257/simplelinux/blob/master/localvar.md#top) \[2012/11/12更新](4)  
	i=3; (++i)+(++i)+(++i) 不同編譯器結果不同，怎麼看它們的運算過程。
	3. [全局變量](https://github.com/1184893257/simplelinux/blob/master/globalvar.md#top) \[2012/11/8上線](5)  
	全局變量與局部變量在訪問方式上有什麼不同
	4. [函數調用](https://github.com/1184893257/simplelinux/blob/master/call.md#top) \[2012/11/9上線](6)  
	調用一個函數的前前後後
	5. [值傳遞](https://github.com/1184893257/simplelinux/blob/master/byval.md#top) \[2012/11/11上線](7)  
	C語言只有值傳遞，怎麼修改外部變量
	6. [數組與指針](https://github.com/1184893257/simplelinux/blob/master/array.md#top) \[2012/12/23更新](8)  
	數組的起始地址存在哪兒？
	7. [字符串](https://github.com/1184893257/simplelinux/blob/master/string.md#top) \[2012/11/15上線](9)  
	為什麼有的字符串不能修改
	8. [結構體](https://github.com/1184893257/simplelinux/blob/master/struct.md#top) \[2012/11/17上線](10)  
	結構體與子元素什麼關係，數組不能複製？
	9. [奇怪的宏](https://github.com/1184893257/simplelinux/blob/master/macro.md#top) \[2012/11/19上線](11)  
	do{...} while(0)是何用意
	10. [內存對齊](https://github.com/1184893257/simplelinux/blob/master/align.md#top) \[2012/11/28更新](12)  
	為什麼要進行內存對齊，怎麼關閉內存對齊
	11. [函數幀](https://github.com/1184893257/simplelinux/blob/master/frame.md#top) \[2012/11/24上線](13)  
	函數的局部環境：函數幀
	12. [函數幀應用一：誰調用了main？](https://github.com/1184893257/simplelinux/blob/master/main.md#top) \[2012/11/27上線](14)  
	不復雜
	13. [函數幀應用二：所有遞歸都可以變循環](https://github.com/1184893257/simplelinux/blob/master/recur.md#top) \[2012/11/30上線](15)  
	真的可以
	14. [未初始化全局變量](https://github.com/1184893257/simplelinux/blob/master/bss.md#top) \[2012/12/3上線](16)  
	未初始化全局變量 不跟 初始化全局變量 存一塊兒
	15. [進程內存分佈](https://github.com/1184893257/simplelinux/blob/master/mem.md#top) \[2012/12/6上線](17)  
	全局變量、堆、棧 在哪兒？訪問它們的特點
	16. [編譯優化](https://github.com/1184893257/simplelinux/blob/master/optimize.md#top) \[2012/12/9上線](18)  
	C語言比彙編慢，怎麼優化編譯過程
	17. [static變量 及 作用域控制](https://github.com/1184893257/simplelinux/blob/master/static.md#top) \[2012/12/12上線](19)  
	壓縮變量的作用域，提高源代碼的可讀性
	18. [變量名、函數名](https://github.com/1184893257/simplelinux/blob/master/name.md#top) \[2012/12/15上線](20)  
	變量名、函數名在哪裡終結，有什麼用？
	19. [函數指針](https://github.com/1184893257/simplelinux/blob/master/pfunc.md#top) \[2012/12/18上線](21)  
	函數指針跟普通指針有什麼區別
	20. [可變參數](https://github.com/1184893257/simplelinux/blob/master/varargs.md#top) \[2012/12/21上線](22)  
	可變參數怎麼實現的？變參函數的可行性？
	21. [C語言的棧是靜態的](https://github.com/1184893257/simplelinux/blob/master/staticstack.md#top) \[2012/12/23上線](23)  
	變參函數力不能及的地方
	22. [內聯彙編](https://github.com/1184893257/simplelinux/blob/master/inlineasm.md#top) \[2012/12/24上線](24)  
	gcc 以及 VC 的內聯彙編
	23. [彙編實現的動態棧](https://github.com/1184893257/simplelinux/blob/master/dynamicstack.md#top) \[2012/12/25上線](25)  
	實現一個運行時的接受可變參數的printf
* 內核小知識
	1. [linux0.01進程時間片的消耗和再生](https://github.com/1184893257/simplelinux/blob/master/process0.01.md#top) \[2012/11/7更新](1)
	2. [linux2.6.XX進程切換和時間片再生](https://github.com/1184893257/simplelinux/blob/master/process2.6.md#top) \[2012/11/7上線](2)
