# signed 跟 unsigned 高位元差別

```c
#include <stdio.h>

int main(void)
{
    char a[4] = {0xFF, 0xEE, 0xDD, 0xCC};
    unsigned char *uc = a;
    unsigned char b[4] = {0xFF, 0xEE, 0xDD, 0xCC};

    printf("%x\n", (a[3]&0xFF) << 24);
    printf("%x\n", (a[2]&0xFF) << 16);
    printf("%x\n", (a[1]&0xFF) << 8);
    printf("%x\n\n", (a[0]&0xFF));

    printf("uc=%x\n", (uc[3]) << 24);
    printf("uc=%x\n", (uc[2]) << 16);
    printf("uc=%x\n", (uc[1]) << 8);
    printf("uc=%x\n\n", (uc[0]));

    printf("%x\n", (a[3]) << 24);
    printf("%x\n", (a[2]) << 16);
    printf("%x\n", (a[1]) << 8);
    printf("%x\n\n", (a[0]));


    printf("%x\n", (b[3]&0xFF) << 24);
    printf("%x\n", (b[2]&0xFF) << 16);
    printf("%x\n", (b[1]&0xFF) << 8);
    printf("%x\n", (b[0]&0xFF));


    printf("%x\n", (b[3]) << 24);
    printf("%x\n", (b[2]) << 16);
    printf("%x\n", (b[1]) << 8);
    printf("%x\n", (b[0]));

    return 0;
}

```