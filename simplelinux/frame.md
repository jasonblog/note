[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">函數幀
</h1>

　　這標題一念出來我立刻想到了一個名人：白素貞……當然，
此女與本文無關，下面進入正題：

<pre>其實程序運行就好比一幀一幀地放電影，每一幀是一次函數調用，電影放完了，我們就看到結局了。</pre>

　　我們用一個遞歸求解階乘的程序來看看這個放映過程（fac.c）：

	#include <stdio.h>
	
	int fac(int n)
	{
		if(n <= 1)
			return 1;
		return n * fac(n-1);
	}
	
	int main()
	{
		int n = 3;
		int ans = fac(n);
		
		printf("%d! = %d\n", n, ans);
		return 0;
	}

## main 幀

　　首先 main 函數被調用（程序可不是從 main 開始執行的）：

<table>
<tr><td>
<pre><code>main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$3, 28(%esp)	# n = 3
	movl	28(%esp), %eax
	movl	%eax, (%esp)
	call	fac
	movl	%eax, 24(%esp)	# 返回值存入 ans
	movl	$.LC0, %eax
	movl	24(%esp), %edx
	movl	%edx, 8(%esp)
	movl	28(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	$0, %eax
	leave
	ret
</code></pre></td>
<td><img src="http://fmn.rrimg.com/fmn056/20121124/1940/original_D0zG_726b00003e04118c.jpg" /></td>
</tr></table>

`　　`main 函數創建了一幀：

* 從 esp 到 ebp + 4
* 上邊是本次調用的返回地址、舊的 ebp 指針
* 然後是 main 的局部變量 n、ans
* 最下邊是參數的空間，右上圖顯示的是 main 中調用 printf 
前的棧的使用情況

`　　`進入 main 函數，前 4 條指令開闢了這片空間，
在退出 main 函數之前的 leave ret 回收了這片空間
（<b>C++ 在回收這片空間之前要析構此函數中的所有局部對象</b>）。
<b>在 main 函數執行期間 ebp 一直指向 幀頂 - 4 的位置，
ebp 被稱為幀指針也就是這個原因</b>。

## 調用慣例

　　調用函數的時候，先傳參數，然後 call，
具體這個過程怎麼實現有相關規定，這樣的規定被稱為<b>調用慣例</b>，
C語言中有多種調用慣例，它們的不同之處在於：

1. 參數是壓棧還是存入寄存器
2. 參數壓棧的次序（從右至左 | 從左至右）
3. 調用完成後是調用者還是被調用者來恢復棧

`　　`各種調用慣例<em>《程序員的自我修養》——鏈接、裝載與庫</em>
這本書中有簡要介紹，我照抄後在本文後面列出。C語言默認的
調用慣例是 cdecl：

1. 參數從右至左壓棧
2. 調用完成後調用者負責恢復棧

`　　`可以從 printf("%d! = %d\n", n, ans); 的調用過程
中看出。

　　雖然 VC、gcc 都默認使用 cdecl 調用慣例，
但它們的實現卻各有風格：

* VC 一般是從右至左 push 參數，call，add esp, XXX
* 而 gcc 在給局部變量分配空間的時候也給參數分配了足夠的空間，
所以只要從右至左 mov 參數, XXX(%esp)，call 就可以了，
調用者根本不用去恢復棧，因為傳參數的時候並沒有修改棧指針 esp。

## fac 幀

　　說完調用慣例我們接著來看第一次調用 fac：

<table>
<tr><td>
<pre><code>fac:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	cmpl	$1, 8(%ebp)
	jg	.L2			# n > 1 就跳到 .L2
	movl	$1, %eax
	jmp	.L3			# 無條件跳到 .L3
.L2:
	movl	8(%ebp), %eax
	subl	$1, %eax
	movl	%eax, (%esp)
	call	fac		#  fac(n-1)
	imull	8(%ebp), %eax	# eax = n * eax
.L3:
	leave
	ret
</code></pre></td>
<td><img src="http://fmn.rrfmn.com/fmn058/20121124/1940/original_XcDN_08c400005ab9125d.jpg" /></td>
</tr></table>

　　fac(3) 開闢了第一個 fac 幀：

* 從 esp 到 ebp + 4（fac 還能"越界"地讀到參數 n）
* 上邊是 返回地址、舊的 ebp 指針（指向 main 幀）
* fac 沒有局部變量，又浪費了很多字節
* 參數佔了最下邊的 4 字節（需要遞歸時使用）

`　　`這時還不滿足遞歸終止條件，於是fac(3)又遞歸地調用了fac(2)，
fac(2)又遞歸的調用了fac(1)，到這個時候棧變成了如下情況：

![total](http://fmn.rrimg.com/fmn062/20121124/1940/original_y9zg_1a3800005b5a118e.jpg)

　　上圖的箭頭的含義很明顯：
<b>從 ebp 可回溯到所有的函數幀</b>，
這是由於每個函數開頭都來兩條 pushl %ebp、movl %esp, %ebp造成的。

　　參數總是調用者寫入，被調用者來讀取（被調用者修改參數毫無意義），
這是一種默契^_^。

程序繼續運行：

1. fac(1) 滿足了遞歸終止條件，fac(1) 返回 1，fac(1)#3 幀消亡
2. 繼續執行 fac(2)，fac(2) 返回 1\*2，fac(2)#2 幀消亡
3. 繼續執行 fac(3)，fac(3) 返回 2\*3，fac(1)#1 幀消亡
4. 繼續執行 main，printf 結果，返回 0，main 幀消亡
5. 繼續執行 ？？？（且聽下回分解）

最終程序結束（進程僵死，一會兒後操作系統會來收屍
（回收內存及其他資源））。

## 小結

　　函數幀保存的是函數的一個完整的局部環境，
保證了函數調用的正確返回（函數幀中有返回地址）、
返回後繼續正確地執行，因此函數幀是 C語言 能調來調去的保障。

<h2 align="center">主要的調用慣例</h2>
<table border="1">
 <tr>
  <th>調用慣例</th>
  <th>出棧方</th>
  <th>參數傳遞</th>
  <th>名字修飾</th>
 </tr>
 <tr>
  <td>cdecl</td>
  <td>函數調用方</td>
  <td>從右至左的順序壓參數入棧</td>
  <td>下劃線+函數名</td>
 </tr>
 <tr>
  <td>stdcall</td>
  <td>函數本身</td>
  <td>從右至左的順序壓參數入棧</td>
  <td>下劃線+函數名+@+參數的字節數，
  如函數 int func(int a, double b)的修飾名是
  _func@12</td>
 </tr>
  <tr>
  <td>fastcall</td>
  <td>函數本身</td>
  <td>頭兩個 DWORD(4字節)類型或者更少字節的參數
  被放入寄存器，其他剩下的參數按從右至左的順序入棧</td>
  <td>@+函數名+@+參數的字節數</td>
 </tr>
 <tr>
  <td>pascal</td>
  <td>函數本身</td>
  <td>從左至右的順序入棧</td>
  <td>較為複雜，參見pascal文檔</td>
 </tr>
 </table>

[回目錄][content]
