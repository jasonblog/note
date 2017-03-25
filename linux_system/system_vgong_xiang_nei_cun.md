# system V共享內存


system V共享內存和posix共享內存類似，system V共享內存是調用shmget函數和shamat函數。   
shmget函數創建共享內存區，或者訪問一個存在的內存區，類似系統調用共享內存的open和posix共享內存shm_open函數。shmget函數原型為：


```c
#include <sys/ipc.h>  
#include <sys/shm.h>  
  
int shmget(key_t key, size_t size, int shmflg); 
```


key: 函數ftok返回值，或者IPC_PRIVATE ,當使用IPC_PRIVATE時，最好兩個進程空間是共享的，比如父子進程，否則當前進程產生的共享內存標識（返回值），在另一個進程裡面不易得到；


ftok函數原型為：key_t ftok(const char *pathname, int proj_id); 參數pathname為文件絕對路徑名，proj_id為一個整型標識符，該函數將一個已存在的的路徑名和一個整型標識符轉化成一個key_t值（返回值），稱為IPC鍵。

size：創建新的共享內存大小，當創建一片新的共享內存時，該值為不為0的參數。如果是讀取一片共享內存，該值可以為0。

shmflg：讀寫權限值組合。IPC_CREAT（創建新的共享內存）或IPC_CREAT|IPC_EXCL（當將要創建的共享內存已經存在時，再試圖創建將返回EEXIST）。其實IPC_CREAT和IPC_EXCL的組合和open函數的O_CREAT和O_EXCL組合類似。

函數返回共享內存區的標識。shmxxx函數操作共享內存將使用該函數返回值。該函數類似posix共享內存shm_open函數功能。

當shmget創建或打開一個共享內存區後，需要使用函數shmat來將該片共享內存連接到當前進程空間中來，當某一進程使用完共享內存後，使用函數shmdt斷開和共享內存的鏈接。


```c
#include <sys/types.h>
#include <sys/shm.h>
void *shmat(int shmid, const void *shmaddr, int shmflg);
int shmdt(const void *shmaddr);  
```

 shmid：是函數shmget函數返回的共享內存標識符。
 
shmaddr： 連接到調用進程地址空間的地址，如果該參數為NULL，系統選擇一個合適地址；如果shmaddr非空並且shmflg指定了選項SHM_RND，那麼相應的共享內存鏈接到由shmaddr參數指定的地址向下舍入一個SHMLAB常值。如果shmaddr非空並且shmflg未指定SHM_RND，共享內存地址鏈接到shmaddr參數指定的地址。

shmflg：可以指定SHM_RND和SHM_RDONLY(只讀)，如果指定SHM_RDONLY選項，那麼調用進程對該片共享內存只有讀權限，否則，進程對該片內存將有讀寫權限。

```c
#include <sys/ipc.h>
#include <sys/shm.h>
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
```

shmid：共享內存區標識；

cmd：對共享內存的操作命令，命令IPC_RMID銷燬（destroy）一片共享內存，銷燬之後所有shmat，shmdt，shmctl對該片內存操作都將失效，銷燬該共享內存要等到該共享內存引用計數變為0才進行；IPC_SET命令設置shmid_ds結構成員；IPC_STAT返回當前共享內存結構；其餘命令查看man手冊。

buf：為指向shmid_ds數據結構；


system V 共享內存示例：

server process：

```c
int sln_shm_get(char* shm_file, void** mem, int mem_len)
{
    int shmid;
    key_t key;


    if (NULL == fopen(shm_file, "w+")) {
        printf("fopen: %s\n", strerror(errno));
        return -1;
    }

    key = ftok(shm_file, 0);

    if (key < 0) {
        printf("ftok: %s\n", strerror(errno));
        return -1;
    }

    shmid = shmget(key, mem_len, IPC_CREAT);

    if (shmid < 0) {
        printf("shmget: %s\n", strerror(errno));
        return -1;
    }

    *mem = (void*)shmat(shmid, NULL, 0);

    if ((void*) - 1 == *mem) {
        printf("shmat: %s\n", strerror(errno));
        return -1;
    }

    return shmid;
}

int main(int argc, const char* argv[])
{
    char* shm_file = NULL;
    char* shm_buf = NULL;
    int shmid;


    shmid = sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN);

    if (shmid < 0) {
        return -1;
    }

    snprintf(shm_buf, SHM_IPC_MAX_LEN,
             "Hello system V shaare memory IPC! this is write by server.");

    sleep(15);

    printf("System V server delete share memory segment!\n");
    //shmdt(shm_buf);

    shmctl(shmid, IPC_RMID,
           NULL); //server在15秒之後destroy該片共享內存，此時客戶進程將獲取不到共享內存的內容

    return 0;
}
```

client process：

```c
int sln_shm_get(char* shm_file, void** mem, int mem_len)
{
    int shmid;
    key_t key;


    key = ftok(shm_file, 0);

    if (key < 0) {
        printf("ftok: %s\n", strerror(errno));
        return -1;
    }

    shmid = shmget(key, mem_len, IPC_CREAT);

    if (shmid < 0) {
        printf("shmget: %s\n", strerror(errno));
        return -1;
    }

    *mem = (void*)shmat(shmid, NULL, 0);

    if ((void*) - 1 == *mem) {
        printf("shmat: %s\n", strerror(errno));
        return -1;
    }

    return shmid;
}

int main(int argc, const char* argv[])
{
    char* shm_buf = NULL;
    int i;

    if (sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN) < 0) {
        return -1;
    }


    printf("ipc client get: %s\n", shm_buf);


    return 0;
}
```

運行時，首先執行server process，使用命令ipcs可以查看當前系統共享內存：
```sh
# ipcs 
------ Message Queues -------- 
key msqid owner perms used-bytes messages 


------ Shared Memory Segments -------- 
key shmid owner perms bytes nattch status 
0x0010a797 131072 root 0 4096 1 


------ Semaphore Arrays -------- 
key semid owner perms nsems 
```

可以看到存在一個共享內存區，其中key為：0x0010a797 ，共享內存ID為：131072 

```sh
# ./client 
ipc client get: Hello system V shaare memory IPC! this is write by server. 
#
```
當server進程destroy共享內存之後，再重複上面步驟，
```sh
# ipcs 

------ Message Queues -------- 
key msqid owner perms used-bytes messages 


------ Shared Memory Segments -------- 
key shmid owner perms bytes nattch status 


------ Semaphore Arrays -------- 
key semid owner perms nsems 
```

此時共享內存已經不在了，但文件依然存在。

```sh
# ./client
ipc client get: 
```



此時客戶端已經不能獲取之前共享內存內容了。
另外，ipcrm命令可以在命令行上刪除指定共享內存區。
通過讀取文件/proc/sys/kernel/shmmax可以獲取系統所支持共享內存最大值，

```sh
# cat /proc/sys/kernel/shmmax
33554432
```


可以看到我目前系統支持最大個共享內存值為：32M。


通過上示例可以看到system V共享內存和posix共享內存類似，不過posix共享內存的大小可以隨時通過ftruncate改變，而system V 的共享內存大小在shmget時就已經確定下來了。
同樣的，system V共享內存大多數時候也需要在多進程之間同步，system V 可以使用自己的信號量來實現，具體細節將在後面同步相關專欄詳細講解。

本節源碼下載：

http://download.csdn.net/detail/gentleliu/8140887