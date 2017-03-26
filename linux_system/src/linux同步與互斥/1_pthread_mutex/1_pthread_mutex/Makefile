# Copyright (C) 2014 shallnew \at 163 \dot com

CFLAGS += -g -Wall -Werror -O2
CPPFLAGS +=
LDFLAGS += -lpthread

# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
SRC_FILES = $(wildcard *.c)
SRC_OBJ = $(SRC_FILES:.c=.o)
SRC_BIN = target_bin

all : $(SRC_BIN)

$(SRC_BIN) : $(SRC_OBJ)
	@echo "Linking $(SRC_BIN)..."
	@$(CC) -o $@ $^ $(LDFLAGS)

obj : $(SRC_OBJ)

tag:
	ctags -R

# clean target
clean:
	$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe

distclean:
	$(RM) $(SRC_OBJ) $(SRC_BIN) $(SRC_BIN).exe tags *~

.PHONY : all obj tag clean disclean

