#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef unsigned short  u16;
typedef unsigned long long u64;

#define BUFSIZE  4096

struct gdtr_struct{
    short limit;
    unsigned long long address __attribute__((packed));
};
int main(int argc, char **argv)
{
    int fd, ret;
    u16 cs, ds, ss, es, fs, gs;
    char buf[BUFSIZE];

    unsigned long int a = 0x12345678beaf5dde;
    printf("a = 0x%016lX addr: %p \n", a, &a);

    printf("\n----  Segment Registers ----\n");
    asm volatile("mov %%cs, %0" : "=r" (cs));
    asm volatile("mov %%ds, %0" : "=r" (ds));
    asm volatile("mov %%ss, %0" : "=r" (ss));
    asm volatile("mov %%es, %0" : "=r" (es));
    asm volatile("mov %%fs, %0" : "=r" (fs));
    asm volatile("mov %%gs, %0" : "=r" (gs));
    printf("cs :%04x TI = %s Index = %d RPL = %d\n", cs, (cs & 0x04) ? "LDT" : "GDT", cs >> 3, (int)cs & 0x03);
    printf("ds :%04x TI = %s Index = %d RPL = %d\n", ds, (ds & 0x04) ? "LDT" : "GDT", ds >> 3, (int)ds & 0x03);
    printf("ss :%04x TI = %s Index = %d RPL = %d\n", ss, (ss & 0x04) ? "LDT" : "GDT", ss >> 3, (int)ss & 0x03);
    printf("es :%04x TI = %s Index = %d RPL = %d\n", es, (es & 0x04) ? "LDT" : "GDT", es >> 3, (int)es & 0x03);
    printf("fs :%04x TI = %s Index = %d RPL = %d\n", fs, (fs & 0x04) ? "LDT" : "GDT", fs >> 3, (int)fs & 0x03);
    printf("gs :%04x TI = %s Index = %d RPL = %d\n", gs, (gs & 0x04) ? "LDT" : "GDT", gs >> 3, (int)gs & 0x03);

    if ((fd = open("/proc/registers", O_RDONLY)) < 0) {
        fprintf(stderr, "Open /proc/registers file failed! \n");
        exit(EXIT_FAILURE);
    }
    lseek(fd, 0L, SEEK_SET);
    if ((ret = read(fd, buf, sizeof buf - 1)) < 0) {
        perror("/proc/registers");
        exit(EXIT_FAILURE);
    }
    buf[ret] = 0;
    close(fd);
    puts( buf );

    puts( "Press any key to exit" );
    fgetc(stdin);

    return 0;
}

