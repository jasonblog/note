# 使用valgrind检查cache命中率，提高程序性能


Valgrind为一个debugging 和 profiling的工具包，检查内存问题只是其最知名的一个用途。今天介绍一下，valgrind工具包中的cachegrind。关于cachegrind的具体介绍，请参见valgrind的在线文档http://www.valgrind.org/docs/manual/cg-manual.html


下面使用一个古老的cache示例：

```c
#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

int main(int argc, char **argv)
{
    int array[SIZE][SIZE] = {0};
    int i,j;

#if 1
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            array[i][j] = i + j;
        }
    }
#else
    for (j = 0; j < SIZE; ++j) {
        for (i = 0; i < SIZE; ++i) {
            array[i][j] = i + j;
        }
    }
#endif

    return 0;
}
```

这个示例代码从很久就开始用于说明利用局部性来增加cache的命中率。传统的答案是第一个for循环的性能要优于第二个循环。
我使用条件编译，在没有打开任何优化开关的条件下，第一种情况生成文件为test1，第二种情况生成文件为test2。
下面是输出

<ol style="margin-left: 0px; padding-top: 5px; padding-right: 0pt; padding-bottom: 5px; padding-left: 0pt; " start="1" class="dp-css"><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test1</li><li>==2079== Cachegrind, a cache and branch-prediction profiler</li><li>==2079== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2079== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2079== Command: ./test1</li><li>==2079==</li><li>==2079==</li><li>==2079== I   refs:      219,767</li><li>==2079== I1  misses:        614</li><li>==2079== L2i misses:        608</li><li>==2079== I1  miss rate:    0.27%</li><li>==2079== L2i miss rate:    0.27%</li><li>==2079==</li><li>==2079== D   refs:      124,402  (95,613 rd   + 28,789 wr)</li><li><font color="#f00000">==2079== D1  misses:      2,041  (   621 rd   +  1,420 wr)</font></li><li>==2079== L2d misses:      1,292  (   537 rd   +    755 wr)</li><li>==2079== D1  miss rate:     1.6% (   0.6%     +    4.9%  )</li><li>==2079== L2d miss rate:     1.0% (   0.5%     +    2.6%  )</li><li>==2079==</li><li>==2079== L2 refs:         2,655  ( 1,235 rd   +  1,420 wr)</li><li>==2079== L2 misses:       1,900  ( 1,145 rd   +    755 wr)</li><li>==2079== L2 miss rate:      0.5% (   0.3%     +    2.6%  )</li><li><br></li><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test2</li><li>==2080== Cachegrind, a cache and branch-prediction profiler</li><li>==2080== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2080== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2080== Command: ./test2</li><li>==2080==</li><li>==2080==</li><li>==2080== I   refs:      219,767</li><li>==2080== I1  misses:        614</li><li>==2080== L2i misses:        608</li><li>==2080== I1  miss rate:    0.27%</li><li>==2080== L2i miss rate:    0.27%</li><li>==2080==</li><li>==2080== D   refs:      124,402  (95,613 rd   + 28,789 wr)</li><li><font color="#f00000">==2080== D1  misses:      1,788  (   621 rd   +  1,167 wr)</font></li><li>==2080== L2d misses:      1,292  (   537 rd   +    755 wr)</li><li>==2080== D1  miss rate:     1.4% (   0.6%     +    4.0%  )</li><li>==2080== L2d miss rate:     1.0% (   0.5%     +    2.6%  )</li><li>==2080==</li><li>==2080== L2 refs:         2,402  ( 1,235 rd   +  1,167 wr)</li><li>==2080== L2 misses:       1,900  ( 1,145 rd   +    755 wr)</li><li>==2080== L2 miss rate:      0.5% (   0.3%     +    2.6%  )</li></ol>



结果有点出人意料，第一种情况在D1的命中率反而低于第二种情况。

