# author's blog: http://blog.csdn.net/shallnet
#

# CFLAGS = -Wall -Werror -O2
CFLAGS = -Wall
LDFLAGS =


SRC_FILES = $(wildcard *.c)
SRC_OBJ = $(SRC_FILES:.c=.o)
SRC_BIN = target_bin

all : $(SRC_BIN)

$(SRC_BIN) : $(SRC_OBJ)
	@echo "Linking $(SRC_BIN)..."
	@$(CC) -o $@ $^ $(LDFLAGS)

# clean target
clean:
	$(RM) $(SRC_OBJ) $(SRC_BIN) *~

tag:
	ctags -R

.PHONY : all obj tag clean
