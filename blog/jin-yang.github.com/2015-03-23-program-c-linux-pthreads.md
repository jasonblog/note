---
title: Linux 线程编程
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: linux,program,pthread
description: 简单介绍下 Linux 中与线程相关的编程。
---

简单介绍下 Linux 中与线程相关的编程。

<!-- more -->


## TSD

在多线程程序中，所有线程共享程序中的变量，对于全局变量，所有线程都可以访问修改，如果希望么个线程单独使用，那么就需要 Thread Specific Data, TSD 了。

简言之，表面是一个全局变量，所有线程都可以使用它，而实际实现时，它的值在每一个线程中又是单独存储的。

TSD 的使用方法简单介绍如下：

1. 声明一个类型为 pthread_key_t 类型的全局变量，后续所有的操作都会用到该变量；
2. 在创建线程前调用 pthread_key_create() 创建该变量，入参为上面的变量以及清理函数(为NULL时调用默认的清理函数)；
3. 启动线程，在不同的线程中分别通过 pthread_setspcific() 和 pthread_getspecific() 函数设置以及获取变量。

使用的函数声明如下。

{% highlight c %}
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
{% endhighlight %}

如下是一个简单示例。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_key_t key;
struct test_struct { // 用于测试的结构
    int i;
    float k;
};

void *child1(void *arg)
{
    struct test_struct struct_data;
    struct_data.i = 10;
    struct_data.k = 3.1415;
    pthread_setspecific(key, &struct_data);
    printf("child1--> 0x%p\n", &(struct_data));
    printf("child1--> 0x%p\n", (struct test_struct *)pthread_getspecific(key));
    printf("child1--%d %f\n",
        ((struct test_struct *)pthread_getspecific(key))->i, ((struct test_struct *)pthread_getspecific(key))->k);
}
void *child2(void *arg)
{
    int temp = 20;
    sleep(1);
    pthread_setspecific(key, &temp);
    printf("child2--> 0x%p\n", &temp);
    printf("child2--> 0x%p\n", (int *)pthread_getspecific(key));
    printf("child2--> %d\n", *((int *)pthread_getspecific(key)));
}

int main(void)
{
    pthread_t tid1, tid2;
    pthread_key_create(&key, NULL);

    pthread_create(&tid1, NULL, child1, NULL);
    pthread_create(&tid2, NULL, child2, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_key_delete(key);
    return (0);
}
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
