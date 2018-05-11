//
//  main.cpp
//  mythread
//
//  Created by 杜国超 on 17/9/7.
//
//

#include <iostream>
#include <vector>
#include <chrono>
#include "threadpool.h"

int main()
{
    
    CThreadPool* pool = new CThreadPool;
    std::vector< std::future<int> > results;
    
    for(int i = 0; i < 50; ++i) {
        results.emplace_back(pool->PushTaskBack([i,pool] {
            printf("i = %d thread id = %ld\n",i,this_thread::get_id());
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (i % 10 == 0)
            {
                pool->PushTaskBack([i] {
                    printf("I am %d multiple of 10\n",i / 10);
                });
            }
            return i*i;
        }));
    }

    for(int i = 50; i <= 100; ++i) {
        results.emplace_back(pool->PushTaskFront([i,pool] {
            printf("i = %d thread id = %ld\n",i,this_thread::get_id());
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (i % 10 == 0)
            {
                pool->PushTaskFront([i] {
                    printf("I am %d multiple of 10\n",i / 10);
                });
            }
            return i*i;
        }));
    }

    for(auto && result: results)
    {
        printf("result = %d \n",result.get());
    }

    if (pool != NULL)
    {
        delete pool;
        pool = NULL;
    }

    return 0;
}
