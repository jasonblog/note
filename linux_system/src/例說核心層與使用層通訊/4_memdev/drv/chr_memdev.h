#ifndef MEMDEV_H_
#define MEMDEV_H_

#define CHR_MEMDEV_NUM          1
#define CHR_MEMDEV_DATA_SIZE    4096

#define MEM_CLEAR   0x0
#define MEM_RESET   0x1

struct chrmem_dev {
    struct cdev     cdev;
    char*            data;
};

#endif
