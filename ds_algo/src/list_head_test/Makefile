CC = gcc
TGT = list_head_test
OBJS = list_head_test.o

all: $(TGT)

$(TGT): $(OBJS)
	$(CC) -o $@ $^

test: $(TGT)
	./$(TGT)

.PHONY: clean
clean:
	@rm -rf $(OBJS) $(TGT)
