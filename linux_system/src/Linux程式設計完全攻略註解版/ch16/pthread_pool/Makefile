CC = gcc
TARGET = pthread_pool
SRC = pthread_pool.c base.c
OBJECT = pthread_pool.o  base.o
INCLUDES = -I./
LDFLAGS = -lpthread

all:$(TARGET)

$(OBJECT):$(SRC)
	$(CC) -c $(INCLUDES) ${SRC}

$(TARGET):$(OBJECT)
	$(CC) -o $@ $(OBJECT) $(LDFLAGS)

.PHONY:clean

clean:
	@rm -rf $(OBJECT) $(TARGET) *~

