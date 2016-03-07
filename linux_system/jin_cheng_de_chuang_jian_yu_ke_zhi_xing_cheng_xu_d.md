# 進程的創建與可執行程序的加載


## 1.進程管理
Linux中的進程主要由kernel來管理。系統調用是應用程序與內核交互的一種方式。系統調用作為一種接口，通過系統調用，應用程序能夠進入操作系統內核，從而使用內核提供的各種資源，比如操作硬件，開關中斷，改變特權模式等等。
常見的系統調用：`exit,fork,read,write,open,close,waitpid,execve,lseek,getpid...`

###用戶態和內核態
為了使操作系統提供一個很好的進程抽象，限制一個程序可以執行的指令和可以訪問的地址空間。

處理器通常是使用某個控制寄存器中的一個模式位來提供這種功能，該寄存器描述了進程當前享有的特權。當設置了模式位時，進程就運行在內核態，可以執行指令集中的任何指令，並且可以訪問系統中任何存儲器位置。
沒有設置模式位時，進程就運行在用戶態，不允許執行特權指令，也不允許直接引用地址空間中內核區內的代碼和數據。任何這樣的嘗試都會導致致命的保護故障，反之，用戶程序必須通過系統調用接口間接地訪問內核代碼和數據。

關於fork的分析，參見這篇博文。


###waitpid

首先來瞭解一下殭屍進程，當一個進程由於某種原因終止時，內核並不是立即把它從系統中清除。相反，進程被保存在一種已終止的狀態中，直到它的夫進程回收。當父進程回收已終止的子進程時，內核將子進程的退出狀態傳遞給父進程，然後拋棄已終止的進程，從此時開始，該進程就不存在了。一個終止了但還未被回收的進程稱為殭屍進程。

如果父進程沒有回收子進程就終止了，子進程就成了殭屍進程，即時沒有運行，但仍然消耗系統的存儲器資源。

一個進程可以通過調用waitpid函數來等待它的子進程終止或是停止。
函數原型如下:
```sh
pid_t waitpid(pid_t pid, int *status, int options)
```

如果成功，則為子進程的PID，如果WNOHANG，則為0，如果其他錯誤，則為-1.
看一個waitpid函數的例子。

```c
#include"csapp.h"
#include<errno.h>
#define N 5
int main()
{
    int status, i;
    pid_t pid;

    for (i = 0; i < N; i++) {
        if ((pid = Fork()) == 0) {
            exit(100 + i);
        }
    }

    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status)) {
            printf("Child %d exited normally with status=%d!\n", pid, WIFEXITED(status));
        } else {
            printf("Child %d terminated abnormally!\n", pid);
        }
    }

    if (errno != ECHILD) {
        unix_error("waitpid error\n");
    }

    return 1;
}
```

運行結果


![](./images/20130520154759400)

waitpid的第一個參數是-1，則等待集合由父進程的所有子進程組成。大於0的話就是等待進程的pid。 

waitpid的第三個參數是-1，則waitpid會掛起調用進程的執行，直到它的等待集合的一個子進程終止。如果等待集合中的一個進程終止了，那麼waitpid就立即返回。

程序運行的結果就是waitpid函數不按照特定的順序回收僵死的子進程。

提一下wait函數，它就是waitpid函數的簡單版本，原型如下：

```sh
pid_t wait(int *status)
```

等價於waitpid(-1, &status, 0)

### execve
在Linux中要使用exec函數族來在 一個進程中啟動另一個程序。系統調用execve（）對當前進程進行替換，替換者為一個指定的程序，其參數包括文件名（filename）、參數列表（argv）以及環境變量（envp）。exec函數族當然不止一個，但它們大致相同，在 Linux中，它們分別是`：execl，execlp，execle，execv，execve和execvp`，下面我只以execve為例，其它函數究竟與execlp有何區別，請通過man exec命令來瞭解它們的具體情況。


一個進程一旦調用exec類函數，它本身就"死亡"了，系統把代碼段替換成新的程序的代碼，廢棄原有的數據段和堆棧段，併為新程序分配新的數據段與堆棧段，唯一留下的，就是進程號，也就是說，對系統而言，還是同一個進程，不過已經是另一個程序了。（不過exec類函數中有的還允許繼承環境變量之類的信息。）
原型如下：

