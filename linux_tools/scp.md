# scp 跨機遠程拷貝


scp是secure copy的簡寫，用於在Linux下進行遠程拷貝文件的命令，和它類似的命令有cp，不過cp只是在本機進行拷貝不能跨服務器，而且scp傳輸是加密的。當你服務器硬盤變爲只讀 read only system時，用scp可以幫你把文件移出來。

註解

類似的工具有rsync；scp消耗資源少，不會提高多少系統負荷，在這一點上，rsync就遠遠不及它了。rsync比scp會快一點，但當小文件多的情況下，rsync會導致硬盤I/O非常高，而scp基本不影響系統正常使用。
### 18.1. 命令格式：
scp [參數] [原路徑] [目標路徑]

### 18.2. 命令參數：
- -1 強制scp命令使用協議ssh1
- -2 強制scp命令使用協議ssh2
- -4 強制scp命令只使用IPv4尋址
- -6 強制scp命令只使用IPv6尋址
- -B 使用批處理模式（傳輸過程中不詢問傳輸口令或短語）
- -C 允許壓縮。（將-C標誌傳遞給ssh，從而打開壓縮功能）
- -p 留原文件的修改時間，訪問時間和訪問權限。
- -q 不顯示傳輸進度條。
- -r 遞歸複製整個目錄。
- -v 詳細方式顯示輸出。scp和ssh(1)會顯示出整個過程的調試信息。這些信息用於調試連接，驗證和配置問題。
- -c cipher 以cipher將數據傳輸進行加密，這個選項將直接傳遞給ssh。
- -F ssh_config 指定一個替代的ssh配置文件，此參數直接傳遞給ssh。
- -i identity_file 從指定文件中讀取傳輸時使用的密鑰文件，此參數直接傳遞給ssh。
- -l limit 限定用戶所能使用的帶寬，以Kbit/s爲單位。
- -o ssh_option 如果習慣於使用ssh_config(5)中的參數傳遞方式，
- -P port 注意是大寫的P, port是指定數據傳輸用到的端口號
- -S program 指定加密傳輸時所使用的程序。此程序必須能夠理解ssh(1)的選項。

### 18.3. 使用說明
### 從本地服務器複製到遠程服務器
複製文件:
```
$scp local_file remote_username@remote_ip:remote_folder
$scp local_file remote_username@remote_ip:remote_file
$scp local_file remote_ip:remote_folder
$scp local_file remote_ip:remote_file
```

指定了用戶名，命令執行後需要輸入用戶密碼；如果不指定用戶名，命令執行後需要輸入用戶名和密碼；

複製目錄:
```
$scp -r local_folder remote_username@remote_ip:remote_folder
$scp -r local_folder remote_ip:remote_folder
```
第1個指定了用戶名，命令執行後需要輸入用戶密碼； 第2個沒有指定用戶名，命令執行後需要輸入用戶名和密碼；

註解

從遠程複製到本地的scp命令與上面的命令一樣，只要將從本地複製到遠程的命令後面2個參數互換順序就行了。
### 18.4. 使用示例
### 實例1：從遠處複製文件到本地目錄
```
$scp root@10.6.159.147:/opt/soft/demo.tar /opt/soft/
```
說明： 從10.6.159.147機器上的/opt/soft/的目錄中下載demo.tar 文件到本地/opt/soft/目錄中

### 實例2：從遠處複製到本地
```
$scp -r root@10.6.159.147:/opt/soft/test /opt/soft/
```
說明： 從10.6.159.147機器上的/opt/soft/中下載test目錄到本地的/opt/soft/目錄來。

### 實例3：上傳本地文件到遠程機器指定目錄
```
$scp /opt/soft/demo.tar root@10.6.159.147:/opt/soft/scptest
```
說明： 複製本地opt/soft/目錄下的文件demo.tar 到遠程機器10.6.159.147的opt/soft/scptest目錄

### 實例4：上傳本地目錄到遠程機器指定目錄
```
$scp -r /opt/soft/test root@10.6.159.147:/opt/soft/scptest
```
說明： 上傳本地目錄 /opt/soft/test到遠程機器10.6.159.147上/opt/soft/scptest的目錄中
