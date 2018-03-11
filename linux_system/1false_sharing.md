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
