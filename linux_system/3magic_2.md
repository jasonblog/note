# 3(magic 2)


在編碼的時候，編譯器生成什麼樣的代碼，往往我們是不清楚的，但一旦清楚了以後，就能利用這種性質來做一些優化，當然這種優化可能是有限的，但在累積的效應下，這些優化會顯得很大，比如每天省1毛錢，省一輩子，也是一筆不小的開銷。

我們來用一段簡單的代碼來說明這個問題，下面這段代碼可能太平平無奇了，其中結構體test_1是3個字節，而test_2是4個字節，為了讓test_2湊夠2的倍數，特別加了一個padding字段。

詳細的過程參見代碼和附帶的註釋信息，我們把最後的結論提前，因為代碼和分析有點長。

結論如下：

```
1）在結構體大小的選擇上，我們儘可能選擇2的冪，2，4，8，16

2）在做乘法時，儘可能選擇2的冪，這樣可以大大加快計算速度。

3）在設定一些變量時，儘可能選擇2的冪，512，1024，而不是選擇整數例如500，1000，這在計算上都可能帶來低效。
```


最後本文介紹了gdb單步調試彙編的一些技巧，如果看寄存器的話，還可以加上display $rsp等命令，以後會有例子用到。

我的實驗環境為64位系統，32位也基本類似，希望有興趣的讀者可以自行實驗，獲得更加真實的體驗。


```c
#include <stdlib.h>
#include <stdio.h>

struct test_1
{
        char a;
        char b;
        char c;
};

struct test_2
{
        char a;
        char b;
        char c;
        char padding;
};

int main(void)
{
        int a,b,c,d;
        a = 0;
        b = 1;
        c = 2;
        a *= 2;
        b *= 3;
        c *= 4;
        d *= 50;
        //////////////乘法和加法的關係////////////////////////////


        test_1* p = (test_1*)malloc(10*sizeof(test_1));
        test_2* q = (test_2*)malloc(10*sizeof(test_2));
        printf("sizeof test_1:%d,test_2:%d/n",sizeof(test_1),sizeof(test_2));
        //////////////結構體大小是2的倍數的好處///////////////////
        for(int i=0;i<10;++i)
        {
                p[i].a = '0';
                q[i].a = '1';
        }

        return 0;
}
```

以上程序用g++ -g test.cpp -o test生成可執行程序。我們這裡都沒有使用編譯器優化，有興趣的讀者可以看看

```sh
g++ -O3 -g test.cpp -o test後的代碼，很多地方在編譯過程中就被略去了。
```
 
