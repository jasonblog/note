[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">static變量 及 作用域控制
</h1>

## 一、static變量

　　<b>static變量放在函數中，就只有這個函數能訪問它；
放在函數外就只有這個文件能訪問它。</b>
下面我們看看兩個函數中重名的static變量是怎麼區別開來的
（static.c）：

	#include <stdio.h>
	
	void func1()
	{
		static int n = 1;
		n++;
	}
	
	void func2()
	{
		static int n = 2;
		n++;
	}
	
	int main()
	{
		return 0;
	}

`　　`下面是編譯後的部分彙編：

	func1:
		pushl	%ebp
		movl	%esp, %ebp
		movl	n.1671, %eax
		addl	$1, %eax
		movl	%eax, n.1671
		popl	%ebp
		ret
	
	func2:
		pushl	%ebp
		movl	%esp, %ebp
		movl	n.1674, %eax
		addl	$1, %eax
		movl	%eax, n.1674
		popl	%ebp
		ret

`　　`好傢伙！編譯器居然"偷偷"地改了變量名，
這樣兩個static變量就容易區分了。

　　<b>其實static變量跟全局變量一樣被放置在 .data段 或 
.bss段 中，所以它們也是程序運行期間一直存在的，
最終也是通過絕對地址來訪問</b>。
但是它們的作用域還是比全局變量低了一級：
static變量被標識為LOCAL符號，全局變量被標識為GLOBAL符號，
在鏈接過程中，目標文件尋找外部變量時只在GLOBAL符號中找，
所以static變量別的源文件是"看不見"的。

## 二、作用域控制

　　作用域控制為的是提高源代碼的可讀性，
一個變量的作用域越小，它可能出沒的範圍就越小。

　　C語言中的變量按作用域從大到小可分為四種：
全局變量、函數外static變量、函數內static變量、局部變量：

1. 全局變量是殺傷半徑最大的：<b>不僅在定義該變量的源文件中可用，
而且在任一別的源文件中只要用 extern 聲明它後也可以使用</b>，
因此，當你看到一個全局變量的時候應該心生敬畏！
2. 函數外的static變量處於文件域中，
只有定義它的源文件中可以使用。如果你看到一個static變量，
那是作者在安慰你：哥們（妹子），這個變量不會在別的文件中出現。
3. 函數內static變量在函數的<b>每次調用</b>中可用（只初始化一次），
<b>它同以上兩種變量一樣在程序運行期間一直存在</b>，
所以它的功能是局部變量無法實現的。
4. 局部變量在函數的<b>一次調用</b>中使用，
調用結束後就消失了。

`　　`顯然，作用域越小越省心，
該是局部變量的就不要定義成全局變量，
如果"全局變量"只在本源文件中使用那就加個static。

　　即便是局部變量也還可以壓縮其作用域：

　　有的同學寫的函數一開頭就聲明瞭函數中要用到的所有局部變量，
一開始我也這麼做，因為我擔心：<b>如果把變量定義在循環體內，
是不是每一次循環都會給它們分配空間、回收空間，從而降低效率？</b>
但事實是它們的空間在函數的開頭就一次性分配好了（scope.c）：

	#include <stdio.h>
	
	int main()
	{
		int a = 1;
		{
			int a = 2;
			{
				int a = 3;
			}
			{
				int a = 4;
			}
		}
		return 0;
	}

編譯後的彙編代碼如下：

	main:
		pushl	%ebp
		movl	%esp, %ebp
		subl	$16, %esp
		movl	$1, -4(%ebp)
		movl	$2, -8(%ebp)
		movl	$3, -12(%ebp)
		movl	$4, -16(%ebp)
		movl	$0, %eax
		leave
		ret

`　　`各層局部環境中的變量a是subl $16, %esp一次性分配好的。
<b>由此可見不是每個{}都要分配回收局部變量，
一個函數只分配回收一次</b>。因此，
如果某個變量只在某個條件、循環中用到的話，
還是在條件、循環中定義吧，這樣，
規模比較大的函數的可讀性將提高不少，而效率絲毫沒有下降，
可謂是百利而無一害！

[回目錄][content]
