#include <thread>
#include "../tool.h"
#include <stdlib.h>
#include <stdio.h>

int run(int par) {
    for (int i=0; i< 100000; i++)
    switch (par) {
        case 1: printf(RED"%5d ", i); break;
        case 2: printf(GREEN"%5d ", i); break;
        case 3: printf(CYAN"%5d ", i); break;
        case 4: printf(YELLOW"%5d ", i); break;
    }
}
int main(int argc, char** argv) {
    std::thread thrd1 (run,0); std::thread thrd2 (run,1);
    std::thread thrd3 (run,2); std::thread thrd4 (run,3);
    thrd1.join();thrd2.join();
    thrd3.join();thrd4.join();
}