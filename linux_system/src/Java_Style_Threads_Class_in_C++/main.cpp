/*
   main.cpp

   Test application for a Java style thread class in C++.

   ------------------------------------------

   Copyright (c) 2013 Vic Hargrave

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread.h"

class MyThread : public Thread
{
public:
    void* run()
    {
        for (int i = 0; i < 5; i++) {
            printf("thread %lu running - %d\n", (long unsigned int)self(), i + 1);
            sleep(2);
        }

        printf("thread done %lu\n", (long unsigned int)self());
        return NULL;
    }
};

int main(int argc, char** argv)
{
    MyThread* thread1 = new MyThread();
    MyThread* thread2 = new MyThread();
    thread1->start();
    thread2->start();
    thread1->join();
    thread2->join();
    printf("main done\n");
    exit(0);
}
