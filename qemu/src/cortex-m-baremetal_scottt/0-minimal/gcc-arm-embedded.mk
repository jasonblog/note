HOST := arm-none-eabi
CROSS_COMPILE := $(HOST)-

CC := $(CROSS_COMPILE)gcc
export CC
CXX := $(CROSS_COMPILE)g++
export CXX
LD := $(CROSS_COMPILE)ld
export LD
AS := $(CROSS_COMPILE)as
export AS
READELF := $(CROSS_COMPILE)readelf
export READELF
NM := $(CROSS_COMPILE)nm
export NM
OBJDUMP := $(CROSS_COMPILE)objdump
export OBJDUMP
GDB := $(CROSS_COMPILE)gdb
export GDB
OBJCOPY := $(CROSS_COMPILE)objcopy
export OBJCOPY
AR := $(CROSS_COMPILE)ar
export AR
