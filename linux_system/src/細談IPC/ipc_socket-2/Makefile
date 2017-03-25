# makefile for ipc example
#
# author's blog: http://blog.csdn.net/shallnet or http://blog.csdn.net/gentleliu


# PATH:=$(PATH):/xxx/xxx/bin
# CROSS_COMPILE=arm-linux-

CC = $(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip

CFLAGS += -Wall
CPPFLAGS += -Iinclude
LDFLAGS += 

CLT_OBJ = $(patsubst %.c, %.o, $(wildcard clt/*.c))
SER_OBJ = $(patsubst %.c, %.o, $(wildcard ser/*.c))

CLT_BIN = client
SER_BIN = server
SRC_BIN = $(CLT_BIN) $(SER_BIN)

all: $(SRC_BIN)

$(CLT_BIN) : $(CLT_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
$(SER_BIN) : $(SER_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(CLT_OBJ) $(SER_OBJ) $(SRC_BIN) *~ clt/*~ ser/*~ include/*~

