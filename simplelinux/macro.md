[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">奇怪的宏
</h1>

　　這一篇介紹這些奇怪的宏：

## 一、do while(0)

　　為了交換兩個整型變量的值，前面<em>值傳遞</em>中已經用
包含指針參數的 swap 函數做到了，這次用<b>宏</b>來實現（swap.c）：

	#include <stdio.h>
	
	#define SWAP(a,b)		\
		do{					\
			int t = a;		\
			a = b;			\
			b = t;			\
		}while(0)
	
	int main()
	{
		int c=1, d=2;
		int t;	// 測試 SWAP 與環境的兼容性
		
		SWAP(c,d);
		
		printf("c:%d d:%d\n", c, d);
		return 0;
	}

`　　`這個宏看起來就有點怪了：do while(0) 是寫了個循環
又不讓它循環，蛋疼啊！其實不然，這樣寫是有妙用的：

　　<b>首先</b>，SWAP 有多條語句，如果這樣寫：

	#define SWAP(a,b)		\
			int t = a;		\
			a = b;			\
			b = t;

`　　`那麼用的時候就得這麼用：

	SWAP(c,d)

`　　`<b>不能加分號</b>！不習慣吧？

　　<b>其次</b>，使用 do{...}while(0)，
中間的語句用大括號括起來了，所以是另一個命名空間，
<b>其中的新變量 t 不會發生命名衝突</b>。

　　SWAP 宏要比之前那個函數的效率要高，
因為沒有發生函數調用，沒有參數傳遞，
宏會在編譯前被替換，所以只是嵌入了一小段代碼。

## 二、&#35;

　　標題我沒打錯，這裡要說的就是井號，#的功能是將其後面的
宏參數進行字符串化操作。比如下面代碼中的宏： 

	#define WARN_IF(EXP) \
	do{ if (EXP) \
		fprintf(stderr, "Warning: " #EXP "\n"); } \
	while(0) 

`　　`那麼實際使用中會出現下面所示的替換過程： 

	WARN_IF (divider == 0); 


`　　`被替換為 

	do { if (divider == 0) 
		fprintf(stderr, "Warning: " "divider == 0" "\n"); 
	} while(0); 

`　　`需要注意的是<b>C語言中多個雙引號字符串放在一起
會自動連接起來</b>，所以如果 divider 為 0 的話，就會打印出：

	Warning: divider == 0

## 三、&#35;&#35;

　　# 還是比較少用的，## 卻比較流行，
在 linux0.01 中就用到過。## 被稱為連接符，
用來將兩個 記號（編譯原理中的詞彙） 連接為一個 記號。
看下面的例子吧（add.c）：

	#include <stdio.h>
	
	#define add(Type)				\
	Type add##Type(Type a, Type b){	\
		return a+b;					\
	}
	
	// 下面兩條是奇蹟發生的地方
	add(int)
	add(double)
	
	int main()
	{
		int a = addint(1, 2);
		double d = adddouble(1.5, 1.5);
		
		printf("a:%d d:%lf\n", a, d);
		return 0;
	}

`　　`那兩行被替換後是這個樣子的：

	int addint(int a, int b){ return a+b; }
	double adddouble(double a, double b){ return a+b; }

<b>以上內容都可以使用<em>照妖鏡</em>看到宏被替換後的情形</b>。

[回目錄][content]
