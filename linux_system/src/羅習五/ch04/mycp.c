#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
 
#define BUF_SIZE 4096
 
int main(int argc, char* argv[]) {
 
    int inputFd, outputFd;
    ssize_t numIn, numOut;
    char buffer[BUF_SIZE];
 
    inputFd = open (argv [1], O_RDONLY);
    if (inputFd == -1) {
            perror ("cannot open the file for read"); exit(1); }
 
    outputFd = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR| S_IWUSR);
    if(outputFd == -1){
        perror("canot open the file for write"); exit(1); }
 
    while((numIn = read (inputFd, buffer, BUF_SIZE)) > 0){
            numOut = write (outputFd, buffer, (ssize_t) numIn);
            if(numIn != numOut){ perror("numIn != numOut"); exit(1); }
    }
    close (inputFd); close (outputFd);
    return (EXIT_SUCCESS);
}


