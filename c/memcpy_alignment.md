# memcpy alignment


```c
#include <stdio.h>
#include <stdlib.h>
#include <memory.h> // memcpy
#include <time.h>

#define N 787654321
typedef unsigned int  word; /* 假定 4bytes */
typedef unsigned char byte;

void cpy1(void *target, void *source, size_t size)
{
    memcpy(target, source, size);
}

void cpy2(void* target, void* source, size_t size)
{
    register byte* Dest = (byte*)target;
    register byte* Sour = (byte*)source;
    register byte* End = Sour + size;
    while( Sour!=End ) 
        *Dest++ = *Sour++;
}

void cpy3(void* target, void* source, size_t size)
{
    register word *Wsrc_s  = (word*)source;
    register word *Wdes_s  = (word*)target; 
    register byte *Bsrc_s;
    register byte *Bdes_s;
    // 處理 4 bytes 
    while(size>4){
        *Wdes_s++ = *Wsrc_s++;
        size-=4;
    }
    // 處理 < 4 bytes
    Bsrc_s = (byte*)(Wsrc_s);
    Bdes_s = (byte*)(Wdes_s);
    while(size) {
        *Bdes_s++ = *Bsrc_s++;
        --size;
    }
}

// 
typedef struct tagT256{ byte trunk[256];}T256;
void cpy4(void* target, void* source, size_t size)
{
    register T256 *Tsrc_s  = (T256*)source;
    register T256 *Tdes_s  = (T256*)target;
    register word *Wsrc_s;
    register word *Wdes_s;
    register byte *Bsrc_s;
    register byte *Bdes_s;
    // 處理 256 bytes 
    while(size>256){
        *Tdes_s++ = *Tsrc_s++;
        size-=256;
    }
    // 處理 4 bytes
    Wsrc_s = (word*)(Tsrc_s);
    Wdes_s = (word*)(Tdes_s);
    while(size>4){
        *Wdes_s++ = *Wsrc_s++;
        size-=4;
    }
    // 處理 < 4 bytes
    Bsrc_s = (byte*)(Wsrc_s);
    Bdes_s = (byte*)(Wdes_s);
    while(size) {
        *Bdes_s++ = *Bsrc_s++;
        --size;
    }
}
// 
typedef struct tagT1024{    byte trunk[1024];}T1024;
void cpy5(void* target, void* source, size_t size)
{
    register T1024 *TTsrc_s = (T1024*)source;
    register T1024 *TTdes_s = (T1024*)target;
    register T256 *Tsrc_s;
    register T256 *Tdes_s;
    register word *Wsrc_s;
    register word *Wdes_s;
    register byte *Bsrc_s;
    register byte *Bdes_s;
    // 處理 1024 bytes
    while(size>1024){
        *TTdes_s++ = *TTsrc_s++;
        size-=1024;
    }
    // 處理 256 bytes 
    Tsrc_s = (T256*)(TTsrc_s);
    Tdes_s = (T256*)(TTdes_s);
    while(size>256){
        *Tdes_s++ = *Tsrc_s++;
        size-=256;
    }
    // 處理 4 bytes
    Wsrc_s = (word*)(Tsrc_s);
    Wdes_s = (word*)(Tdes_s);
    while(size>4){
        *Wdes_s++ = *Wsrc_s++;
        size-=4;
    }
    // 處理 < 4 bytes
    Bsrc_s = (byte*)(Wsrc_s);
    Bdes_s = (byte*)(Wdes_s);
    while(size) {
        *Bdes_s++ = *Bsrc_s++;
        --size;
    }
}

void cpy6(void* target, void* source, size_t size)
{
    register byte* to=(byte*)target;
    register byte* from=(byte*)source;
    register size_t n=(size+7)>>3;
    switch (n&7){
        case 0: do {    *to++ = *from++;
        case 7:         *to++ = *from++;
        case 6:         *to++ = *from++;
        case 5:         *to++ = *from++;
        case 4:         *to++ = *from++;
        case 3:         *to++ = *from++;
        case 2:         *to++ = *from++;
        case 1:         *to++ = *from++;
                } while(--n > 0);
    }               
}

void cpy7(void* target, void* source, size_t size) 
{
    register byte* to=(byte*)target;
    register byte* from=(byte*)source;
    register size_t n=size;
    while(n--)
        *to++=*from++;
}

int main()
{
    int i;
    clock_t t;
    const size_t size = sizeof(byte)*N;
    byte* arr1 = (byte*)malloc(size);
    byte* arr2 = (byte*)malloc(size);
    // setter
    for(i=0; i<N; ++i) arr1[i] =i;
    
    // memcpy test
    memset(arr2,-1,size);
    t = clock(), cpy1(arr2, arr1, size);
    printf("cpy1 : %-5ld  ", clock()-t);
    puts((!memcmp(arr1, arr2, size))? "arr2=arr1" : "arr2!=arr1");

    memset(arr2,-1,size);
    t = clock(), cpy2(arr2, arr1, size);
    printf("cpy2 : %-5ld  ", clock()-t);
    puts((!memcmp(arr1, arr2, size))? "arr2=arr1" : "arr2!=arr1");

    memset(arr2,-1,size);
    t = clock(), cpy3(arr2, arr1, size);
    printf("cpy3 : %-5ld  ", clock()-t);
    puts((!memcmp(arr1, arr2, size))? "arr2=arr1" : "arr2!=arr1");

    memset(arr2,-1,size);
    t = clock(), cpy4(arr2, arr1, size);
    printf("cpy4 : %-5ld  ", clock()-t);
    puts((!memcmp(arr1, arr2, size))? "arr2=arr1" : "arr2!=arr1");

    memset(arr2,-1,size);
    t = clock(), cpy5(arr2, arr1, size);
    printf("cpy5 : %-5ld  ", clock()-t);
    puts((!memcmp(arr1, arr2, size))? "arr2=arr1" : "arr2!=arr1");

    memset(arr2,-1,size);
    t = clock(), cpy6(arr2, arr1, size);
    printf("cpy6 : %-5ld  ", clock()-t);
    puts((!memcmp(arr1, arr2, size))? "arr2=arr1" : "arr2!=arr1");

    memset(arr2,-1,size);
    t = clock(), cpy7(arr2, arr1, size);
    printf("cpy7 : %-5ld  ", clock()-t);
    puts((!memcmp(arr1, arr2, size))? "arr2=arr1" : "arr2!=arr1");

    // release memory
    free(arr1), free(arr2);
    getchar();
    return 0;
}
```

