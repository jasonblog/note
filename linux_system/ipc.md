# IPC


- 父進程通過fork可以將打開文件的描述符傳遞給子進程
- 子進程結束時，父進程調用wait可以得到子進程的終止信息
- 幾個進程可以在文件系統中讀寫某個共享文件，也可以通過給文件加鎖來實現進程間同步
- 進程之間互發信號，一般使用SIGUSR1和SIGUSR2實現用戶自定義功能
- 管道
- FIFO
- mmap函數，幾個進程可以映射同一內存區
- SYS V IPC，以前的SYS V UNIX系統實現的IPC機制，包括消息隊列、信號量和共享內存，現在已經基本廢棄
- UNIX Domain Socket，目前最廣泛使用的IPC機制