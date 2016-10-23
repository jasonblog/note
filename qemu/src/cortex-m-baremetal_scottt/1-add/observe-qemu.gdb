target remote :1234

file add.elf
load
monitor system_reset
delete

break add.c:14
commands
printf "x: %d, y: %d, z: %d\n", x, y, z
end

continue
quit
