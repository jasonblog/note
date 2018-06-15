#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdatomic.h>
#include <sys/syscall.h>
#include <signal.h>
#include <assert.h>

volatile atomic_int a, b, c, d, e, f;

int main(int argc, char** argv) {
    volatile int w, x, y, z, l , m, n;
    w = 0x1111;
    atomic_store_explicit(&a, 0x2222, memory_order_relaxed);
    x = 0x3333;
    atomic_load_explicit(&b, memory_order_consume);
    y=0x5555;
    atomic_load_explicit(&c, memory_order_acquire);
    z=0x7777;
    atomic_store_explicit(&d, 0x8888, memory_order_release);
    l=0x9999;
    //atomic_store_explicit(&e, 0xAAAA, memory_order_acq_rel);
    m=0xBBBB;
    //atomic_store_explicit(&f, 0xCCCC, memory_order_seq_cst);
    n=0xDDDD;
    printf("%d\n", a | b | c | d | e | f | w | x | y | z | l | m | n);
}