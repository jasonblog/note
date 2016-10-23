set confirm 0
target remote :1234
set disassemble-next-line 1
file minimal.elf
load
monitor system_reset

break minimal.c:10
commands
silent
end

continue
printf "X: %d\n", x
printf "X should equal 17: %d\n", x == 17
quit