这个结果其实是应该可以理解的。
1. 现在的CPU的cache是以line为单位的。这样，当数组的size不大时，第二种情况的循环，虽然没有使用局部性原则，但是并不会因此降低cache的命中率，并且可能可以迅速的将数据填到cache中
2. 现在的CPU的cache空间较大。这样，当数组的size不大时，即使没有使用局部性原则，也不会导致cache的频繁更新。
由于我对cache的理解，也比较粗浅，所以不能明确的指出这个结果的根本原因。根据上面的两个条件，基本上也可以理解为什么第二种情况更快。

为了使cachegrind的结果与传统的答案一样，我们就需要破坏上面两个条件。那么，现在将SIZE从100增大的1000。再次看一下输出结果：

<ol style="margin-left: 0px; padding-top: 5px; padding-right: 0pt; padding-bottom: 5px; padding-left: 0pt; " start="1" class="dp-css"><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test1</li><li>==2094== Cachegrind, a cache and branch-prediction profiler</li><li>==2094== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2094== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2094== Command: ./test1</li><li>==2094==</li><li>==2094==</li><li>==2094== I   refs:      11,519,463</li><li>==2094== I1  misses:           617</li><li>==2094== L2i misses:           611</li><li>==2094== I1  miss rate:       0.00%</li><li>==2094== L2i miss rate:       0.00%</li><li>==2094==</li><li>==2094== D   refs:       7,305,498  (6,038,310 rd   + 1,267,188 wr)</li><li><font color="#f00000">==2094== D1  misses:       125,791  (      621 rd   +   125,170 wr)</font></li><li>==2094== L2d misses:       125,763  (      595 rd   +   125,168 wr)</li><li><font color="#f00000">==2094== D1  miss rate:        1.7% (      0.0%     +       9.8%  )</font></li><li>==2094== L2d miss rate:        1.7% (      0.0%     +       9.8%  )</li><li>==2094==</li><li>==2094== L2 refs:          126,408  (    1,238 rd   +   125,170 wr)</li><li>==2094== L2 misses:        126,374  (    1,206 rd   +   125,168 wr)</li><li>==2094== L2 miss rate:         0.6% (      0.0%     +       9.8%  )</li><li><br></li><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test2</li><li>==2095== Cachegrind, a cache and branch-prediction profiler</li><li>==2095== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2095== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2095== Command: ./test2</li><li>==2095==</li><li>==2095==</li><li>==2095== I   refs:      11,519,463</li><li>==2095== I1  misses:           617</li><li>==2095== L2i misses:           611</li><li>==2095== I1  miss rate:       0.00%</li><li>==2095== L2i miss rate:       0.00%</li><li>==2095==</li><li>==2095== D   refs:       7,305,498  (6,038,310 rd   + 1,267,188 wr)</li><li><font color="#f00000">==2095== D1  misses:     1,063,300  (      621 rd   + 1,062,679 wr)</font></li><li>==2095== L2d misses:       116,261  (      595 rd   +   115,666 wr)</li><li><font color="#f00000">==2095== D1  miss rate:       14.5% (      0.0%     +      83.8%  )</font></li><li>==2095== L2d miss rate:        1.5% (      0.0%     +       9.1%  )</li><li>==2095==</li><li>==2095== L2 refs:        1,063,917  (    1,238 rd   + 1,062,679 wr)</li><li>==2095== L2 misses:        116,872  (    1,206 rd   +   115,666 wr)</li><li>==2095== L2 miss rate:         0.6% (      0.0%     +       9.1%  )</li></ol>


对比红色的两行，第一种情况的miss率为1.7%，而第二种情况的miss率高达14.5%。现在符合了传统答案。

总结一下：
1. 我们可以使用cachegrind来检查cache的命中率，提高程序性能；
2. 尽信书不如无书。书中的一些结果面对现在的环境，很可能是错误的。毕竟IT技术更新太快。还是自己动手实践一下更好！


注：Valgrind对于cache的测量，只是一种`模拟`。但是按照valgrind的文档，结果的可靠性还是有保证的。
