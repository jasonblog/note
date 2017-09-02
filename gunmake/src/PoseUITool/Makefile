#
# Makefile for ubuntu POSE render tool
# HaroldChao 2016/09/09

CC = g++
CXXFLAGS = -I ./src `pkg-config libusb-1.0 --cflags`
#Add for gnuplot support
CXXFLAGS += \
	-I ./external
LDFLAGS = -lGL -lglut -lGLU -pthread `pkg-config libusb-1.0 --libs`
#Add for gnuplot support
LDFLAGS += \
	-lboost_iostreams -lboost_system -lboost_filesystem -g -rdynamic
SOURCEDIR = src
BUILDDIR = build
SRCS = $(wildcard $(SOURCEDIR)/*.cpp)
OBJS = $(addprefix $(BUILDDIR)/,$(notdir $(SRCS:.cpp=.o)))
UTILS_SRCS = $(wildcard $(SOURCEDIR)/utils/*.cpp)
UTILS_OBJS = $(addprefix $(BUILDDIR)/utils/,$(notdir $(UTILS_SRCS:.cpp=.o)))
TEST_SRCS = $(wildcard $(SOURCEDIR)/test/*.cpp)
TEST_OBJS = $(addprefix $(BUILDDIR)/test/,$(notdir $(TEST_SRCS:.cpp=.o)))
all : dir UiTool
test : dir testcase

dir :
	mkdir -p $(BUILDDIR)
	mkdir -p $(BUILDDIR)/utils
	mkdir -p $(BUILDDIR)/test

UiTool : $(OBJS) $(UTILS_OBJS)
	$(CC) -o Cube $(CXXFLAGS) $(OBJS) $(UTILS_OBJS) $(LDFLAGS)

testcase : $(UTILS_OBJS) $(TEST_OBJS)
	$(CC) -o testcase $(CXXFLAGS) $(UTILS_OBJS) $(TEST_OBJS) $(LDFLAGS)

$(BUILDDIR)/%.o : src/%.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf Cube testcase $(BUILDDIR)
