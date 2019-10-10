# Opencv Makefile

## 使用 export & rpath

```sh
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
```


---

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

### Makefile 標準寫法

```sh
thirdparty_root := /usr/local/thirdparty
facesdk_root := /usr/local/facesdk
export PKG_CONFIG_PATH := ${thirdparty_root}/lib64/pkgconfig:${PKG_CONFIG_PATH}

CC := g++
INCS += `pkg-config --cflags opencv`
INCS += -I${facesdk_root}/include
INCS += -I${thirdparty_root}/include
INCS += -I${thirdparty_root}/include/apr-1


LIBS += `pkg-config --libs opencv`
LIBS +=	-L${thirdparty_root}/lib -lglog -lactivemq-cpp -lapr-1
LIBS +=	-L${facesdk_root}/lib -lrr_faceverify_t -lrr_facedetect_t

AFLAGS := -Wall -c -fPIC -O2 -std=c++11 $(INCS)
LDFLAGS := -lpthread -lcurl
LDFLAGS += -Wl,-rpath,${thirdparty_root}/lib64:${thirdparty_root}/lib:${facesdk_root}/lib

OBJS := $(patsubst %.cpp,%.o,$(shell find . -name "*.cpp"))


FaceFilter: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

$(OBJS): %.o:%.cpp
	$(CC) $(AFLAGS) $^ -o $@

debugOBJS:
	@echo ${OBJS}

.PHONY: clean FaceFilter setenv

clean:					
	rm -rf *.o FaceFilter baseutils/*.o factory/*.o message/*.o servicemodules/*.o
```