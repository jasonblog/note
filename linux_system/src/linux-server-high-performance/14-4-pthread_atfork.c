//用于解决14-3中死锁问题，就是在fork前加入如下代码
void prepare()
{
    pthread_mutex_lock(&mutex);
}

void infork()
{
    pthread_mutex_unlock(&mutex);
}

pthread_atfork(prepare, infork, infork);
