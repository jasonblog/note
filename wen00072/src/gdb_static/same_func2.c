#include <stdio.h>

#if OP1
void func1(void)
{
    printf("%s: %s_OP1\n", __FILE__, __PRETTY_FUNCTION__);

}
#else
void func1(void)
{
    printf("%s: %s_NOT_OP1\n", __FILE__, __PRETTY_FUNCTION__);

}
#endif
