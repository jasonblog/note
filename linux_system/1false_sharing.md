# 1(FALSE SHARING)


注：考虑到我本人长期使用linux系统做开发，因此有些代码在windows环境下无法编译或者会有问题，建议大家都使用linux环境做实验，最好是2.6内核的，处理器需要是多核。很多读者说我是纸上谈兵，这个确实不好，从本系列开始基本都是50行左右的代码。本系列不代表任何学术或业界立场，仅我个人兴趣爱好，由于水平有限，错误难免，请不要有过分期望。


废话不多说，今天就写第一篇如下：



以下一段代码分别编译成两个程序，仅仅是变量定义的差别，执行时间差距巨大，这是什么原因呢？

本博客暂不解密，等数天后，我把后半部写上，希望读者朋友们踊跃实验，并回答。


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
        pthread_create(&thread[0],NULL,sum1,NULL);  //创建2个线程分别求和
        pthread_create(&thread[1],NULL,sum2,NULL);
        pthread_join(thread[0],NULL);    //等待2个线程结束计算。
        pthread_join(thread[1],NULL);
        free(thread);
        printf("cnt_1:%d,cnt_2:%d",cnt_1,cnt_2);
}
```

编译方法：
```sh
g++ fs.cpp -o test_nfs -g -D NONFS –lpthread
g++ fs.cpp -o test_fs -g -D FS –lpthread
```
用time ./test_nfs 和 time ./test_fs会发现执行时间差别很大，请读者踊跃跟帖作答，谢谢。



查了一下 __attribute__((aligned(64)))，是让变量按64字节对齐，让两个变量在内存的位置相聚远一些，避开了false share的情况。所以时间少了。
按照ForestDB的说法，如果编译器自动把两个变量安排得相聚远了，这个__attribute__((aligned(64)))就没啥意义了。

## 【续】

该文有很多网友回复，比较集中的看法是CPU字节对齐，巧合的是有一个朋友用这个代码做了测试，发现对齐和不对齐的代码执行的速度是一样的，原因是他的笔记本安装的linux操作系统，而笔记本是单核的，所以就出现了这个状况，如果和CPU字节对齐，在单核的情况下怎么会速度一样呢？另外如果是CPU字节对齐，把线程去掉，替换成两个函数依次执行，也应该有效率的差异。 

这是一种典型的FALSE SHARING问题，在SMP(对称多处理）的架构下常见的问题。SMP简单的说就是多个CPU核，共享一个内存和总线，L1 cache也叫芯片缓存，一般是私有的，即每个CPU核带一个，L2 cache可能是私有的也可能是部分共享的。

为了表明FALSE SHARE带来的影响，设计了这个简单的多线程程序，包含两个线程，他们分别做求和使用不同的变量，但由于cnt_1的地址和cnt_2的地址在同一条cache line中，实测环境中cnt_1的地址为0x600c00，cnt_2的地址为0x600c08，而cache line的大小为64个字节（cache line大小可以通过getconf LEVEL1_DCACHE_LINESIZE得到,或者命令cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size得到），这样就会发生FALSE SHARING问题。将两个变量在64字节对齐后，cnt_1的地址为0x600c40，cnt_2的地址为0x600c80，恰好错开在两条cache line上，源代码参加上篇博客。



FALSE SHARING问题在此前的博客也有详细讨论，可以参见：http://blog.csdn.net/pennyliang/archive/2010/07/27/5766541.aspx



最后，可能有读者会问，有谁会这么脑残写这样的代码，日常编码怎么会碰到这样的问题呢？我给大家说一个具体的场景，假如有一个lock-free的queues，里面包含了很多类型的queue，每个queue包含一个head和一个tail，这两个值分别被消费者和生产者之间竞争，因此如果不考虑false sharing问题，可能会造成低效代码。这样一个多线程共享的队列结构（多生产者，多消费者共享）用以下哪种结构更好呢？我不再公布答案，有兴趣的朋友可以去找找这方面的代码，看看他们是怎么写的。


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
