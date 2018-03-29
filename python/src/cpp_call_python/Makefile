CC = g++
LDFLAGS = -lm -lpthread -ldl -lutil -lpython3.6m 
CFLAGS = -Wall -pedantic -ggdb3 -O0
INCLUDE = -I/home/shihyu/anaconda3/include/python3.6m
LIB = -L/home/shihyu/anaconda3/lib

SOURCEDIR = ./
BUILDDIR = build
SRCS = $(wildcard $(SOURCEDIR)/*.cpp)
OBJS = $(addprefix $(BUILDDIR)/,$(notdir $(SRCS:.cpp=.o)))

all : dir process 

dir :
	mkdir -p $(BUILDDIR)

process : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LIB) $(LDFLAGS)

$(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@
 
clean:
	rm -rf process hello.pyc $(BUILDDIR)

