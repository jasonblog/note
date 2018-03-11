# 3(magic 2)


在编码的时候，编译器生成什么样的代码，往往我们是不清楚的，但一旦清楚了以后，就能利用这种性质来做一些优化，当然这种优化可能是有限的，但在累积的效应下，这些优化会显得很大，比如每天省1毛钱，省一辈子，也是一笔不小的开销。

我们来用一段简单的代码来说明这个问题，下面这段代码可能太平平无奇了，其中结构体test_1是3个字节，而test_2是4个字节，为了让test_2凑够2的倍数，特别加了一个padding字段。

详细的过程参见代码和附带的注释信息，我们把最后的结论提前，因为代码和分析有点长。

结论如下：

```
1）在结构体大小的选择上，我们尽可能选择2的幂，2，4，8，16

2）在做乘法时，尽可能选择2的幂，这样可以大大加快计算速度。

3）在设定一些变量时，尽可能选择2的幂，512，1024，而不是选择整数例如500，1000，这在计算上都可能带来低效。
```


最后本文介绍了gdb单步调试汇编的一些技巧，如果看寄存器的话，还可以加上display $rsp等命令，以后会有例子用到。

我的实验环境为64位系统，32位也基本类似，希望有兴趣的读者可以自行实验，获得更加真实的体验。


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
        //////////////乘法和加法的关系////////////////////////////


        test_1* p = (test_1*)malloc(10*sizeof(test_1));
        test_2* q = (test_2*)malloc(10*sizeof(test_2));
        printf("sizeof test_1:%d,test_2:%d/n",sizeof(test_1),sizeof(test_2));
        //////////////结构体大小是2的倍数的好处///////////////////
        for(int i=0;i<10;++i)
        {
                p[i].a = '0';
                q[i].a = '1';
        }

        return 0;
}
```

以上程序用g++ -g test.cpp -o test生成可执行程序。我们这里都没有使用编译器优化，有兴趣的读者可以看看

```sh
g++ -O3 -g test.cpp -o test后的代码，很多地方在编译过程中就被略去了。
```
 
可用objdump -d test看汇编代码，我们下面只给出gdp单步调试的过程。

 
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
1: x/i $pc  0x4005d5 <main+29>: shll   0xffffffffffffffd8(%rbp)           //乘2的情况下，只需要一个shll指令就能完成
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
1: x/i $pc  0x4005e1 <main+41>: mov    %eax,0xffffffffffffffdc(%rbp)   //乘3的情况下，相当于2个加法，b'=b+b;b=b+b'
(gdb) 
27              c *= 4;
1: x/i $pc  0x4005e4 <main+44>: shll   $0x2,0xffffffffffffffe0(%rbp)      //乘4的情况下，1个shll命令就能搞定
(gdb) 
28              d *= 50;
1: x/i $pc  0x4005e8 <main+48>: mov    0xffffffffffffffe4(%rbp),%eax
(gdb) 
0x000liang004005eb      28              d *= 50;
1: x/i $pc  0x4005eb <main+51>: imul   $0x32,%eax,%eax                //50不是2的倍数且不能靠有限加法来解决，只好用imul
(gdb) 
0x00000000004005ee      28              d *= 50;
1: x/i $pc  0x4005ee <main+54>: mov    %eax,0xffffffffffffffe4(%rbp)
(gdb) 
32              test_1* p = (test_1*)malloc(10*sizeof(test_1));
1: x/i $pc  0x4005f1 <main+57>: mov    $0x1e,%edi


。。。略去一部分分配和打印sizeof的汇编代码

 

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
0x0000000000400632      38                      p[i].a = '0';                               //test_1的size为3，因此在执行的时候耗费了
1: x/i $pc  0x400632 <main+122>:        movslq %eax,%rdx                       //6条指令
(gdb) 
0x0000000000400635      38                      p[i].a = '0';
1: x/i $pc  0x400635 <main+125>:        mov    %rdx,%rax
(gdb) 
0x0000000000400638      38                      p[i].a = '0';                               //这里计算偏移耗费了2个加法指令
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
39                      q[i].a = '1';                                                                           //test_2的size为4，只需要5条指令
1: x/i $pc  0x400645 <main+141>:        mov    0xfffffffffffffffc(%rbp),%eax
(gdb) 
0x0000000000400648      39                      q[i].a = '1';
1: x/i $pc  0x400648 <main+144>:        cltq   
(gdb) 
0x000000000040064a      39                      q[i].a = '1';
1: x/i $pc  0x40064a <main+146>:        shl    $0x2,%rax                             //这里在计算偏移的时候直接用%rax*4就可算得。
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