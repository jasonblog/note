[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">誰調用了main？
</h1>

　　這是函數幀的應用之一。

## 操作可行性

　　從上一篇中可以發現：用幀指針 ebp 可以回溯到所有的函數幀，
那麼 main 函數幀之上的函數幀自然也是可以的；
而幀中 舊ebp 的上一個四字節存的是函數的返回地址，
由這個地址我們可以判斷出誰調用了這個函數。

## 準備活動

　　下面就是這次黑客行動的主角（up.c）：

	#include <stdio.h>
	
	int main()
	{
		int *p;
		
		// 以下這行內聯彙編將 ebp 寄存器的值存到指針 p 中
		__asm__("movl %%ebp, %0"
				:"=m"(p));
		
		while(p != NULL){
			printf("%p\n", p[1]);
			p = (int*)(p[0]);
		}
		
		return 0;
	}

`　　`首先，請允許我使用一下 gcc 內聯彙編，
這裡簡單的解釋一下：

1. "=m"(p) 表示將內存變量 p 作為一個輸出操作數
2. %0 代表的是第一個操作數，那就是 p 了
3. 為了與操作數區別開來，寄存器要多加個 %，
%%ebp 表示的就是 ebp 寄存器

`　　`總之，這塊內聯彙編將 ebp 寄存器的值賦給了指針 p。

　　然後解釋一下while循環：循環中，首先打印 p[1]，
p[1]就是該幀所存的返回地址；然後將指針 p 改為 p[0]，
p[0]是 舊ebp（上一幀的幀指針）；
這樣，程序將按照<b>調用順序的逆序</b>打印出各個返回地址。

　　為什麼終止條件是 p==NULL 呢？這是 gcc 為了支援我們的
黑客行動特意在開始執行程序的時候將 ebp 清零了，
所以第一次執行某個函數的時候壓棧的 舊ebp 是 NULL。

## 開始行動

　　我們使用靜態鏈接的方式編譯 up.c
（靜態鏈接的可執行文件中包含所有用戶態下執行的代碼），
然後執行它：

	[lqy@localhost temp]$ gcc -static -o up up.c
	[lqy@localhost temp]$ ./up
	0x8048464
	0x80481e1
	[lqy@localhost temp]$ 

## 分析結果

　　up 打印了了兩個指向代碼區的地址，
接著就看它們是屬於哪兩個函數了：

	nm up | sort > up.txt

* nm up 可列出各個全局函數的地址
* | sort > up.txt 通過<b>管道</b>將 nm up 的輸出作為 sort 的輸入，
sort 排序後<b>輸出重定向</b>到 up.txt 文件中（輸出有1910行，
不得不這麼做o(╯□╰)o）

`　　`然後發現兩個地址分別位於 `__libc_start_main`、_start 中：

	...
	08048140 T _init
	080481c0 T _start
	080481f0 t __do_global_dtors_aux
	08048260 t frame_dummy
	080482bc T main
	08048300 T __libc_start_main
	080484d0 T __libc_check_standard_fds
	...

`　　`實際上程序正好是從 _start 開始執行的，
而且從 up 的反彙編結果中可看出 _start 的第一條指令
 xor %ebp,%ebp 就是那條傳說中的將 ebp 清零的指令
（兩個一樣的數相異或的結果一定是0）。

　　那麼調用 main 函數之前程序都幹了些啥事呢？
比如說<b>堆的初始化</b>，如果是 C++ 程序的話，
全局對象的構造也是在 main 之前完成的
（不能讓 main 中使用全局對象的時候竟然還沒構造吧！），
而全局對象的析構也相當有趣地在 main 執行完了之後才執行。

　　main 在你心目中的地位是不是一落千丈了？

[回目錄][content]