```sh
int execve(const char *filename, const char *argv[], const char *envp[]);
```

成功調用不會返回，出錯返回-1.
execve函數加載並運行可執行目標文件filename,且帶參數列表argv和環境變量列表envp.只有當出現錯誤時，例如找不到filename，execve才會返回到調用程序。所以，與fork一次調用返回兩次，execve調用一次並從不返回。
argv的在內存中組織方式如下圖：

![](./images/20130520193450965)

argv[0]是可執行目標文件的名字。
envp的在內存中組織方式如下圖：


![](./images/20130520193542916)

環境變量的列表是由一個和指針數組類似的數據結構表示，envp變量指向一個以null結尾的指針數組，其中每個指針指向一個環境變量串，其中每個串都是形如“NAME=VALUE”的鍵值對。
可以用下面的命令來打印命令行參數和環境變量：

```c
#include"csapp.h"

int main(int argc, char* argv[], char* envp[])
{
    int i;

    printf("Command line arguments:\n");

    for (i = 0; argv[i] != NULL; i++) {
        printf("argv[%2d]: %s\n", i, argv[i]);
    }

    printf("\n");
    printf("Environment variables:\n");

    for (i = 0; envp[i] != NULL; i++) {
        printf("envp[%2d]: %s\n", i, envp[i]);
    }

    exit(0);
}
```

![](./images/20130520200402518)

##2.簡單的shell

結合上面的fork，wait和exec，下面來實現一個簡單shell。
先搭建一個shell框架，步驟是讀取一個來自用戶的命令行，求值並解析命令行。

```c
#include<stdio.h>
#include"csapp.h"
#define MAXARGS 128
void eval(char* cmdline);
int parseline(char* buf, char** argv);
int builtin_command(char** argv);

int main()
{
    char cmdline[MAXLINE];

    while (1) {
        printf("> ");
        Fgets(cmdline, MAXLINE, stdin);

        if (feof(stdin)) {
            exit(0);
        }

        eval(cmdline);
    }

    //printf("Hello\n");
    return 1;
}

int builtin_command(char** argv)
{
    if (!strcmp(argv[0], "quit")) {
        exit(0);
    }

    if (!strcmp(argv[0], "&")) {
        return 1;
    }

    if (!strcmp(argv[0], "-help")) {
        printf("-help    help infomation.\n");
        printf("ls       list files and folders of current path.\n");
        printf("pwd      show current path.\n");
        return 1;
    }

    if (!strcmp(argv[0], "pwd")) {
        printf("%s\n", getcwd(NULL, 0));
        return 1;
    }

    return 0;
}

void eval(char* cmdline)
{
    char* argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if (argv[0] == NULL) {
        return;
    }

    if (!builtin_command(argv)) {
        if ((pid = Fork()) == 0) {
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s:Command not found.\n", argv[0]);
                exit(0);
            }
        }

        if (!bg) {
            int status;

            if (waitpid(pid, &status, 0) < 0) {
                unix_error("waitfg:waitpid error");
            }
        } else {
            printf("%d %s", pid, cmdline);
        }
    }

    return;
}

int parseline(char* buf, char** argv)
{
    char* delim;
    int argc;
    int bg;
    buf[strlen(buf) - 1] = ' ';

    while (*buf && (*buf == ' ')) {
        buf++;
    }

    argc = 0;

    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;

        while (*buf && (*buf == ' ')) {
            buf++;
        }
    }

    argv[argc] = NULL;

    if (argc == 0) {
        return 1;
    }

    bg = (*argv[argc - 1] == '&');

    if (bg != 0) {
        argv[--argc] = NULL;
    }

    return bg;
}
```
解釋一下代碼。

主要的幾個函數：
```sh
eval：解釋收到的命令。
parseline：解析以空格分隔的命令行參數，並構造argv傳遞給execve，執行相應的程序。
builtin_command:  檢測參數是否為shell的內建命令，如果是，就立即解釋這個命令，並返回1，否則返回0.
```

下面用通過一些System Call，實現幾個linux的常用命令。

###ls
顯示當前路徑下的文件和文件夾信息。
c代碼實現

