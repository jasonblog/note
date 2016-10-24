# 64位x86的函數調用棧佈局


在看本文之前，如果不瞭解x86的32位機的函數佈局的話，建議先閱讀一下前一篇文章《如何手工展開函數棧定位問題》—— http://blog.chinaunix.net/space.php?uid=23629988&do=blog&id=3029639

為啥還要就64位的情況單開一篇文章呢，難道64位與32位不一樣嗎？


還是先看測試代碼：

```c
#include <stdlib.h>
#include <stdio.h>


static void test(void *p1, void *p2, int p3)
{
    p1 = p1;
    p2 = p2;
    p3 = p3;
}

int main()
{
    void *p1 = (void*)1;
    void *p2 = (void*)2;
    int p3 = 3;

    test(p1, p2, p3);

    return 0;
}
```


編譯gcc -g -Wall test.c，調試進入test

```sh
(gdb) bt
#0 test (p1=0x1, p2=0x2, p3=3) at test.c:10
#1 0x0000000000400488 in main () at test.c:18
```

那麼檢查棧的內容

```sh
(gdb) x /16xg 0x7fffab620d00
0x7fffab620d00: 0x00007fffab620d30 0x0000000000400488
0x7fffab620d10: 0x00000000004004a0 0x0000000000000002
0x7fffab620d20: 0x0000000000000001 0x0000000300000000
0x7fffab620d30: 0x0000000000000000 0x00007f93bbaa11c4
0x7fffab620d40: 0x0000000000400390 0x00007fffab620e18
0x7fffab620d50: 0x0000000100000000 0x0000000000400459
0x7fffab620d60: 0x00007f93bc002c00 0x85b4aff07d2e87c7
0x7fffab620d70: 0x0000000000000000 0x00007fffab620e10
```

開始分析棧的內容：

```sh
1. 0x00007fffab620d30：為test調用者main的BP內容，沒有問題；
2. 0x0000000000400488：為test的返回地址，與前面的bt輸出相符，沒有問題；
3. 0x00000000004004a0：——這個是什麼東東？？！！
4. 0x0000000000000002， 0x0000000000000001， 0x0000000300000000：這裡也有不少疑問啊？！
1. 這個0x00000003是第3個參數？因為是整數所以在64位的機器上，只使用棧的一個單元的一半空間？
2. 參數的順序為什麼是3,1,2呢？難道是因為前兩個參數為指針，第三個參數為int有關？
```

我在工作中遇到了類似的問題，所以才特意寫了上面的測試代碼，就為了測試相同參數原型的函數調用棧的問題。看到這裡，感覺很奇怪，對於上面兩個問題很困惑啊。上網也沒有找到64位的x86函數調用棧的特別的資料。


難道64位機與32位機有這麼大的不同？！大家先想一下，答案馬上揭曉。

當遇到疑難雜症時，彙編則是王道：


<div id="codeText" class="codeText" style="width: 645px; "><ol start="1" class="dp-css"><li>(gdb) disassemble main</li><li>Dump of assembler code for function main:</li><li>0x0000000000400459 <main+0>: push %rbp</main+0></li><li>0x000000000040045a <main+1>: mov %rsp,%rbp</main+1></li><li><font class="Apple-style-span" color="#0000f0">0x000000000040045d <main+4>: sub $0x20,%rsp</main+4></font></li><li><font class="Apple-style-span" color="#0000f0">0x0000000000400461 <main+8>: movq $0x1,-0x10(%rbp)</main+8></font></li><li><font class="Apple-style-span" color="#0000f0">0x0000000000400469 <main+16>: movq $0x2,-0x18(%rbp)</main+16></font></li><li><font class="Apple-style-span" color="#0000f0">0x0000000000400471 <main+24>: movl $0x3,-0x4(%rbp)</main+24></font></li><li><font class="Apple-style-span" color="#f00000">0x0000000000400478 <main+31>: mov -0x4(%rbp),%edx</main+31></font></li><li><font class="Apple-style-span" color="#f00000">0x000000000040047b <main+34>: mov -0x18(%rbp),%rsi</main+34></font></li><li><font class="Apple-style-span" color="#f00000">0x000000000040047f <main+38>: mov -0x10(%rbp),%rdi</main+38></font></li><li><font class="Apple-style-span" color="#f00000">0x0000000000400483 <main+42>: callq 0x400448 <test></test></main+42></font></li><li>0x0000000000400488 <main+47>: mov $0x0,%eax</main+47></li><li>0x000000000040048d <main+52>: leaveq</main+52></li><li>0x000000000040048e <main+53>: retq</main+53></li><li>End of assembler dump.</li></ol></div>

看紅色部分的彙編代碼，為調用test時的處理，原來64位機器上，調用test時，根本沒有對參數進行壓棧，所以上面對於棧內容的分析有誤。後面的內存中存放的根本不是test的參數。看到彙編代碼，我突然想起，由於64位cpu的寄存器比32位cpu的寄存器要多，所以gcc會盡量使用寄存器來傳遞參數來提高效率。

讓我們重新運行程序，再次在test下查看寄存器內容：

<ol start="1" class="dp-css"><li>(gdb) info registers</li><li>rax 0x7f141fea1a60 139724411509344</li><li>rbx 0x7f14200c2c00 139724413742080</li><li>rcx 0x4004a0 4195488</li><li><font class="Apple-style-span" color="#f00000">rdx 0x3 3</font></li><li><font class="Apple-style-span" color="#f00000">rsi 0x2 2</font></li><li><font class="Apple-style-span" color="#f00000">rdi 0x1 1</font></li><li>rbp 0x7fff9c08d380 0x7fff9c08d380</li><li>rsp 0x7fff9c08d380 0x7fff9c08d380</li></ol>


與大家分享一下64位機器上調試時需要注意的一個問題：函數調用時，編譯器會盡量使用寄存器來傳遞參數，這點與32位機有很大不同。在我們的調試中，要特別注意這點。


注：關於壓棧順序，參數的傳遞方式等等，都可以通過編譯選項來指定或者禁止的。本文的情況為GCC的默認行為。

