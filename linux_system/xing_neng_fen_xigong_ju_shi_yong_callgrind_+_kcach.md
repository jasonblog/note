# 性能分析工具使用 callgrind+kcachegrind


##一、簡述
本文講述如何應用callgrind和kcachegrind進行性能分析。該工具可以方便分析出可執行文件函數的執行時間及調用關係。
    
##二、開發環境
操作系統
Mint17
內核
3.13.0
編譯器
gcc4.8.0
CPU
VIA Nano X2 L4530 @ 1.6+ GHz
內存
4G
多核
2個
 
三、安裝
sudo apt-get install valgrind  kcachegrind
四、使用
 原始調試文件：
perf.c:

```c
#include <stdio.h>
void hello()
{
        printf("hello world!\n");
}
void test()
{
        hello();
        printf("test \n");
}
void main()
{
        hello();
        hello();
        test();
        test();
        test();
}
```

 編譯：
 
 ```sh
 gcc  -W -Wall  -pedantic -ansi -std=gnu99 -g -O0 -pg perf.c -o perf 
 ```
 
 
 利用valgrind生成性能日誌
 
 ```sh
 valgrind --tool=callgrind ./perf
```

利用kcachegrind 打開生成的callgrind.out.2622。

```sh
kcachegrind callgrind.out.2622
```

可以看出分析出了每個函數調用的時間及比例以調用關係圖。
 