```c
#include<stdio.h>
#include<time.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<pwd.h>
#include<grp.h>
void do_ls(char[]);
void dostat(char*);
void show_file_info(char*, struct stat*);
void mode_to_letters(int, char[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);

void main(int argc, char* argv[])
{
    if (argc == 1) {
        do_ls(".");
    } else {
        printf("Error input\n");
    }
}

void do_ls(char dirname[])
{
    DIR* dir_ptr;   //Path
    struct dirent* direntp;     //Struct to save next file node

    if ((dir_ptr = opendir(dirname)) == 0) {
        fprintf(stderr, "ls:cannot open %s\n", dirname);
    } else {
        while ((direntp = readdir(dir_ptr)) != 0) {
            dostat(direntp->d_name);
        }

        closedir(dir_ptr);
    }
}

void dostat(char* filename)
{
    struct stat info;

    if (lstat(filename, &info) == -1) {
        perror("lstat");
    } else {
        show_file_info(filename, &info);
    }
}

void show_file_info(char* filename, struct stat* info_p)
{
    char modestr[11];
    mode_to_letters(info_p->st_mode, modestr);
    printf("%-12s", modestr);
    printf("%-4d", (int)info_p->st_nlink);
    printf("%-8s", uid_to_name(info_p->st_uid));
    printf("%-8s", gid_to_name(info_p->st_gid));
    printf("%-8ld", (long)info_p->st_size);
    time_t timelong = info_p->st_mtime;
    struct tm* htime = localtime(&timelong);
    printf("%-4d-%02d-%02d %02d:%02d", htime->tm_year + 1990, htime->tm_mon + 1,
           htime->tm_mday, htime->tm_hour, htime->tm_min);
    printf(" %s\n", filename);
}

//cope with permission
void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");

    if (S_ISDIR(mode)) {
        str[0] = 'd';
    }

    if (S_ISCHR(mode)) {
        str[0] = 'c';
    }

    if (S_ISBLK(mode)) {
        str[0] = 'b';
    }

    if (mode & S_IRUSR) {
        str[1] = 'r';
    }

    if (mode & S_IWUSR) {
        str[2] = 'w';
    }

    if (mode & S_IXUSR) {
        str[3] = 'x';
    }

    if (mode & S_IRGRP) {
        str[4] = 'r';
    }

    if (mode & S_IWGRP) {
        str[5] = 'w';
    }

    if (mode & S_IXGRP) {
        str[6] = 'x';
    }

    if (mode & S_IROTH) {
        str[7] = 'r';
    }

    if (mode & S_IWOTH) {
        str[8] = 'w';
    }

    if (mode & S_IXOTH) {
        str[9] = 'x';
    }
}

//transfor uid to username
char* uid_to_name(uid_t uid)
{
    struct passwd* pw_str;
    static char numstr[10];

    if ((pw_str = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    } else {
        return pw_str->pw_name;
    }
}

//transfor gid to username
char* gid_to_name(gid_t gid)
{
    struct group* grp_ptr;
    static char numstr[10];

    if ((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    } else {
        return grp_ptr->gr_name;
    }
}
```
實現思路：

主要是do_ls函數，通過opendir命令打開文件夾，然後用readdir來讀取文件夾中的文件或文件夾，輸出信息。

通過剛才的shell調用編譯好ls程序，效果如下：


![](./images/20130526201730023)

## 3.信號
軟中斷信號（signal，又簡稱為信號）用來通知進程發生了異步事件。進程之間可以互相通過系統調用kill發送軟中斷信號。內核也可以因為內部事件而給進程發送信號，通知進程發生了某個事件。注意，信號只是用來通知某進程發生了什麼事件，並不給該進程傳遞任何數據。

收到信號的進程對各種信號有不同的處理方法。處理方法可以分為三類：第一種是類似中斷的處理程序，對於需要處理的信號，進程可以指定處理函數，由該函數來處 理。第二種方法是，忽略某個信號，對該信號不做任何處理，就象未發生過一樣。第三種方法是，對該信號的處理保留系統的默認值，這種缺省操作，對大部分的信 號的缺省操作是使得進程終止。進程通過系統調用signal來指定進程對某個信號的處理行為。 比如一個進程可以通過向另一個進程發送SIGKILL信號強制終止它。當一個子進程終止或者停止時，內核會發送一個SIGCHLD給父進程。

