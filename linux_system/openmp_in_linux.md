# OpenMP in linux


##1. Introduction

寫 project euler 的時候, 有些問題想不到速度快的解法, 只好用平行運算來加速
平常程式都以一條 thread 在執行, 要開多條 thread 來跑, 在管理上就很麻煩
OpenMP 讓人可以平行處理, 又可以減少管理的麻煩

##2. 對 for 做 parallel

底下是範例程式

```c
#include <stdio.h>
#include <omp.h>

int main()
{
    #pragma omp parallel for
    for (int i = 0; i < 6; i++) {
        printf("%d ", i);
    }

    printf("\r\n");

    return 0;
}
```

然後compile它, 這邊使用c99來compile:

```sh
c99 -o openmp_example_01 openmp_example_01.c -fopenmp
```

幾個重點
- 在 header 檔要加入 omp.h
- 在 for 迴圈前加上 #pragma omp, 這個是用來使用omp的功能, 後面是控制敘述
- compile 的時候加上 -fopenmp

結果可以看到
```sh
4 0 1 5 2 3
```

每次執行的結果都不一樣 

##3. 語法

它的語法大改長這樣

```c
#pragma omp <directive> [clause[[,] clause] ...]
```

前個例子裡, 其實 parallel 和 for 都是 directive 的敘述, 所以它可以拆成兩個


```c
#pragma omp parallel
{
    #pragma omp for
    for (int i = 0; i < 6; i++)
    {
        printf("%d ", i);
    }
}
```

可以看 wiki 上面的指令清單
OpenMP in wiki

##4. thread information

如果要知道目前的 thread number, 可以用 omp_get_thread_num()

```c
#include <stdio.h>
#include <omp.h>

int main()
{
    #pragma omp parallel for
    for (int i = 0; i < 6; i++) {
        printf("T:%d i:%d\r\n", omp_get_thread_num(), i);
    }

    return 0;
}
```

執行結果
```c
T:2 i:4
T:1 i:2
T:1 i:3
T:0 i:0
T:0 i:1
T:3 i:5
```

可以看到我的電腦上, 它跑了4條thread, 其中 thread 0處理 i 是 0 & 1的情況, thread 1 處理 i 是 2 & 3 的情況....

##5. 控制 thread number

可以用 num_threads(n) 來控制要開多少 thread

```c
#include <stdio.h>
#include <omp.h>

int main()
{
    #pragma omp parallel for num_threads(2)
    for (int i = 0; i < 6; i++) {
        printf("T:%d i:%d\r\n", omp_get_thread_num(), i);
    }

    return 0;
}
```
執行結果可以看到只開2條thread了

```c
T:1 i:3
T:1 i:4
T:1 i:5
T:0 i:0
T:0 i:1
T:0 i:2
```