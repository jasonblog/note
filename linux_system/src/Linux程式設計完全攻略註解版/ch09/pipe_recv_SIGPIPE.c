#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
    if (SIGPIPE == sig) {
        printf("revc SIGPIPE");
    }
}

int main(int argc, char* argv[])
{
    int p[2];
    signal(SIGPIPE, handler);
    pipe(p);
    // 如果當前沒有任何一個 process 可以存取讀取端會, 寫入操作會收到 SIGPIPE 訊號
    close(p[0]);
    int ret = 0;
    ret = write(p[1], "Helloworld", 10);
    printf("ret=%d\n", ret);
}
