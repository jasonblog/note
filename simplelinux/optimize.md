
<a name="top"></a>

<h1 align="center">編譯優化
</h1>

　　C語言沒有彙編快，因為C語言要由編譯器翻譯為彙編，
編譯器畢竟是人造的，翻譯出來的彙編源代碼總有那麼N條
指令在更智能、更有創造性的我們看來是多餘的。

　　C語言翻譯後的彙編有如下惡劣行徑：

1. <b>C語言偏愛內存</b>。我們寫的彙編一般偏愛寄存器，
寄存器比內存要快很多倍。當然，寄存器的數量屈指可數，
數據多了的話也必須用內存。
2. <b>內存多餘讀</b>。假如在一個 for 循環中經常要執行
++i 操作，編譯後的彙編可能是這樣的情形：

```c
movl i, %eax
addl $1, %eax
movl %eax, i
```
即使 eax 寄存器一直存著 i 的值，
C語言也喜歡操作它前先讀一下，以上3條指令濃縮為一條
incl %eax 速度就快上好幾倍了。

儘管C語言"如此不堪"，但是考慮到高級語言帶來的
源碼可讀性和開發效率在數量級上的提高，我們還是原諒了它。
而且很多編譯器都有提供優化的選項，
開啟優化選項後C語言翻譯出來的彙編代碼幾近無可挑剔。

VC、VS有 Debug、Release 編譯模式，
Release 下編譯後，程序的大小、執行效率都有顯著的改善。
gcc 也有優化選項，我們來看看 gcc 優化的神奇效果：

我故意寫了一個垃圾程序（math.c）：

```c
#include <stdio.h>

int main()
{
	int a=1, b=2;
	int c;

	c = a + a*b + b;

	printf("%d\n", c);
	return 0;
}
```
且看看不優化的情況下，彙編代碼有多麼糟糕：

編譯命令：gcc -S math.c
main部分的彙編代碼：
```c
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$1, 28(%esp)	# 28(%esp) 是 a
	movl	$2, 24(%esp)	# 24(%esp) 是 b
	movl	24(%esp), %eax	#\
	addl	$1, %eax		#-\
	imull	28(%esp), %eax	#-eax=(b+1)*a
	addl	24(%esp), %eax	#\
	movl	%eax, 20(%esp)	#-c=(b+1)*a+b
	movl	$.LC0, %eax
	movl	20(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	$0, %eax
	leave
	ret
```
彙編代碼規模龐大，翻譯水平中規中矩。
現在開啟優化選項：

編譯命令：gcc -O2 -S math.c

```c
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$16, %esp
	movl	$5, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	xorl	%eax, %eax
	leave
	ret
```

規模變為原來的一半，而且 gcc 發現了 a、b、c
變量是多餘的，直接將結果 5 傳給 printf 打印了出來
——計算器是編譯器必備的一大技能。
初中那時候苦逼地做計算題，怎麼就不學學C語言呢O(∩_∩)O~
