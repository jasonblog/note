ifneq	($(KERNELRELEASE),)
obj-m := alloc.o
else
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
#    rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers
endif

KERNEL_VERSION := $(shell uname -r)
secure:
	/usr/src/kernels/$(KERNEL_VERSION)/scripts/sign-file sha256 ~/private_key.priv ~/public_key.der *.ko

clean:
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*.cmd .tmp_versions

.PHONY:clean
