# 好用 Makefile

```sh
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

```


## opencv 
```sh
CXX = g++
CXXFLAG :=
CXXFLAG += -g -O0 -ggdb3
CXXFLAG += -std=c++11
CXXFLAG += -Wall

CXXLIBS :=
CXXLIBS += `pkg-config opencv --libs`
CXXINCS :=
CXXINCS += `pkg-config opencv --cflags`


SOURCEDIR = src
BUILDDIR = build
SRCS = $(wildcard $(SOURCEDIR)/*.cpp)
OBJS = $(addprefix $(BUILDDIR)/,$(notdir $(SRCS:.cpp=.o)))

all : dir main 

dir :
	mkdir -p $(BUILDDIR)

main : $(OBJS)
	$(CXX) -o $@ $(CXXFLAG) $(OBJS) $(CXXINCS) $(CXXLIBS)

$(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAG) -c $< -o $@ $(CXXINCS) $(CXXLIBS) 

clean:
	rm -rf main $(BUILDDIR)
```