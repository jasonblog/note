# Userspace I/O scheduling for multi-file operations



在數量眾多的文件讀寫時，例如一個目錄下有400萬個圖片文件，需要將這400萬個圖片文件打包，如果這400萬個文件讀寫順序不對，將會到這磁頭來回seek，即便單文件順序，但由於每個文件較小，因此順序的機會有限，大部分呈現隨機讀的狀態。

linux內核的IO scheduling 是針對單個文件的，有四種調度方式，但缺乏對大量文件的調度，因此有必要在用戶空間進行IO調度。

簡單來說我們有一個目錄D，目錄下有file1，....file400million（文件名），這樣400萬個文件

首先第一步，獲得每個file的首個邏輯塊對應的PBI(physical block id)，生成(PBI,filename)的pair

第二步，對FBI進行排序

第三步，按照排序結果進行IO



舉例，假定有三個文件FILEA，FILEB，FILEC

第一步，FILEA的FBI為4094，FILEB的FBI為2310，FILEC的FBI為8910

則得到如下的列表

4094 FILEA

2310    FILEB

8910    FILEC

第二步，對FBI按升序排列，得到

2310    FILEB 

4094 FILEA

8910    FILEC 

第三步，讀入文件的順序確定為FILEB,FILEA,FILEC，依次入讀



補充兩個基本要點：

1）每個文件的邏輯塊從0開始編號

2）每個文件的物理塊可以認為是連續（或者接近連續，這個由操作系統保證）



以下代碼為獲得任意一個文件的全部物理塊號

```c
#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "linux/fs.h"

int main(void)
{
    int fd = open("/data/gram_test", O_RDONLY);

    if (fd < 0) {
        return 1;
    }

    struct stat buf;

    int ret = fstat(fd, &buf);

    int nr_blocks = buf.st_blocks; //獲取邏輯塊數

    for (int i = 0; i < nr_blocks; ++i) {

        int physic_block_id = i;
        int ret = ioctl(fd, FIBMAP,
                        &physic_block_id); //從指定邏輯塊獲得物理塊號，需要有root權限。

        if (!physic_block_id) {
            continue;
        }

        printf("%d\t%d\n", i, physic_block_id);
    }
    
    close(fd);  
}
```

