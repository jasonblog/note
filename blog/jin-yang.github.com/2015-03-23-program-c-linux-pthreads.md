---
title: Linux 線程編程
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: linux,program,pthread
description: 簡單介紹下 Linux 中與線程相關的編程。
---

簡單介紹下 Linux 中與線程相關的編程。

<!-- more -->


## TSD

在多線程程序中，所有線程共享程序中的變量，對於全局變量，所有線程都可以訪問修改，如果希望麼個線程單獨使用，那麼就需要 Thread Specific Data, TSD 了。

簡言之，表面是一個全局變量，所有線程都可以使用它，而實際實現時，它的值在每一個線程中又是單獨存儲的。

TSD 的使用方法簡單介紹如下：

1. 聲明一個類型為 pthread_key_t 類型的全局變量，後續所有的操作都會用到該變量；
2. 在創建線程前調用 pthread_key_create() 創建該變量，入參為上面的變量以及清理函數(為NULL時調用默認的清理函數)；
3. 啟動線程，在不同的線程中分別通過 pthread_setspcific() 和 pthread_getspecific() 函數設置以及獲取變量。

使用的函數聲明如下。

{% highlight c %}
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
{% endhighlight %}

如下是一個簡單示例。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_key_t key;
struct test_struct { // 用於測試的結構
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
