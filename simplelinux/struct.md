[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">結構體
</h1>

　　結構體是 C 語言主要的自定義類型方案，
這篇就來認識一下結構體。

## 一、結構體的形態

C源程序（struct.c）：
```c
#include <stdio.h>

typedef struct{
	unsigned short int a;
	unsigned short int b;
}Data;

int main()
{
	Data c, d;

	c.a = 1;
	c.b = 2;
	d = c;

	printf("d.a:%d\nd.b:%d\n", d.a, d.b);
	return 0;
}
```

賦值部分翻譯後：

```c
movw	$1, 28(%esp)	# c.a = 1
movw	$2, 30(%esp)	# c.b = 2
movl	28(%esp), %eax	#
movl	%eax, 24(%esp)	# d = c
```
可以看出：

* c.a 是在 28(%esp) 之後的2個字節
* c.b 是在 30(%esp) 之後的2個字節
* c 是 28(%esp) 之後的4個字節
* d 是 24(%esp) 之後的4個字節

不得不感嘆名字（結構體名字、子元素名字）再一次被拋棄了，
子元素名代表的是相對於結構體的偏移。

## 二、結構體的複製

大一的時候，老師千叮嚀萬囑咐：<b>數組不能複製！</b>，
但是當發現下面這個程序正常運行後，我困惑了（block.c）：

```c
#include <stdio.h>

typedef struct{
	char data[1000];
}Block;

Block a={{'a','b','c',}};

int main()
{
	Block b;

	b=a;

	puts(b.data);
	return 0;
}
```

Block a={{'a','b','c',}} 是對 a 的部分初始化，
'c' 後面自動填 0，寫成 Block a={{"abc"}} 也一樣，
C 語言對初始化還是很寬容的。

上面這個程序居然正常的編譯、運行了，這究竟是怎樣的逆天？
看看彙編部分：
```c
leal	24(%esp), %edx
movl	$a, %ebx
movl	$250, %eax
movl	%edx, %edi	# edi = &b
movl	%ebx, %esi	# esi = &a
movl	%eax, %ecx	# ecx = 250
rep movsl
```
我們發現程序確實通過 250 次 movsl 複製了一個"數組"。
其原因是：結構體是可以複製的，
結構體又可以包括任意類型的子元素，數組也行，
所以"數組"也被複制了。

那為什麼純粹的數組就不能複製呢？
我們可以這樣去理解：<b>一個變量能被複制的必要條件是
我們知道它的大小</b>。結構體做為自定義類型，
在編譯的時候編譯器必然存儲了它的子元素類型、個數等相關信息，
結構體的大小也就知道了；而數組一般只在乎它的類型和
起始地址，元素個數總是被忽視的（例如：
void func(char s[]) 可接受任何長度的字符數組做參數），
而且元素個數也沒有被當做數組的一部分存入內存，
所以數組的複製是不好實現的。

## 小結

　　如果給結構體下一個實在點的定義話，那就是：
<b>有格式的字節數組</b>。有了結構體後 C 語言的
變量類型就豐富多了，但是同時也要注意：

1. 超過 4 字節的結構體不宜做參數（參數傳遞浪費時間、空間），
換做指針更好。
2. 超過 4 字節的結構體不宜做返回值類型
（話說一般返回值都用 eax 來存，
那麼超過 4 字節的時候怎麼存呢？自己去探索吧！）。
