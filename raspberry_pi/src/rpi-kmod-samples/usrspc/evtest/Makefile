CC = $(CROSS_COMPILE)gcc
CFLAGS += 
LDFLAGS +=

.PHONY: clean

all: install

evtest: evtest.c
	$(CC) $(CFLAGS) evtest.c -o evtest $(LDFLAGS)

install: evtest
	mkdir -p ../../sysroot/usr/bin/
	cp evtest ../../sysroot/usr/bin/.
	
clean:
	rm evtest
