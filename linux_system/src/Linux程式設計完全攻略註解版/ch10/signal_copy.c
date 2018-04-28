#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<stdlib.h>

//the copy fie size must>M
int count;              //current copy number
int file_size;          //the file size
void sig_alarm(int arg);
void sig_usr(int sig);

int main(int argc, char* argv[])
{
    pid_t pid;
    int i;
    int fd_src, fd_des;
    char buf[128];      //in order to infirm the problem, buf can set small

    if (argc != 3) {
        printf("check the format:comm src_file des_file\n");
        return -1;
    }

    if ((fd_src = open(argv[1], O_RDONLY)) == -1) {
        perror("open file src");
        exit(EXIT_FAILURE);
    }

    file_size = lseek(fd_src, 0, SEEK_END);
    lseek(fd_src, 0, SEEK_SET);

    if ((fd_des = open(argv[2], O_RDWR | O_CREAT, 0644)) == -1) {
        perror("open fd_fdes");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        signal(SIGUSR1, sig_usr);

        do {
            memset(buf, '\0', 128);

            if ((i = read(fd_src, buf, 1)) == -1) { //the copy number may modify
                perror("read");
                exit(EXIT_FAILURE);
            } else if (i == 0) {
                kill(pid, SIGINT);
                break;
            } else {
                if (write(fd_des, buf, i) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }

                count += i;
                //usleep(1);
            }
        } while (i != 0);

        wait(pid, NULL, 0);
        exit(EXIT_SUCCESS);
    }

    else if (pid == 0) {
        usleep(1);  //wait for parent to install signal

        signal(SIGALRM, sig_alarm);
        ualarm(1, 1);   //if alarm ,in sig_alarm function to install again

        while (1) {
            ;
        }

        exit(EXIT_SUCCESS);
    }

}

void sig_alarm(int arg)
{
    //alarm(1);         //if alarm may add this line
    kill(getppid(), SIGUSR1);
}

void sig_usr(int sig)
{
    float i;
    i = (float)count / (float)file_size;
    printf("curent over :%0.0f%%\n", i * 100);
}


