
# CC=$(CROSS_COMPILE)gcc
# STRIP=$(CROSS_COMPILE)strip

# LINUX_SRC = /lib/modules/2.6.31.5-127.fc12.i686.PAE
# LINUX_SRC = /usr/src/kernels/2.6.31.5-127.fc12.i686.PAE/
# LINUX_SRC = /home/vitesse/vsc7427_7434/sdk/build_dir/linux-vcoreiii_jag1/linux_vsc7434

KVERS = $(shell uname -r)

MODULE_NAME = chardev
SRC_OBJ = chrdev.o

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs = $(SRC_OBJ)

all: make_module

make_module:
	# make -C $(LINUX_SRC) M=`pwd` modules
	make -C /lib/modules/$(KVERS)/build M=`pwd` modules
	cp $(MODULE_NAME).ko ../

.PHONY: all clean
clean:
	rm -fr *.ko *.o* *.symvers *.markers *.order *.mod* .*cmd .tmp_versions ../$(MODULE_NAME).ko *~
