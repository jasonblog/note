
<a name="top"></a>

<h1 align="center">函數指針
</h1>

## 一、函數指針的值

　　函數指針跟普通指針一樣，存的也是一個內存地址，
只是這個地址是一個函數的起始地址，
下面這個程序打印出一個函數指針的值（func1.c）：

```c
#include <stdio.h>

typedef int (*Func)(int);

int Double(int a)
{
	return (a + a);
}

int main()
{
	Func p = Double;
	printf("%p\n", p);
	return 0;
}
```
編譯、運行程序：

```c
[lqy@localhost notlong]$ gcc -O2 -o func1 func1.c
[lqy@localhost notlong]$ ./func1
0x80483d0
[lqy@localhost notlong]$

`　　`然後我們用 nm 工具查看一下 Double 的地址，
看是不是正好是 0x80483d0：

[lqy@localhost notlong]$ nm func1 | sort
08048294 T _init
08048310 T _start
08048340 t __do_global_dtors_aux
080483a0 t frame_dummy
080483d0 T Double
080483e0 T main
...
```
不出意料，Double 的起始地址果然是 0x080483d0。

## 二、調用函數指針指向的函數

　　直接調用一個函數是 call 一個常量，
而通過函數指針調用一個函數顯然不能這麼做，
因為函數地址是可變的了，指向誰就得 call 誰。
下面比較一下直接調用和通過函數指針間接調用同一個函數的
彙編代碼（func2.c）：

```c
#include <stdio.h>

typedef int (*Func)(int);

int Double(int a)
{
	return (a + a);
}

int main()
{
	Func p = Double;
	Double(2);	// 直接調用
	p(2);		// 間接調用
	return 0;
}
```
部分彙編代碼如下：

```c
movl	$2, (%esp)
call	Double
movl	$2, (%esp)
movl	28(%esp), %eax	# 28(%esp) 是 p
call	*%eax
```
可見通過函數指針間接調用一個函數，
call 指令的操作數不再是一個常量，
而是寄存器 eax（其它寄存器應該也行），
此時 eax 寄存器的值正好是 Double 函數的起始地址，
所以接著就會去執行 Double 函數的指令。

## 三、參數弱匹配

從上面的例子中我們也看到了函數指針也沒什麼特別的，
也就存了個地址，但是調用一個函數不僅需要知道它的起始地址，
還得根據它的參數列表來壓棧傳遞參數。

參數列表在定義函數指針類型的時候就約定好了，
凡是具有相同參數列表的函數都可以賦值給該類型的函數指針，
而參數列表不同的函數也可以通過強制類型轉換後賦值給它
（C語言的指針類型可以任意轉換⊙﹏⊙），
下面這個程序就大膽的強制轉換了一下（func3.c）：

```c
#include <stdio.h>

typedef int (*Func)(int);

int Double2(int a, int b)
{
	return (a + a);
}

int main()
{
	Func p = (Func)Double2;
	printf("%d\n", p(2));
	return 0;
}
```
不強制轉換的話，編譯的時候會報告一個 warring
（居然不是 error ⊙﹏⊙），
上面這個程序編譯的時候 0 error 0 warring，
執行也沒有出錯：

```c
[lqy@localhost notlong]$ gcc -o func3 func3.c
[lqy@localhost notlong]$ ./func3
4
[lqy@localhost notlong]$
```
真算是朵奇葩了！

沒有出錯的原因是：參數 a 對應的剛好是壓棧的 2，
而 b 對應的是一個危險地帶，還好沒用到 b，
所以這個程序依然順利地執行完了。

<b>綜上所述，函數指針真沒什麼特別的。</b>
