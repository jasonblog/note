# linux系統編程之文件與IO（八）：文件描述符相關操作-dup,dup2,fcntl


本節目標：

###1,文件共享
- 打開文件內核數據結構
- 一個進程兩次打開同一個文件
- 兩個進程打開同一個文件

###2,複製文件描述符（dup、dup2、fcntl）

##一，文件共享

1，一個進程打開兩個文件內核數據結構

![](./images/mickole/11163923-ebe087572f6b4f17ae31aa12e82f656a.png)

說明：

`文件描述符表`：每個進程都有一張，彼此獨立，每個文件描述符表項都指向一個文件表，文件描述符0（STDIN_FILENO）、1（STDOUT_FILENO）、2（STDERR_FILENO）,默認已經打開，分別表示：標準輸入，標準輸出，標準錯誤設備。

`文件表`：每打開一個文件就對應一張文件表，文件表可以共享，當多個文件描述符指向同一個文件表時，文件表中的

refcnt字段會相應變化。文件狀態標識：文件的打開模式（R,W,RW,APPEND,NOBLOCK,等），當前文件偏移量，refcnt：被引用數量，

`v節點指針`：指向一個v節點表。

v節點表：每個文件對應一個，無論被被多少個進程打開都只有一個，它包括v節點信息（主要是stat結構體中的信息），i節點信息。

每個進程默認只能打開1024個文件描述符，當一個進程打開一個文件時，默認會從0開始查找未被使用的描述符，由於0,1,2默認被佔用，所有一般從3開始使用。


2、一個進程兩次打開同一個文件

![](./images/mickole/11163924-088c225b47f945b7b5abbfdf20b40426.png)

當一個進程多次打開同一個文件時，首先會在描述符表順序查找未被使用的描述符，然後每打開一次建立一張文件表，但各文件表中的v節點指針都指向同一個v節點表。

示例程序：

```c
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

int main(int argc, char *argv[])
{
    int fd1;
    int fd2;
    char buf1[1024] = {0};
    char buf2[1024] = {0};
    fd1 = open("test.txt", O_RDONLY);
    if (fd1 == -1)
        ERR_EXIT("open error");
    read(fd1, buf1, 5);
    printf("buf1=%s\n", buf1);
    

    fd2 = open("test.txt", O_RDWR);
    if (fd2 == -1)
        ERR_EXIT("open error");

    read(fd2, buf2, 5);
    printf("buf2=%s\n", buf2);
    write(fd2, "AAAAA", 5);

    memset(buf1, 0, sizeof(buf1));
    read(fd1, buf1, 5);
    printf("buf1=%s\n", buf1);
    close(fd1);
    close(fd2);
    return 0;
}
```
運行結果：

![](./images/mickole/11163925-0019ae61065e45459e152726db86cbb9.png)

說明：先創建test.txt文件寫入hello，再同一個進程兩次打開該文件，可見每打開一次文件就參數一張文件表，不共享偏移量，都開始位置讀取，之後利用第二個文件描述符寫入AAAAA，在利用第一個描述符可以讀取出，表明都指向同一個v節點表，操作同一個文件。

3，兩個不同進程打開同一個文件

![](./images/mickole/11163926-21b18b14712c4df48c5d71d079bb6c15.png)

當不同進程打開同一個文件時，每個進程首先在它們各自的文件描述符表中順序查找未被使用的描述符，最終獲得的文件描述符可能相同也可能不同，每個fd指向各自的文件表，但同樣，每個文件表中的v節點指針都指向同一個v節點表。

##二、複製文件描述符

複製前：


![](./images/mickole/11163927-606511098dea49e8b9578e9c39ef9230.png)

複製後：

![](./images/mickole/11163928-414d203e8ad846a0bb276258c9388148.png)

複製後，兩個文件描述符都指向了同一個文件表，refcnt=2。

複製文件描述符有`三種`方法：

- 1，dup
- 2，dup2
- 3，fcntl

```c
#include <unistd.h>

int dup(int oldfd); 
int dup2(int oldfd, int newfd);
```

