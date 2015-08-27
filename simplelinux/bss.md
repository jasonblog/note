[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">未初始化全局變量
</h1>

　　為下一篇介紹進程內存分佈做準備，
這一篇先來介紹一下未初始化全局變量：

　　未初始化全局變量，這名字就很直白，就是 C 程序中定義成
全局作用域而又沒有初始化的變量，我們知道這種變量在程序運行
後是被自動初始化為 全0 的。編譯器編譯的時候會將這類變量
收集起來集中放置到 .bss 段中，<b>這個段只記錄了段長，
沒有實際上的內容（全是0，沒必要存儲），
在程序被裝載時操作系統會
為它分配等於段長的內存，並全部初始化為0</b>。

　　這有兩個 C程序，都定義了全局數組 data（長度為1M，
佔用內存4MB），一個部分初始化（bss\_init1.c），
一個未初始化（bss\_uninit1.c）：

bss_init1.c：

	#include <stdio.h>
	#include <windows.h>
	
	#define MAXLEN 1024*1024
	
	int data[MAXLEN]={1,};
	
	int main()
	{
		Sleep(-1);
		return 0;
	}

bss_uninit1.c：

	#include <stdio.h>
	#include <windows.h>
	
	#define MAXLEN 1024*1024
	
	int data[MAXLEN];
	
	int main()
	{
		Sleep(-1);
		return 0;
	}

`　　`編譯以上兩個程序後：

![bss1](http://fmn.rrfmn.com/fmn059/20121203/1935/original_4q5M_35d80000b351118d.jpg)

　　可以看到有初始化的可執行文件的大小差不多是4MB，
而未初始化的只有47KB！這就是 .bss 段有段長，
而沒有實際內容的表現。用 UltraEdit 打開 bss_init1.exe 
可看到文件中大部分是全0（data數組的內容）：

![bss5](http://fmn.rrimg.com/fmn065/20121203/1935/original_RbRN_5afd0000b341125d.jpg)

　　但是接下來運行（return 0 之前的 Sleep(-1) 保證了
程序暫時不會退出）的時候，卻發現 bss_init1.exe 
佔用的空間明顯少於 4MB，這是怎麼回事呢？

![bss2](http://fmn.rrimg.com/fmn065/20121203/1935/original_ejt4_363e0000b309118d.jpg)

　　這就涉及程序裝載的策略了。早期的操作系統（如：linux 0.01）
採用的是一次裝載：將可執行文件一次性完整裝入內存後再執行程序。
不管程序是 1KB 還是 60MB，都要等全部裝入內存後才能執行，
這顯然是不太合理的。

　　而現在的操作系統都是採用延遲裝載：
<b>將進程空間映射到可執行文件之後就開始執行了</b>，
執行的時候如果發現要讀/寫的頁不在內存中，
就根據映射關係去讀取進來，然後繼續執行應用程序
（應該是在頁保護異常的處理中實現的）。

　　bss_init1.exe 肯定是被映射了，<b>而程序中又沒有對 data 
數組進行讀/寫操作</b>，所以操作系統也就懶得去裝入這片內存了。
下面修改一下這兩個程序：在 Sleep(-1) 前將 data 數組
的每個元素賦值為 -1：

	int i;
	for(i=0; i<MAXLEN; ++i)
		data[i] = -1;

`　　`再運行，它們佔用的內存都是 4M 了：

![bss4](http://fmn.rrimg.com/fmn061/20121203/1935/original_OHR0_75660000b3e5118c.jpg)

[回目錄][content]
