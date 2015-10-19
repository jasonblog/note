CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
LD = $(CROSS_COMPILE)ld
ARCH = $(CROSS_COMPILE)ar
IMAGE = boot


	      

CFLAGS = \
	-fno-builtin \
	-O0 -g -fomit-frame-pointer -fno-strict-aliasing -mcpu=arm1176jzf-s 

LDSCRIPT := ld-scr.lds
LDFLAGS = \
	-nostdlib -static -nostartfiles \
	-T $(LDSCRIPT) 

OBJCOPYFLAGS = \
	-O binary \
	-S

C_SRC = ./boot.c ./mmu.c ./interrupt.c ./mem.c\
        ./lib/print.c
ASM_SRC = ./start.S 


C_OBJ = $(C_SRC:.c=.o)
HW_DEP_ASM_OBJ = $(ASM_SRC:.S=.o)


$(IMAGE).bin: $(IMAGE).elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(IMAGE).elf: $(C_OBJ) $(HW_DEP_ASM_OBJ) 
	$(CC) $(LDFLAGS) -o $(IMAGE).elf \
	$(C_OBJ) $(HW_DEP_ASM_OBJ) -lgcc

$(C_OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@


$(HW_DEP_ASM_OBJ) : %.o : %.S
	$(CC) -c $(CFLAGS) $< -o $@




clean:
	rm -f ./*.o \
	./lib/*.o \
	$(IMAGE).bin $(IMAGE).elf \
              













	