```c
DESCRIPTION 
       These system calls create a copy of the file descriptor oldfd.

       dup()  uses  the lowest-numbered unused descriptor for the new descriptor.

       dup2() makes newfd be the copy of oldfd, closing newfd first if  necessary, but note

                  the following:

       *  If  oldfd  is  not a valid file descriptor, then the call fails, and newfd is not closed.

       *  If oldfd is a valid file descriptor, and newfd has the same value as 
          oldfd, then dup2() does nothing, and returns newfd.

       After  a  successful return from one of these system calls, the old and new file descriptors may be used interchangeably.  They  refer  to  the same open file description (see open(2)) and thus share file offset and file status flags; for example, if the file offset is modified by using lseek(2)  on one of the descriptors, the offset is also changed for the other.

RETURN VALUE 
       On success, these system calls return the new descriptor.  On error, -1 is returned, and errno is set appropriately.
```

示例程序：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


int main(void)
{
    int fd;
    fd = open("test.txt",O_WRONLY);
    if( fd == -1){
        perror("open error");
        exit(EXIT_FAILURE);
    }
    int fd2;
    fd2 = dup(fd);
    if(fd2 == -1){
        perror("dup error");
        exit(EXIT_FAILURE);
    }
    printf("oldfd = %d\n",fd);
    printf("newfd = %d\n",fd2);
    int fd3;
    close(0); //或者close(STDIN_FILENO)
    fd3 = dup(fd);
    if(fd3 == -1){
        perror("dup error");
        exit(EXIT_FAILURE);
    }
    printf("after close stdin,the newfd = %d\n",fd3); 

    exit(EXIT_SUCCESS);
}
```
運行結果：

![](./images/mickole/11163929-d8a5e3c548884b71ac8b934657ad90a3.png)

用dup進行文件描述符的複製時，順序查找即從0開始查找可以文件描述符 
示例2：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
int main(void)
{
    int fd;
    fd = open("test.txt",O_WRONLY);
    if( fd == -1){
        perror("open error");
        exit(EXIT_FAILURE);
    }
    int fd2;
    fd2 = dup2(fd,0);
    if(fd2 == -1){
        perror("dup error");
        exit(EXIT_FAILURE);
    }
    printf("oldfd = %d\n",fd);
    printf("newfd = %d\n",fd2);
    exit(EXIT_SUCCESS);
}
```
運行結果：


![](./images/mickole/11163930-47a991995a80417d9259a1fb5d6a7d88.png)

用dup2進行文件描述符複製時，指定需要複製的新的描述符，如果該描述符已經被佔用，則先關閉它在重新複製，類似於先調用close再dup

- 3，fcntl
- 
功能：操縱文件描述符，改變已打開的文件的屬性

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, ... /* arg */ );
```

```c
DESCRIPTION 
       fcntl() performs one of the operations described below on the open file 
       descriptor fd.  The operation is determined by cmd.

       fcntl() can take an optional third argument.  Whether or not this argu- 
       ment  is  required is determined by cmd.  The required argument type is 
       indicated in parentheses after  each  cmd  name  (in  most  cases,  the 
       required  type  is  long,  and  we identify the argument using the name 
       arg), or void is specified if the argument is not required.

```

由第二個參數指定操作類型，後面點的可變參數指定該命令所需的參數

這裡我們進行文件描述符複製，可將cmd 設為： F_DUPFD (long)，該命令表示：

`Find the lowest numbered available file descriptor greater  than 
  or  equal to arg` and make it be a copy of fd.  This is different 
from dup2(2), which uses exactly the descriptor specified.

  On success, the new descriptor is returned.

示例程序：
```c
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

int main(int argc, char *argv[])
{
    int fd;
    fd = open("test.txt", O_WRONLY);
    if (fd == -1)
        ERR_EXIT("open error");

    close(1);
    if (fcntl(fd, F_DUPFD, 0) < 0)
        ERR_EXIT("dup fd error");
    printf("-----------\n");//進行重定向，將不會顯示在標準輸出，
    return 0;
}
```
運行結果：

![](./images/mickole/11163931-d6cf796e70dd4a84b8f46504f9f4c7d1.png)
