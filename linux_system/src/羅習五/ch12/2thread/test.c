#include <stdatomic.h>
void fun() {
    do {
        do {
            atomic_spin_nop ();
            val = atomic_load_relaxed (lock);
        }
        while (val != 0);
    }
    while (!atomic_compare_exchange_weak_acquire (lock, &val, 1));
}
