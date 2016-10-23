set confirm 0
target remote :1234

set disassemble-next-line 1

file hello-semihosting.elf
load
monitor system_reset

break main
continue
