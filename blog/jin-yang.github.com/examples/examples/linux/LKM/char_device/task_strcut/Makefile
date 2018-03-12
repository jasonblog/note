ifneq	($(KERNELRELEASE),)
obj-m := hello.o
else
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
#    rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers
endif

clean:
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*.cmd .tmp_versions
.PHONY:clean
