# 12(存储计算)


有时候，我们可以将计算好的值进行存储，需要的时候取出，这样可以大大降低计算量，用空间代替时间。

 

我们从一个问题出发，农夫John和他的朋友们一同去参加Cownty展会，这个展会的门票是50元，排队购票的人

有2n个人，其中n个人拿着100元的钞票，另外n个人拿着50元的钞票，农夫john想知道在这种情况下着2n个人共有

多少种排队的方式，使得售票处在不准备零钱的情况下，也能把票卖给这2n个人，而不会出现找不开钱的局面。

这是一个经典的组合问题，最后可以通过求解catalan数一步解决，我们这里通过深入搜索，写通项和存储计算三种方法来实现。

通过实现，存储计算的这种方法最好，但需要对计算的顺序精心设计。

 

在动态规划等很多场合都会用到将计算存储在一个数组中，此前的关于pfordelta的实现中也出现了将计算存储在数组中的方式，这是很常见的技巧，该问题前两种方法只所以慢一方面是因为大量重复冗余的计算，另一方面是过多的跳转，通过第三种方法改写后，计算的效率大大提高了，没有组合数学背景的同学仔细研读，应该也可以读懂，该题来自吴文虎的《程序设计中的组合数学》，实现也部分参考了这本书，但有较多不同。


```c
#include <stdio.h>

#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile("rdtsc" : "=A"(x));
    return x;
}
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

#endif


int all_n = 20;
int five_cnt = 10;
int ten_cnt = 10;

int dfs(int i, int change_cnt, int fi, int tj,
        int& all_cases_cnt) //fi表示拿50元钞票的人数，tj表示拿100元钞票的人数
{
    if (i == 1) {
        if (change_cnt == 1 && tj == 1 && fi == 0) {
            all_cases_cnt += 1;
            tj -= 1;
        } else if (change_cnt == 0 && tj == 0 && fi == 1) {
            all_cases_cnt += 1;
            fi -= 1;
        }
    } else {
        for (int k = 0; k < 2; ++k) {
            if (k == 0) {
                if (fi >= 1) {
                    dfs(i - 1, change_cnt + 1, fi - 1, tj, all_cases_cnt);
                }
            }

            if (k == 1) {
                if (tj >= 1) {
                    if (change_cnt > 0) {
                        dfs(i - 1, change_cnt - 1, fi, tj - 1, all_cases_cnt);
                    }
                }
            }
        }

    }

};

int f(int m, int n)
{
    if (m < n) {                                 //50元的人数少于100元的人数必然无解
        return 0;
    } else if (n ==
               0) {                     //100元的人已经不存在了，后续的都是50元的算1种
        return 1;
    } else {
        return f(m, n - 1) + f(m - 1,
                               n); //否则方案数为先用100元的，再用50元的方案之和.
    }
}

int fast_f(int m, int n)
{
    int fast[m + 1][n + 1];

    for (int i = 0; i <= n;
         ++i) {        //直接改写自f函数的第一个条件分支
        for (int j = 0; j < i && j <= m; ++j) {
            fast[j][i] = 0;
        }
    }

    for (int i = 0; i <= m;
         ++i) {     //直接改写自f函数的第二个分支，计算可以合并到上一个循环中，这里为清晰起见，拆开。
        fast[i][0] = 1;
    }

    for (int i = 1 ; i <= m;
         ++i) {    //这一步的计算有很高的技巧，每一层的计算依赖的另外两个数恰好都是已经计算好的
        for (int j = 1; j <= i && j <= n; ++j) {
            fast[i][j] = fast[i][j - 1] + fast[i - 1][j];
        }
    }

    return fast[m][n - 1] + fast[m - 1][n];
}



int main(void)
{
    int all_cases_cnt = 0;

    int start = rdtsc();
    dfs(all_n, 0, five_cnt, ten_cnt, all_cases_cnt);
    int end = rdtsc();
    printf("方案数%d,cycle:%d/n", all_cases_cnt, end - start);

    start = rdtsc();
    all_cases_cnt = f(five_cnt, ten_cnt);
    end = rdtsc();
    printf("方案数%d,cycle:%d/n", all_cases_cnt, end - start);

    start = rdtsc();
    all_cases_cnt = fast_f(five_cnt, ten_cnt);
    end = rdtsc();
    printf("方案数%d,cycle:%d/n", all_cases_cnt, end - start);

    return 0;
}
```