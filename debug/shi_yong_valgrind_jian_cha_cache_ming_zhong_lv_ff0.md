# 使用valgrind檢查cache命中率，提高程序性能


Valgrind為一個debugging 和 profiling的工具包，檢查內存問題只是其最知名的一個用途。今天介紹一下，valgrind工具包中的cachegrind。關於cachegrind的具體介紹，請參見valgrind的在線文檔http://www.valgrind.org/docs/manual/cg-manual.html


下面使用一個古老的cache示例：

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

這個示例代碼從很久就開始用於說明利用局部性來增加cache的命中率。傳統的答案是第一個for循環的性能要優於第二個循環。
我使用條件編譯，在沒有打開任何優化開關的條件下，第一種情況生成文件為test1，第二種情況生成文件為test2。
下面是輸出

<ol style="margin-left: 0px; padding-top: 5px; padding-right: 0pt; padding-bottom: 5px; padding-left: 0pt; " start="1" class="dp-css"><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test1</li><li>==2079== Cachegrind, a cache and branch-prediction profiler</li><li>==2079== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2079== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2079== Command: ./test1</li><li>==2079==</li><li>==2079==</li><li>==2079== I   refs:      219,767</li><li>==2079== I1  misses:        614</li><li>==2079== L2i misses:        608</li><li>==2079== I1  miss rate:    0.27%</li><li>==2079== L2i miss rate:    0.27%</li><li>==2079==</li><li>==2079== D   refs:      124,402  (95,613 rd   + 28,789 wr)</li><li><font color="#f00000">==2079== D1  misses:      2,041  (   621 rd   +  1,420 wr)</font></li><li>==2079== L2d misses:      1,292  (   537 rd   +    755 wr)</li><li>==2079== D1  miss rate:     1.6% (   0.6%     +    4.9%  )</li><li>==2079== L2d miss rate:     1.0% (   0.5%     +    2.6%  )</li><li>==2079==</li><li>==2079== L2 refs:         2,655  ( 1,235 rd   +  1,420 wr)</li><li>==2079== L2 misses:       1,900  ( 1,145 rd   +    755 wr)</li><li>==2079== L2 miss rate:      0.5% (   0.3%     +    2.6%  )</li><li><br></li><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test2</li><li>==2080== Cachegrind, a cache and branch-prediction profiler</li><li>==2080== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2080== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2080== Command: ./test2</li><li>==2080==</li><li>==2080==</li><li>==2080== I   refs:      219,767</li><li>==2080== I1  misses:        614</li><li>==2080== L2i misses:        608</li><li>==2080== I1  miss rate:    0.27%</li><li>==2080== L2i miss rate:    0.27%</li><li>==2080==</li><li>==2080== D   refs:      124,402  (95,613 rd   + 28,789 wr)</li><li><font color="#f00000">==2080== D1  misses:      1,788  (   621 rd   +  1,167 wr)</font></li><li>==2080== L2d misses:      1,292  (   537 rd   +    755 wr)</li><li>==2080== D1  miss rate:     1.4% (   0.6%     +    4.0%  )</li><li>==2080== L2d miss rate:     1.0% (   0.5%     +    2.6%  )</li><li>==2080==</li><li>==2080== L2 refs:         2,402  ( 1,235 rd   +  1,167 wr)</li><li>==2080== L2 misses:       1,900  ( 1,145 rd   +    755 wr)</li><li>==2080== L2 miss rate:      0.5% (   0.3%     +    2.6%  )</li></ol>



結果有點出人意料，第一種情況在D1的命中率反而低於第二種情況。

這個結果其實是應該可以理解的。
1. 現在的CPU的cache是以line為單位的。這樣，當數組的size不大時，第二種情況的循環，雖然沒有使用局部性原則，但是並不會因此降低cache的命中率，並且可能可以迅速的將數據填到cache中
2. 現在的CPU的cache空間較大。這樣，當數組的size不大時，即使沒有使用局部性原則，也不會導致cache的頻繁更新。
由於我對cache的理解，也比較粗淺，所以不能明確的指出這個結果的根本原因。根據上面的兩個條件，基本上也可以理解為什麼第二種情況更快。

