CC      =  gcc
CFLAGS  =  -pedantic -Wall -std=c11 -D_BSD_SOURCE -g
TARGET  =  mallocs.out

OBJS    =  main.o malloc_two.o

$(TARGET): $(OBJS)
	@echo "linking ... $(TARGET)"
	@$(CC)  $^ -o $@

relink:
	@echo "relinking ... $(TARGET)"
	@$(CC)  $(CFLAGS)  -o $(TARGET)  $(OBJS)  $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET)
