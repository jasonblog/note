# A netmoon common Makefile for rtk8382m, write by allen

# broadcom
PATH:=$(PATH):/opt/toolchain/broadcom/bcm5334x/usr/bin/
CROSS_COMPILE=arm-linux-

# realtek
# PATH:=$(PATH):/home/sw/rtl/toolchain/linux/v2.6.19/mips-linux-uclibc/bin
# CROSS_COMPILE=mips-linux-

CC = $(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip

CFLAGS += -g -Wall -Werror -O2 -DNMS_BCM53346
# CFLAGS += -g -Wall -O2 -DNMS_RTK8382M
CPPFLAGS += -I. -I./include
# LDFLAGS += -L./rtk8382m_lib -llog -lnms_common

# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
SRC_FILES = $(wildcard *.c)
SRC_OBJ = $(SRC_FILES:.c=.o)
SRC_BIN = loop_read

all : $(SRC_BIN)

$(SRC_BIN) : $(SRC_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	cp $@ /tftpboot/

obj : $(SRC_OBJ)

tag:
	ctags -R

# clean target
clean:
	$(RM) $(SRC_OBJ) $(SRC_BIN)

distclean:
	$(RM) $(SRC_OBJ) $(SRC_BIN)  tags *~

.PHONY : all obj tag clean disclean

