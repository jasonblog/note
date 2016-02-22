# 使用system()函數執行命令行命令簡述

```c
#include<stdlib.h>
#include <process.h>
```

函數原型：int system(const char *command)

system函數可以調用一些DOS或者命令行命令,比如

system("cls");//清屏,等於在DOS上使用cls命令

system(“ls -al”);//清屏,等於在unix上使用ls -al命令

成功了返回0 失敗了返回-1

需要注意的是：該函數對字符串的處理和c中的習慣一致，比如說：如果輸出反斜槓需要 \\
例如:
system("dir c:\\windows");
注意轉義字符等的限制就可以了


system()函數功能強大，很多人用卻對它的原理知之甚少先看linux版system函數的源碼：

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

int system(const char* cmdstring)
{
    pid_t pid;
    int status;

    if (cmdstring == NULL) {
        return (1);
    }

    if ((pid = fork()) < 0) {
        status = -1;
    } else if (pid = 0) {
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        -exit(127); //子進程正常執行則不會執行此語句
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTER) {
                status = -1;
                break;
            }
        }
    }

    return status;
}


```
先分析一下原理，然後再看上面的代碼大家估計就能看懂了：  

當system接受的命令為NULL時直接返回，否則fork出一個子進程，因為fork在兩個進程：父進程和子進程中都返回，這裡要檢查返回的pid，fork在子進程中返回0，在父進程中返回子進程的pid，父進程使用waitpid等待子進程結束，子進程則是調用execl來啟動一個程序代替自己，execl("/bin/sh", "sh", "-c", cmdstring, (char*)0)是調用shell，這個shell的路徑是/bin/sh，後面的字符串都是參數，然後子進程就變成了一個shell進程，這個shell的參數
是cmdstring，就是system接受的參數。在windows中的shell是command，想必大家很熟悉shell接受命令之後做的事了。
   
如果上面的你沒有看懂，那我再解釋下fork的原理：當一個進程A調用fork時，系統內核創建一個新的進程B，並將A的內存映像複製到B的進程空間中，因為A和B是一樣的，那麼他們怎麼知道自己是父進程還是子進程呢，看fork的返回值就知道，上面也說了fork在子進程中返回0，在父進程中返回子進程的pid。

windows中的情況也類似，就是execl換了個又臭又長的名字，參數名也換的看了讓人發暈的，我在MSDN中找到了原型，給大家看看：

```c
HINSTANCE   ShellExecute(
               HWND   hwnd,
               LPCTSTR   lpVerb,
               LPCTSTR   lpFile,
               LPCTSTR   lpParameters,
               LPCTSTR   lpDirectory, 
               INT   nShowCmd 
   );   
```

用法見下：

```c
ShellExecute(NULL,   "open",   "c:\\a.reg",   NULL,   NULL,   SW_SHOWNORMAL);  
```

你也許會奇怪 ShellExecute中有個用來傳遞父進程環境變量的參數 lpDirectory，linux中的 execl卻沒有，這是因為execl是編譯器的函數（在一定程度上隱藏具體系統實現），在linux中它會接著產生一個linux系統的調用 execve, 原型見下：

```c
int execve(const char * file,const char **argv,const char **envp);
```

看到這裡你就會明白為什麼system（）會接受父進程的環境變量，但是用system改變環境變量後，system一返回主函數還是沒變。原因從system的實現可以看到，它是通過產生新進程實現的，從我的分析中可以看到父進程和子進程間沒有進程通信，子進程自然改變不了父進程的環境變量，DOS早死翹翹了，還是玩linux吧。



注： linux下執行系統命令用execl函數：

Linux下頭文件

```c
#include <unistd.h>
int execl(const char *path, const char *arg, ...);
```

例如：
// 執行/bin目錄下的ls, 第一參數為程序名ls, 第二個參數為"-al", 第三個參數為"/etc/passwd"

```c
execl("/bin/ls", "ls", "-al", "/etc/passwd", (char *) 0);
```