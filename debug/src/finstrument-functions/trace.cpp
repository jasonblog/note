#include <stdio.h>

extern "C" {

void
__cyg_profile_func_enter (void *func,  void *caller)
{
    printf("entry %p %p\n", func, caller);
}

void
__cyg_profile_func_exit (void *func, void *caller)
{
    printf("exit %p %p\n", func, caller);
}

}
