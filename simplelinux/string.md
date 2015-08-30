
<a name="top"></a>

<h1 align="center">字符串
</h1>

這一篇分析字符串，字符串經常被使用，
但是它的祕密也不少：

## 一、字符串的存儲位置

C源程序（string1.c）：

```c
#include <stdio.h>

int main()
{
	puts("Hello, World!");
	return 0;
}
```
我們直接看可執行文件的反彙編結果：

```
[lqy@localhost temp]$ gcc -o string1 string1.c
[lqy@localhost temp]$ ./string1
Hello, World!
[lqy@localhost temp]$ objdump -s -d string1 > string1.txt
[lqy@localhost temp]$
```

string1.txt 中的部分內容如下：

<pre><code>Contents of section .rodata:
 8048488 03000000 01000200 00000000 48656c6c  ............Hell
 8048498 6f2c2057 6f726c64 2100               o, World!.

...

080483b4 &lt;main>:
 80483b4:	55                   	push   %ebp
 80483b5:	89 e5                	mov    %esp,%ebp
 80483b7:	83 e4 f0             	and    $0xfffffff0,%esp
 80483ba:	83 ec 10             	sub    $0x10,%esp<b>
 80483bd:	c7 04 24 94 84 04 08 	movl   $0x8048494,(%esp)
 80483c4:	e8 27 ff ff ff       	call   80482f0 &lt;puts@plt></b>
 80483c9:	b8 00 00 00 00       	mov    $0x0,%eax
 80483ce:	c9                   	leave
 80483cf:	c3                   	ret
</code></pre>

可見 0x8048494 應該是 "Hello, World!" 的地址，
然後發現在 .rodata 段中 0x8048488 + 12 處正好存儲著
 "Hello, World!" 的各個字符的
<a target="_blank" href="http://www.asciitable.com/">ASCII碼</a>：
'H' 的 <a target="_blank" href="http://www.asciitable.com/">ASCII碼</a>是 0x48，
而感嘆號 '!' 的 <a target="_blank" href="http://www.asciitable.com/">ASCII碼</a> 碼是 0x21，
最後編譯器還自動的為我們添了個字符串結束符 0x00：
<b>只要是雙引號括起來的字符串，編譯器都會自動的為我們加結束符。</b>

　　由此我們發現：
<b>字符串和全局變量一樣做為靜態數據存儲在可執行文件中，
在使用的時候用常量地址來訪問。</b>

　　但是字符串被放在了 .rodata 段中，
這個段中的數據與 .text 段（代碼段）中的數據一樣
在可執行文件被載入內存運行時
都是隻讀的（這裡的只讀是通過分頁管理實現的：
在頁表表項中有一個位，設置為 1 表示該頁可寫，
設置為 0 表示該頁只讀；如果試圖向只讀的頁中寫入數據，
CPU 就會觸發頁保護異常）。
<b>所以源字符串中的任何字符都不能在程序運行時更改</b>。

## 二、字符串指針 和 字符數組

C源程序（string2.c）：

```c
#include <stdio.h>

int main()
{
	char *s1 = "1234567";
	char s2[]= "1234567";

	puts(s1);
	puts(s2);
	return 0;
}
```

可執行文件的反彙編結果的賦值部分如下：

```c
80483bd:	c7 44 24 1c c4 84 04 	movl   $0x80484c4,0x1c(%esp)
80483c4:	08
80483c5:	a1 c4 84 04 08       	mov    0x80484c4,%eax
80483ca:	8b 15 c8 84 04 08    	mov    0x80484c8,%edx
80483d0:	89 44 24 14          	mov    %eax,0x14(%esp)
80483d4:	89 54 24 18          	mov    %edx,0x18(%esp)
```

0x80484c4 是字符串"1234567"的地址，
所以：<b>常量字符串賦值給指針時傳遞的是源字符串的地址；
而賦值給局部字符數組時，要當成數字一個個拷貝到局部變量空間</b>。
因此第2種方式既浪費空間（4字節 vs 8字節）
又浪費時間（1個mov vs 4個mov）。但是第2種方式也不是
一無是處：<b>第1種方式傳遞的是源字符串的地址，
而源字符串在只讀頁中，無法修改，但是字符數組卻可以修改</b>。

經驗：如果程序中本來就沒想改動該字符串，
那就用指針吧；否則用 字符數組 或 動態申請的空間 來存。

## 三、格式描述符 和 轉義符

這個部分，我們來看看字符串中格式描述符和轉義符的來龍去脈，
C源程序（string3.c）：

```c
#include <stdio.h>

int main()
{
	printf("--------\n%d\n", 123);
	return 0;
}
```

### 彙編源文件中

gcc -S string3.c

結果如下：
```c
.LC0:
    .string	"--------\n%d\n"
```
### 目標文件中

```c
gcc -c string3.c
objdump -s -d string3.o > string3.txt
```
-c 默認輸出到 string3.o 文件中，

string3.txt 中的字符串：

```c
Contents of section .rodata:
 0000 2d2d2d2d 2d2d2d2d 0a25640a 00        --------.%d..
```

兩個'\n'被替換成了 0x0a，%d 沒變

### 可執行文件中
```c
gcc -o string3 string3.c
objdump -s -d string3 > string3.txt
```

可執行文件跟目標文件一樣：
```c
Contents of section .rodata:
80484a8 03000000 01000200 00000000 2d2d2d2d  ............----
80484b8 2d2d2d2d 0a25640a 00                 ----.%d..
```

所以我們知道了：<b>轉義符在變成二進制文件後
就被轉義為我們想表達的那個字符了</b>，
而格式描述符自然是要留給 printf 運行時用的。
知道這個有什麼用？如果我們來設計 printf 函數，
那麼轉義字符我們就不用操心了，
編譯器會把它們轉義過去的。

如果你想看看 printf 大概的實現，
linux 0.01 的 kernel/vsprintf.c 中有一個簡化的
（沒有實現浮點數的處理）實現，其中轉義字符是不操心的。

linux 各版本內核源代碼：<a target="_blank" href="http://www.kernel.org/pub/linux/kernel/">http://www.kernel.org/pub/linux/kernel/</a>
linux 0.01：<a target="_blank" href="http://www.kernel.org/pub/linux/kernel/Historic/">http://www.kernel.org/pub/linux/kernel/Historic/</a>

