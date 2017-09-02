CC = gcc
LDFLAGS = -lm -lpthread
CFLAGS = -Wall -pedantic -ggdb3 -O0 -std=c99

SOURCEDIR = Float
BUILDDIR = build
SRCS = $(wildcard $(SOURCEDIR)/*.c)
OBJS = $(addprefix $(BUILDDIR)/,$(notdir $(SRCS:.c=.o)))

all : dir cb 

dir :
	mkdir -p $(BUILDDIR)

cb : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS)

$(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
 
clean:
	rm -rf cb $(BUILDDIR)
