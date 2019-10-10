thirdparty_root := /home/shihyu/.mybin/opencv3.4.7
export PKG_CONFIG_PATH := ${thirdparty_root}/lib/pkgconfig:${PKG_CONFIG_PATH}

CXXINCS :=
CXXINCS += `pkg-config opencv --cflags`

CXXLIBS :=
CXXLIBS += `pkg-config opencv --libs`

CXX = g++
CXXFLAG :=
CXXFLAG += -g -O0 -ggdb3
CXXFLAG += -std=c++11
CXXFLAG += -Wall $(CXXINCS)

LDFLAGS := -lpthread 
LDFLAGS += -Wl,-rpath,${thirdparty_root}/lib

SOURCEDIR = src
BUILDDIR = build
SRCS = $(wildcard $(SOURCEDIR)/*.cpp)
OBJS = $(addprefix $(BUILDDIR)/,$(notdir $(SRCS:.cpp=.o)))

all : dir main 

dir :
	mkdir -p $(BUILDDIR)

main : $(OBJS)
	$(CXX) $(CXXFLAG) -o $@ $(OBJS) $(LDFLAGS) $(CXXLIBS)

$(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAG) -c $< -o $@ $(CXXLIBS) 

debug:
	cgdb main -- -ex "`find /home/shihyu/github/opencv-3.4.7/modules -type d -printf 'dir %p '`" -ex 'b remap'
	
clean:
	rm -rf main $(BUILDDIR)
