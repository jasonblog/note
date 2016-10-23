target remote :1234

set disassemble-next-line 1
file add.elf
load
monitor system_reset
delete

break main
continue

disassemble /m main
printf "\nHINT: Use \"stepi\" to observe the ADD operation\n"
