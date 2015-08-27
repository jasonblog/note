[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">彙編實現的動態棧
</h1>

　　這一篇就是實現 d_printf，廢話不多說，直接上代碼。
由於 VC 的內聯彙編還是比較清晰，那就先貼 VC 版的。

## 一、d_printf VC版

	#include <stdio.h>
	
	void d_printf(const char *fmt, int n, int a[])
	{
		static int size1, size2;
		static const char *fmt_copy;
	
		size1 = 4*n;		// 可變參數的空間大小
		size2 = size1 + 4;	// 還有 fmt 指針4字節, 恢復 esp 時用
		fmt_copy = fmt;
		
		__asm{
			// 保護要修改的 ecx/esi/edi 寄存器
			push ecx
			push esi
			push edi
	
			// 給 ecx/esi/edi 賦值
			mov ecx, n	// movsd 的執行次數
			mov esi, a	// a -> esi
			sub esp, size1
			mov edi, esp	// esp - size1 -> edi
	
			rep	movsd		// n 次4字節拷貝
			push fmt_copy	// 壓棧格式串(字符串指針)
			call printf
	
			add esp, size2	// 恢復棧
			// 恢復各個寄存器
			pop edi
			pop esi
			pop ecx
		}
	}
	
	int main()
	{
		char fmt[1024];	// 格式串
		char c;			// 額外讀取一個字符
		int a[1024];	// 存讀到的整數
		int i;
		
		while(EOF != scanf("%s%c", fmt, &c)) // 讀到 EOF 就結束
		{
			if(c == '\n') // 格式串後沒有數字
			{
				printf("%s\n\n", fmt); // 直接打印, 不用 d_printf
				continue;
			}
	
			// 循環讀取各個整數
			i = 0;
			do
			{
				scanf("%d%c", &a[i++], &c);
			}while(c != '\n');
	
			// 調用 d_printf, i 剛好是輸入的整數的個數
			d_printf(fmt, i, a);
			printf("\n\n"); // 補個換行比較好看O(∩_∩)O~
		}
	}

## 二、d_printf gcc 版（main 函數跟 VC 版的一樣）

	#include <stdio.h>
	
	void d_printf(const char *fmt, int n, int a[])
	{
		int d0, d1, d2;
		
		static int size1, size2;
		static const char *fmt_copy;
	
		size1 = 4*n;		// 可變參數的空間大小
		size2 = size1 + 4;	// 還有 fmt 指針4字節, 恢復 esp 時用
		fmt_copy = fmt;
		
		asm volatile(
			"subl %6, %%esp\n\t"
			"movl %%esp, %%edi\n\t"
			"rep ; movsl\n\t"
			"pushl %3\n\t"
			"call printf\n\t"
			"addl %7, %%esp"
			: "=&S"(d0), "=&D"(d1), "=&c"(d2)
			: "m"(fmt_copy), "0"(a), "2"(n), "m"(size1), "m"(size2));
	}
	
	int main()
	{
		char fmt[1024];	// 格式串
		char c;			// 額外讀取一個字符
		int a[1024];	// 存讀到的整數
		int i;
		
		while(EOF != scanf("%s%c", fmt, &c)) // 讀到 EOF 就結束
		{
			if(c == '\n') // 格式串後沒有數字
			{
				printf("%s\n\n", fmt); // 直接打印, 不用 d_printf
				continue;
			}
	
			// 循環讀取各個整數
			i = 0;
			do
			{
				scanf("%d%c", &a[i++], &c);
			}while(c != '\n');
	
			// 調用 d_printf, i 剛好是輸入的整數的個數
			d_printf(fmt, i, a);
			printf("\n\n"); // 補個換行比較好看O(∩_∩)O~
		}
	}

## 三、運行效果

　　linux 中的運行效果如下：

	[lqy@localhost temp]$ ./d_printf 
	nospaceword
	nospaceword
	
	%X 256
	100
	
	%d+%d=%d 1 2 3
	1+2=3
	
	>%3d>%03d> 3 3
	>  3>003>
	
	>%3d>%-3d> 3 3
	>  3>3  >
	
	[lqy@localhost temp]$ 

`　　`最後輸入 EOF 結束：Ctrl + D（linux）、Ctrl + Z
（windows）。<b>由於轉義符是編譯時處理的，printf 是不管的，
所以\n什麼的在這裡不管用^_^</b>。

## 四、為什麼用 static

　　d\_printf 中為什麼將 size1、size2、fmt_copy 聲明為
static 變量呢？

1. 不能用寄存器。size2 是在 call printf 之後使用的，
<b>但是後來我發現 printf 執行完後改動了好幾個寄存器的值</b>，
所以如果將 size2 保存到某個寄存器中是不行的
（難怪C語言喜歡內存，寄存器太不可靠了o(╯□╰)o）。
2. 不能用局部變量。不讓用寄存器就用內存唄！
<b>但是我們要自主修改 esp，
而局部變量有可能是通過 esp+常量偏移 定位的</b>
（如果是用 ebp+常量偏移（VC一般這麼用）定位的就沒問題），
所以 subl %6, %%esp 之後 到 addl %7, %%esp 之前
都不能使用局部變量，否則會定位錯誤。
所以才使用 static 變量，<b>
因為 static 變量是用絕對地址定位的，跟 esp 毫無關係</b>。

## 五、使用內聯彙編的建議

1. 不要用。
2. 如果非得用的話，儘量用 C 語言實現+-*/，
如 d_printf 中給 size1、size2 賦值；
內聯彙編只實現不得不用的部分（內聯彙編一般都短小精悍）。

<h2 align="center">《解剖C語言》就此完結。</h2>

[回目錄][content]