這比memcpy快 可以快個三到四倍，調用方法與memcpy一樣

不做restricted保護

## sse 128-bit暫存器

struct 其實compiler都還是用32 bit暫存器copy 同樣copy 16個byte用128 bit站存器就是只要1個指令

```c
#include <emmintrin.h> /*SSE2*/

#define DIV16(VAL)              ((VAL)>>4)
#define MUL16(VAL)              ((VAL)<<4)

#define MEMCPY(PDST, PSRC, SIZE) sse2_memcpy( (PDST), (PSRC), (SIZE))


void sse2_memcpy(void* pDst, void* pSrc, size_t size)
{
    if (pDst == pSrc) {
        return ;
    }

    unsigned int i;
    unsigned int nLoops;

    char* pcDst, *pcSrc;
    __m128i* pMovSrc, *pMovDst;


    pMovDst = (__m128i*)(pDst);
    pMovSrc = (__m128i*)(pSrc);

    nLoops = DIV16((unsigned int)size);

    __m128i _miTemp;

    for (i = 0; i < nLoops; i++) {
        _miTemp = _mm_loadu_si128(pMovSrc);
        _mm_storeu_si128(pMovDst, _miTemp);
        pMovDst++;
        pMovSrc++;
    }


    pcDst = (char*)(pMovDst);
    pcSrc = (char*)(pMovSrc);

    for (i = MUL16(nLoops); i < size; i++) {
        *pcDst++ = *pcSrc++;
    }

}/*sse_memcpy*/
```