為了使cachegrind的結果與傳統的答案一樣，我們就需要破壞上面兩個條件。那麼，現在將SIZE從100增大的1000。再次看一下輸出結果：

<ol style="margin-left: 0px; padding-top: 5px; padding-right: 0pt; padding-bottom: 5px; padding-left: 0pt; " start="1" class="dp-css"><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test1</li><li>==2094== Cachegrind, a cache and branch-prediction profiler</li><li>==2094== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2094== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2094== Command: ./test1</li><li>==2094==</li><li>==2094==</li><li>==2094== I   refs:      11,519,463</li><li>==2094== I1  misses:           617</li><li>==2094== L2i misses:           611</li><li>==2094== I1  miss rate:       0.00%</li><li>==2094== L2i miss rate:       0.00%</li><li>==2094==</li><li>==2094== D   refs:       7,305,498  (6,038,310 rd   + 1,267,188 wr)</li><li><font color="#f00000">==2094== D1  misses:       125,791  (      621 rd   +   125,170 wr)</font></li><li>==2094== L2d misses:       125,763  (      595 rd   +   125,168 wr)</li><li><font color="#f00000">==2094== D1  miss rate:        1.7% (      0.0%     +       9.8%  )</font></li><li>==2094== L2d miss rate:        1.7% (      0.0%     +       9.8%  )</li><li>==2094==</li><li>==2094== L2 refs:          126,408  (    1,238 rd   +   125,170 wr)</li><li>==2094== L2 misses:        126,374  (    1,206 rd   +   125,168 wr)</li><li>==2094== L2 miss rate:         0.6% (      0.0%     +       9.8%  )</li><li><br></li><li>[fgao@fgao-vm-fc13 test]$ valgrind --tool=cachegrind ./test2</li><li>==2095== Cachegrind, a cache and branch-prediction profiler</li><li>==2095== Copyright (C) 2002-2009, and GNU GPL'd, by Nicholas Nethercote et al.</li><li>==2095== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>==2095== Command: ./test2</li><li>==2095==</li><li>==2095==</li><li>==2095== I   refs:      11,519,463</li><li>==2095== I1  misses:           617</li><li>==2095== L2i misses:           611</li><li>==2095== I1  miss rate:       0.00%</li><li>==2095== L2i miss rate:       0.00%</li><li>==2095==</li><li>==2095== D   refs:       7,305,498  (6,038,310 rd   + 1,267,188 wr)</li><li><font color="#f00000">==2095== D1  misses:     1,063,300  (      621 rd   + 1,062,679 wr)</font></li><li>==2095== L2d misses:       116,261  (      595 rd   +   115,666 wr)</li><li><font color="#f00000">==2095== D1  miss rate:       14.5% (      0.0%     +      83.8%  )</font></li><li>==2095== L2d miss rate:        1.5% (      0.0%     +       9.1%  )</li><li>==2095==</li><li>==2095== L2 refs:        1,063,917  (    1,238 rd   + 1,062,679 wr)</li><li>==2095== L2 misses:        116,872  (    1,206 rd   +   115,666 wr)</li><li>==2095== L2 miss rate:         0.6% (      0.0%     +       9.1%  )</li></ol>


對比紅色的兩行，第一種情況的miss率為1.7%，而第二種情況的miss率高達14.5%。現在符合了傳統答案。

總結一下：
1. 我們可以使用cachegrind來檢查cache的命中率，提高程序性能；
2. 盡信書不如無書。書中的一些結果面對現在的環境，很可能是錯誤的。畢竟IT技術更新太快。還是自己動手實踐一下更好！


注：Valgrind對於cache的測量，只是一種`模擬`。但是按照valgrind的文檔，結果的可靠性還是有保證的。
