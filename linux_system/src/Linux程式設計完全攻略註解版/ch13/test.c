#include <stdio.h>
#include <endian.h>
int main(void)
{
    printf("Big-endian:\t%d \n Little-endian:\t%d \n mine:\t%d\n", __BIG_ENDIAN,
           __LITTLE_ENDIAN, __BYTE_ORDER);
    return 0;
}


