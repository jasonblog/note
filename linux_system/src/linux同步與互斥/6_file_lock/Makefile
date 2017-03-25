# author's blog: http://blog.csdn.net/shallnet or http://blog.csdn.net/gentleliu

# PATH:=$(PATH):/xxx/xxx/bin
# CROSS_COMPILE=arm-linux-

CC = $(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip

CFLAGS += -g -Wall -Werror -O2
CPPFLAGS +=
LDFLAGS += -L./ -lflock

# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
# SRC_FILES = $(wildcard *.c)
# SRC_OBJ = $(SRC_FILES:.c=.o)

SRC_BIN = write_lock read_lock test_lock
SRC_LIB = libflock.a

all : $(SRC_LIB) $(SRC_BIN)

$(SRC_LIB) : filelock.o
	@echo "ARCHIVE $@..."
	@$(AR) rcs $@ $^

write_lock : write_lock.o
	@echo "Linking $@..."
	@$(CC) -o $@ $^ $(LDFLAGS)

read_lock : read_lock.o
	@echo "Linking $@..."
	@$(CC) -o $@ $^ $(LDFLAGS)

test_lock : test_lock.o
	@echo "Linking $@..."
	@$(CC) -o $@ $^ $(LDFLAGS)


tag:
	ctags -R

# clean target
clean:
	$(RM) $(SRC_BIN) $(SRC_LIB) *.o

distclean:
	$(RM) $(SRC_BIN) $(SRC_LIB) tags *~ *.o

.PHONY : all obj tag clean disclean

