obj-m += server.o

KID := /lib/modules/`uname -r`/build
PWD := $(shell pwd) 

ifeq ($(strip $(NO_PY)),1)
obj-m := server_npy.o
else
ifeq ($(strip $(NO_PY)),2)
obj-m := server_npy.o
EXTRA_CFLAGS := -DHIGH_PRI
endif 
endif
  
all:  
	make -C $(KID) M=$(PWD) modules  
  
clean:  
	rm -fr  .*.cmd .tmp_versions  *.mod.c  *mod.o *.o *.ko *.c~ 
