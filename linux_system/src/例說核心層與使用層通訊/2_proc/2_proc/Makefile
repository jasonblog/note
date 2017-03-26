
# CC=$(CROSS_COMPILE)gcc
# STRIP=$(CROSS_COMPILE)strip

#LINUX_SRC = /lib/modules/`uname -r`
LINUX_SRC = /usr/src/linux-`uname -r`

MODULE_NAME = proc
SRC_OBJ = core.o

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs = $(SRC_OBJ)

all: make_module

make_module:
	make -C $(LINUX_SRC) M=`pwd` modules

.PHONY: all clean
clean:
	rm -fr *.ko *.o* *.symvers *.markers *.order *.mod* .*cmd .tmp_versions *~
