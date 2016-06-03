# IEEE764 using C


```c
#include <stdio.h>

int main() {
    int i = 0xc15a0000;  // 1 1000 0010 10110100000000000000000
    float f = -13.625;  // 1 1000 0010 10110100000000000000000
    int _i = -1051066368;  // 1 1000 0010 10110100000000000000000
    // double dd = 15.12334523462346;
    double dd = 15.0;
    double ee = -15.0;

#if 1
    printf("%lld\n",sizeof(dd));
    printf("%lld\n",sizeof(unsigned long long));
    printf("0x%llx\n", *(unsigned long long*)&dd);
#else
    // c15a0000 = 1 1000 0010 10110100000000000000000   IEEE 754
    printf("0x%08X\n", *(unsigned int*)&f);
    printf("0x%08X\n", *(unsigned int*)&i);
    printf("0x%08X\n\n", *(unsigned int*)&_i);
    printf("0x%08X\n\n", *(unsigned long long*)&dd);

    printf("%f\n", 00000000000000000000000000000000);
    printf("f=%f\n",f);
    // printf("f=%d\n",f);
    printf("i =%f\n",i);
    printf("_i=%f\n",_i);

    printf("_i=%f\n\n",_i);

    printf("%d\n",*(int*)&f);
#endif

    return 0;
}
```