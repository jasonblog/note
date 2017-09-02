#ifndef BASE_H
#define BASE_H
#include <stdlib.h>
//#define DEBUG
#define NO_DSP
//#define NORMAL
#define CARDBOARD_FUSION
#define PLUGIN
#define FUISON_WORKAROUND

#define container_of(ptr, type, member)                 \
    ({                              \
        const typeof(((type *)0)->member ) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type,member));   \
    })

#define new(TYPE) TYPE ## _construct(malloc(sizeof(TYPE)))

#define delete(TYPE, ptr)   do              \
    {               \
        TYPE ## _destruct(ptr); \
        free(ptr);      \
    }               \
    while(0)
#endif /* BASE_H */
