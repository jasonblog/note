CFLAGS =-std=gnu99 -g -Wall `libgcrypt-config --cflags`
LDFLAGS =`libgcrypt-config --libs`
PROCS = hash symmetric pbkdf

all: ${PROCS}
	@echo "just for file test" > hash_test.txt

%: %.o
	gcc ${LDFLAGS} -o $@ $<
%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} ${TRACE} *.o hash_test.txt