可用objdump -d test看彙編代碼，我們下面只給出gdp單步調試的過程。

 
```sh

(gdb) b main
Breakpoint 1 at 0x4005c0: file test.cpp, line 22.
(gdb) r
Starting program: /opt/intel/vtune/test/esp/test

Breakpoint 1, main () at test.cpp:22
22              a = 0;
(gdb) display /i $pc
1: x/i $pc  0x4005c0 <main+8>:  movl   $0x0,0xffffffffffffffd8(%rbp)
(gdb) ni
23              b = 1;
1: x/i $pc  0x4005c7 <main+15>: movl   $0x1,0xffffffffffffffdc(%rbp)
(gdb) 
24              c = 2;
1: x/i $pc  0x4005ce <main+22>: movl   $0x2,0xffffffffffffffe0(%rbp)
(gdb) 
25              a *= 2;
1: x/i $pc  0x4005d5 <main+29>: shll   0xffffffffffffffd8(%rbp)           //乘2的情況下，只需要一個shll指令就能完成
(gdb) 
26              b *= 3;
1: x/i $pc  0x4005d8 <main+32>: mov    0xffffffffffffffdc(%rbp),%edx
(gdb) 
0x00000000004005db      26              b *= 3;
1: x/i $pc  0x4005db <main+35>: mov    %edx,%eax
(gdb) 
0x00000000004005dd      26              b *= 3;
1: x/i $pc  0x4005dd <main+37>: add    %eax,%eax
(gdb) 
0x0000penny04005df      26              b *= 3;
1: x/i $pc  0x4005df <main+39>: add    %edx,%eax
(gdb) 
0x00000000004005e1      26              b *= 3;
1: x/i $pc  0x4005e1 <main+41>: mov    %eax,0xffffffffffffffdc(%rbp)   //乘3的情況下，相當於2個加法，b'=b+b;b=b+b'
(gdb) 
27              c *= 4;
1: x/i $pc  0x4005e4 <main+44>: shll   $0x2,0xffffffffffffffe0(%rbp)      //乘4的情況下，1個shll命令就能搞定
(gdb) 
28              d *= 50;
1: x/i $pc  0x4005e8 <main+48>: mov    0xffffffffffffffe4(%rbp),%eax
(gdb) 
0x000liang004005eb      28              d *= 50;
1: x/i $pc  0x4005eb <main+51>: imul   $0x32,%eax,%eax                //50不是2的倍數且不能靠有限加法來解決，只好用imul
(gdb) 
0x00000000004005ee      28              d *= 50;
1: x/i $pc  0x4005ee <main+54>: mov    %eax,0xffffffffffffffe4(%rbp)
(gdb) 
32              test_1* p = (test_1*)malloc(10*sizeof(test_1));
1: x/i $pc  0x4005f1 <main+57>: mov    $0x1e,%edi


。。。略去一部分分配和打印sizeof的彙編代碼

 

(gdb) 
0x000000000040062d      36              for(int i=0;i<10;++i)
1: x/i $pc  0x40062d <main+117>:        jmp    0x400659 <main+161>
(gdb) 
0x0000000000400659      36              for(int i=0;i<10;++i)
1: x/i $pc  0x400659 <main+161>:        cmpl   $0x9,0xfffffffffffffffc(%rbp)
(gdb) 
0x000000000040065d      36              for(int i=0;i<10;++i)
1: x/i $pc  0x40065d <main+165>:        jle    0x40062f <main+119>
(gdb) 
38                      p[i].a = '0';
1: x/i $pc  0x40062f <main+119>:        mov    0xfffffffffffffffc(%rbp),%eax
(gdb) 
0x0000000000400632      38                      p[i].a = '0';                               //test_1的size為3，因此在執行的時候耗費了
1: x/i $pc  0x400632 <main+122>:        movslq %eax,%rdx                       //6條指令
(gdb) 
0x0000000000400635      38                      p[i].a = '0';
1: x/i $pc  0x400635 <main+125>:        mov    %rdx,%rax
(gdb) 
0x0000000000400638      38                      p[i].a = '0';                               //這裡計算偏移耗費了2個加法指令
1: x/i $pc  0x400638 <main+128>:        add    %rax,%rax
(gdb) 
0x000000000040063b      38                      p[i].a = '0';
1: x/i $pc  0x40063b <main+131>:        add    %rdx,%rax
(gdb) 
0x000000000040063e      38                      p[i].a = '0';
1: x/i $pc  0x40063e <main+134>:        add    0xffffffffffffffe8(%rbp),%rax
(gdb) 
0x0000000000400642      38                      p[i].a = '0';
1: x/i $pc  0x400642 <main+138>:        movb   $0x30,(%rax)
(gdb) 
39                      q[i].a = '1';                                                                           //test_2的size為4，只需要5條指令
1: x/i $pc  0x400645 <main+141>:        mov    0xfffffffffffffffc(%rbp),%eax
(gdb) 
0x0000000000400648      39                      q[i].a = '1';
1: x/i $pc  0x400648 <main+144>:        cltq   
(gdb) 
0x000000000040064a      39                      q[i].a = '1';
1: x/i $pc  0x40064a <main+146>:        shl    $0x2,%rax                             //這裡在計算偏移的時候直接用%rax*4就可算得。
(gdb) 
0x000000000040064e      39                      q[i].a = '1';
1: x/i $pc  0x40064e <main+150>:        add    0xfffffffffffffff0(%rbp),%rax
(gdb) 
0xpennyliang0400652      39                      q[i].a = '1';
1: x/i $pc  0x400652 <main+154>:        movb   $0x31,(%rax)
(gdb) 
36              for(int i=0;i<10;++i)
1: x/i $pc  0x400655 <main+157>:        addl   $0x1,0xfffffffffffffffc(%rbp)

```