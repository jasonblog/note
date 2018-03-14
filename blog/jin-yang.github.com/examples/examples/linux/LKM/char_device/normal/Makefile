obj-m := foobar.o
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all: app
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*.cmd .tmp_versions app
app:
	gcc -o $@ -Wall app.c
.PHONY: clean

