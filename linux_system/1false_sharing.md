# 1(FALSE SHARING)


注：考慮到我本人長期使用linux系統做開發，因此有些代碼在windows環境下無法編譯或者會有問題，建議大家都使用linux環境做實驗，最好是2.6內核的，處理器需要是多核。很多讀者說我是紙上談兵，這個確實不好，從本系列開始基本都是50行左右的代碼。本系列不代表任何學術或業界立場，僅我個人興趣愛好，由於水平有限，錯誤難免，請不要有過分期望。


廢話不多說，今天就寫第一篇如下：



以下一段代碼分別編譯成兩個程序，僅僅是變量定義的差別，執行時間差距巨大，這是什麼原因呢？

本博客暫不解密，等數天後，我把後半部寫上，希望讀者朋友們踴躍實驗，並回答。


```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#ifdef FS
size_t cnt_1;
size_t cnt_2;
#endif
#ifdef NONFS
size_t __attribute__((aligned(64))) cnt_1;
size_t __attribute__((aligned(64))) cnt_2;
#endif
void* sum1(void*)
{
        for(int i=0;i < 10000000;++i) {
                cnt_1 += 1;
        }
};
void* sum2(void*)
{
        for(int i=0;i < 10000000;++i) {
                cnt_2 += 1;
        }
};
int main()
{
        pthread_t* thread = (pthread_t*) malloc(2*sizeof( pthread_t));
        pthread_create(&thread[0],NULL,sum1,NULL);  //創建2個線程分別求和
        pthread_create(&thread[1],NULL,sum2,NULL);
        pthread_join(thread[0],NULL);    //等待2個線程結束計算。
        pthread_join(thread[1],NULL);
        free(thread);
        printf("cnt_1:%d,cnt_2:%d",cnt_1,cnt_2);
}
```

編譯方法：
```sh
g++ fs.cpp -o test_nfs -g -D NONFS –lpthread
g++ fs.cpp -o test_fs -g -D FS –lpthread
```
用time ./test_nfs 和 time ./test_fs會發現執行時間差別很大，請讀者踴躍跟帖作答，謝謝。



查了一下 __attribute__((aligned(64)))，是讓變量按64字節對齊，讓兩個變量在內存的位置相聚遠一些，避開了false share的情況。所以時間少了。
按照ForestDB的說法，如果編譯器自動把兩個變量安排得相聚遠了，這個__attribute__((aligned(64)))就沒啥意義了。

## 【續】

該文有很多網友回覆，比較集中的看法是CPU字節對齊，巧合的是有一個朋友用這個代碼做了測試，發現對齊和不對齊的代碼執行的速度是一樣的，原因是他的筆記本安裝的linux操作系統，而筆記本是單核的，所以就出現了這個狀況，如果和CPU字節對齊，在單核的情況下怎麼會速度一樣呢？另外如果是CPU字節對齊，把線程去掉，替換成兩個函數依次執行，也應該有效率的差異。 

這是一種典型的FALSE SHARING問題，在SMP(對稱多處理）的架構下常見的問題。SMP簡單的說就是多個CPU核，共享一個內存和總線，L1 cache也叫芯片緩存，一般是私有的，即每個CPU核帶一個，L2 cache可能是私有的也可能是部分共享的。

為了表明FALSE SHARE帶來的影響，設計了這個簡單的多線程程序，包含兩個線程，他們分別做求和使用不同的變量，但由於cnt_1的地址和cnt_2的地址在同一條cache line中，實測環境中cnt_1的地址為0x600c00，cnt_2的地址為0x600c08，而cache line的大小為64個字節（cache line大小可以通過getconf LEVEL1_DCACHE_LINESIZE得到,或者命令cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size得到），這樣就會發生FALSE SHARING問題。將兩個變量在64字節對齊後，cnt_1的地址為0x600c40，cnt_2的地址為0x600c80，恰好錯開在兩條cache line上，源代碼參加上篇博客。



FALSE SHARING問題在此前的博客也有詳細討論，可以參見：http://blog.csdn.net/pennyliang/archive/2010/07/27/5766541.aspx



最後，可能有讀者會問，有誰會這麼腦殘寫這樣的代碼，日常編碼怎麼會碰到這樣的問題呢？我給大家說一個具體的場景，假如有一個lock-free的queues，裡面包含了很多類型的queue，每個queue包含一個head和一個tail，這兩個值分別被消費者和生產者之間競爭，因此如果不考慮false sharing問題，可能會造成低效代碼。這樣一個多線程共享的隊列結構（多生產者，多消費者共享）用以下哪種結構更好呢？我不再公佈答案，有興趣的朋友可以去找找這方面的代碼，看看他們是怎麼寫的。


```sh
【1】

struct queues{

          type head1 

          type head2

          ...

          type headn

          type tail1

          type tail2

          ...

          type tailn 

}

【2】

struct queue{

          type head1 

          type tail1

          type head2

          type tail2

          ...

          type headn

          type tailn 

}

【3】

struct queue{

          type head1 

          type head2

          ...

          type headn

          type add_enough_padding;

          type tail1

          type tail2

          ...

          type tailn

          type add_enough_padding; 

}

【4】

struct queue{

          type head1

          type add_enough_padding; 

          type tail1

          type add_enough_padding; 

          type head2

          type add_enough_padding; 

          type tail2

          type add_enough_padding; 

          ...

          type headn

          type add_enough_padding; 

          type tailn

          type add_enough_padding;  

}
```
