#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096

 
int main(int argc, char* argv[]) {
    int inputFd, outputFd;
    ssize_t numIn, numOut;
    char buffer[BUF_SIZE];
    off_t begin=0, end=0;
    int fileSize, blockSize, pos=0;
 
    inputFd = open (argv [1], O_RDONLY); 
    outputFd = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR| S_IWUSR);
    ftruncate(outputFd, 0);
    
    fileSize = lseek(inputFd, 0, SEEK_END);
    lseek(inputFd, 0, SEEK_SET);

	while (1) {
		pos = lseek(inputFd, pos, SEEK_DATA);
		begin = pos;
		pos = lseek(inputFd, pos, SEEK_HOLE);
		end = pos;
		blockSize=end-begin;
		lseek(inputFd, begin, SEEK_SET);
		lseek(outputFd, begin, SEEK_SET);
		while((numIn = read (inputFd, buffer, BUF_SIZE)) > 0) {
			numOut = write (outputFd, buffer, (ssize_t) numIn);
			if (numIn != numOut) perror("numIn != numOut");
			blockSize-=numIn;
			if (blockSize == 0) break;
		}
		if (lseek(outputFd, 0, SEEK_CUR) == fileSize) break;
    }
    close (inputFd);
    close (outputFd);
 
    return (EXIT_SUCCESS);
}

