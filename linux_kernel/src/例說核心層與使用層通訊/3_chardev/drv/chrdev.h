#ifndef CHRDEV_H____
#define CHRDEV_H____

#define CHAR_DEV_NO 1
#define CHAR_DEV_DATA_SIZE 4096

#define MEM_CLEAR   0x0
#define MEM_RESET   0x1


struct chr_dev {
    struct cdev     cdev;
    char*            data;
};

#endif
