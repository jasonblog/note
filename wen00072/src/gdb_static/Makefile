CFLAGS+=-Wall -Werror -g3
TARGET=test_same_func
SRCS=test_same_func.c

ifeq ($(USE_FILE), 1)
    SRCS += same_func1.c
else
    SRCS += same_func2.c
endif

ifeq ($(EN_OP1), 1)
    CFLAGS += -DOP1=1
endif

OBJS=$(patsubst %.c, %.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) -o $@ $^
	gdb -ex 'file $@' -ex 'info line func1' -ex 'quit'

%.o: %.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm *.o *~ $(TARGET) -f
