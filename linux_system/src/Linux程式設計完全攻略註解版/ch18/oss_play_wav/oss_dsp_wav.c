#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<stdio.h>
#include<linux/soundcard.h>

#define Audio_Device "/dev/dsp"

#define Sample_Size 16      //there're two kinds of bits,8 bits and 16 bits
#define Sample_Rate 44100       //sampling rate

int play_sound(char* filename)
{
    struct stat stat_buf;
    unsigned char* buf = NULL;
    int handler, fd;
    int result;
    int arg, status;

    fd = open(filename, O_RDONLY);

    if (fd < 0) {
        return -1;
    }

    if (fstat(fd, &stat_buf)) {
        close(fd);
        return -1;
    }

    if (!stat_buf.st_size) {
        close(fd);
        return -1;
    }

    buf = malloc(stat_buf.st_size);

    if (!buf) {
        close(fd);
        return -1;
    }

    if (read(fd, buf, stat_buf.st_size) < 0) {
        free(buf);
        close(fd);
        return -1;
    }

    handler = open(Audio_Device, O_WRONLY);

    if (handler == -1) {
        perror("open Audio_Device fail");
        return -1;
    }

    arg = Sample_Rate;
    status = ioctl(handler, SOUND_PCM_WRITE_RATE, &arg);

    if (status == -1) {
        perror("error from SOUND_PCM_WRITE_RATE ioctl");
        return -1;
    }

    arg = Sample_Size;
    status = ioctl(handler, SOUND_PCM_WRITE_BITS, &arg);

    if (status == -1) {
        perror("error from SOUND_PCM_WRITE_BITS ioctl");
        return -1;
    }

    result = write(handler, buf, stat_buf.st_size);

    if (result == -1) {
        perror("Fail to play the sound!");
        return -1;
    }

    free(buf);
    close(fd);
    close(handler);
    return result;
}

void main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("pls usage :%s file.wav\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    play_sound(argv[1]);
}


