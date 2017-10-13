EXEC = \
    tests/test-matrix \
    tests/test-stopwatch

GIT_HOOKS := .git/hooks/applied
OUT ?= .build
.PHONY: all
all: $(GIT_HOOKS) $(OUT) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

CC ?= gcc
CFLAGS = -Wall -std=gnu99 -g -O2 -I.
LDFLAGS = -lpthread

OBJS := \
	stopwatch.o \
	matrix_naive.o

OBJS := $(addprefix $(OUT)/,$(OBJS))
deps := $(OBJS:%.o=%.o.d)

tests/test-%: $(OBJS) tests/test-%.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OUT)/%.o: %.c $(OUT)
	$(CC) $(CFLAGS) -c -o $@ -MMD -MF $@.d $<

$(OUT):
	@mkdir -p $@

check: $(EXEC)
	@for test in $^ ; \
	do \
		echo "Execute $$test..." ; $$test && echo "OK!\n" ; \
	done

clean:
	$(RM) $(EXEC) $(OBJS) $(deps)
	@rm -rf $(OUT)

-include $(deps)