信號有很多種，每種信號類型都對應於某種系統事件。信號的處理流程如下：

![](./images/20130521153743876)

定義信號的接受處理函數原型如下:

```c
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
Returns: ptr to previous handler if OK, SIG_ERR on error (does not set errno)
```

看一個接受信號的例子:


```c
#include "csapp.h"

/* SIGINT handler */
void handler(int sig)
{
    return; /* Catch the signal and return */
}

unsigned int snooze(unsigned int secs)
{
    unsigned int rc = sleep(secs);
    printf("Slept for %u of %u secs.\n", secs - rc, secs);
    return rc;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <secs>\n", argv[0]);
        exit(0);
    }

    if (signal(SIGINT, handler) == SIG_ERR) { /* Install SIGINT handler */
        unix_error("signal error\n");
    }

    (void)snooze(atoi(argv[1]));
    exit(0);
}
```

程序解析：

程序接受一個int參數，用於設置sleep的秒數，正常情況下sleep相應的秒數之後就自動退出程序，由於註冊了SIGINI，當按下鍵盤的Ctrl+C鍵的時候，跳轉到handler函數，處理信號。

## 4.動態鏈接和靜態鏈接
庫有動態與靜態兩種，動態通常用.so為後綴，靜態用.a為後綴。例如：libhello.so libhello.a 
為了在同一系統中使用不同版本的庫，可以在庫文件名後加上版本號為後綴,例如： libhello.so.1.0,由於程序連接默認以.so為文件後綴名。所以為了使用這些庫，通常使用建立符號連接的方式。

```sh
ln -s libhello.so.1.0 libhello.so.1 
ln -s libhello.so.1 libhello.so 
```

使用庫 
當 要使用靜態的程序庫時，連接器會找出程序所需的函數，然後將它們拷貝到執行文件，由於這種拷貝是完整的，所以一旦連接成功，靜態程序庫也就不再需要了。然 而，對動態庫而言，就不是這樣。動態庫會在執行程序內留下一個標記‘指明當程序執行時，首先必須載入這個庫。由於動態庫節省空間，linux下進行連接的 缺省操作是首先連接動態庫，也就是說，如果同時存在靜態和動態庫，不特別指定的話，將與動態庫相連接。

##5.ELF文件格式與進程地址空間的聯繫

進程地址空間中典型的存儲區域分配情況如下：

![](./images/20130526195532582)

從圖中可以看出：

從低地址到高地址分別為：代碼段、（初始化）數據段、（未初始化）數據段（BSS）、堆、棧、命令行參數和環境變量
堆向高內存地址生長
棧向低內存地址生長

對於ELF文件，一般有下面幾個段

```sh
.text section：主要是編譯後的源碼指令，是隻讀字段。
.data section ：初始化後的非const的全局變量、局部static變量。
.bss：未初始化後的非const全局變量、局部static變量。
.rodata：是存放只讀數據 
```

關於ELF的文件的只是這裡就不贅述了。

在ELF文件中，使用section和program兩種結構描述文件的內容。通常來說，ELF可重定位文件採用section，ELF可執行文件使用program，可重鏈接文件則兩種都用。
裝載文件，其實是一個很簡單的過程，通過section或者program中的type屬性判斷是否需要加載，然後通過offset屬性找到文件中的數據，將它讀取（複製）到相應的內存位置就可以了。 這個位置，可以通過program裡面的vaddr屬性確定；對於section來說，則可以自己定義裝載的位置。

動態連接的本質，就是對ELF文件進行重定位和符號解析。
重定位可以使得ELF文件可以在任意的執行（普通程序在鏈接時會給定一個固定執行地址）；符號解析，使得ELF文件可以引用動態數據（鏈接時不存在的數據）。
從流程上來說，我們只需要進行重定位。而符號解析，則是重定位流程的一個分支。

## 6.參考
程序員的自我修養—鏈接、裝載與庫 
  
Computer Systems: A Programmer's Perspective  3rd Edith

Linux內核編程

understanding the kernel  3rd Edith