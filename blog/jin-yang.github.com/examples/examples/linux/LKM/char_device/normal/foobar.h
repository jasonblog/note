#ifndef __FOOBAR_H__
#define __FOOBAR_H__
#include <linux/ioctl.h>
#define DEV_FOOBAR_CHAR_TYPE 'k'
#define DEV_FOOBAR_RESET     _IO(DEV_FOOBAR_CHAR_TYPE, 0x10)
#define DEV_FOOBAR_GETVALUE _IOR(DEV_FOOBAR_CHAR_TYPE, 0x11, int)
#define DEV_FOOBAR_SETVALUE _IOW(DEV_FOOBAR_CHAR_TYPE, 0x12, int)
#define DEV_FOOBAR_MAXNR    0x12
#endif
