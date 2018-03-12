obj-m := hello.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
	gcc -Wall app.c -o app

KERNEL_VERSION := $(shell uname -r)
secure:
	/usr/src/kernels/$(KERNEL_VERSION)/scripts/sign-file sha256 ~/private_key.priv ~/public_key.der hello.ko

.PHONY:clean

clean:
	rm -f *.o *.ko *.mod.c *.order *.symvers app
