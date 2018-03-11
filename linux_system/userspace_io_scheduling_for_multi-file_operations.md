# Userspace I/O scheduling for multi-file operations



在数量众多的文件读写时，例如一个目录下有400万个图片文件，需要将这400万个图片文件打包，如果这400万个文件读写顺序不对，将会到这磁头来回seek，即便单文件顺序，但由于每个文件较小，因此顺序的机会有限，大部分呈现随机读的状态。

linux内核的IO scheduling 是针对单个文件的，有四种调度方式，但缺乏对大量文件的调度，因此有必要在用户空间进行IO调度。

简单来说我们有一个目录D，目录下有file1，....file400million（文件名），这样400万个文件

首先第一步，获得每个file的首个逻辑块对应的PBI(physical block id)，生成(PBI,filename)的pair

第二步，对FBI进行排序

第三步，按照排序结果进行IO



举例，假定有三个文件FILEA，FILEB，FILEC

第一步，FILEA的FBI为4094，FILEB的FBI为2310，FILEC的FBI为8910

则得到如下的列表

4094 FILEA

2310    FILEB

8910    FILEC

第二步，对FBI按升序排列，得到

2310    FILEB 

4094 FILEA

8910    FILEC 

第三步，读入文件的顺序确定为FILEB,FILEA,FILEC，依次入读



补充两个基本要点：

1）每个文件的逻辑块从0开始编号

2）每个文件的物理块可以认为是连续（或者接近连续，这个由操作系统保证）



以下代码为获得任意一个文件的全部物理块号

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

    int nr_blocks = buf.st_blocks; //获取逻辑块数

    for (int i = 0; i < nr_blocks; ++i) {

        int physic_block_id = i;
        int ret = ioctl(fd, FIBMAP,
                        &physic_block_id); //从指定逻辑块获得物理块号，需要有root权限。

        if (!physic_block_id) {
            continue;
        }

        printf("%d\t%d\n", i, physic_block_id);
    }
    
    close(fd);  
}
```

