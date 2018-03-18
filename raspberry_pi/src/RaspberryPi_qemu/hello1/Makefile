ARMGNU = arm-none-eabi
 
AOPS = --warn --fatal-warnings
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 
boot.bin: boot.asm
	$(ARMGNU)-gcc -O2 -c helloworld.c
	$(ARMGNU)-as boot.asm -o boot.o
	$(ARMGNU)-ld -T linker.ld boot.o helloworld.o -o boot.elf
	$(ARMGNU)-objcopy boot.elf -O binary boot.bin

clean:
	rm *.elf *.o *.bin -f
