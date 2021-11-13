obj-m       += hworld.o
hworld-objs   := \
	helloworld.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build

KERNEL_PWD := $(shell pwd)


ARCH:= x86_64

all:
	make -C ${KERNEL_DIR} SUBDIRS=$(KERNEL_PWD) ARCH=${ARCH} modules
clean:  
	rm -rf *.o *.ko *.mod.c .tmp_versions *.o.ur-safe *.o.cmd *.cache.mk *.tmp *.cmd  .*.o .*.cmd

