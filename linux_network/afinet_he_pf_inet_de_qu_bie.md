# AF_INET和PF_INET的區別


在寫網路程式的時候，建立TCP socket一般是這樣的： 
```c
sock = socket(PF_INET, SOCK_STREAM, 0);
```
然後在綁定本地地址或連接遠程地址時需要初始化sockaddr_in結構，其中指定address family時一般設置為AF_INET，即使用IP。
問題是AF_INET和PF_INET到底有什麼區別，甚至很多參考書上都是混用。今 天查了一下相關頭文件的定義，找到了答案。

```c
AF = Address Family
PF = Protocol Family 
AF_INET = PF_INET
```
所以，理論上建立socket時是指定協議，應該用PF_xxxx，設置地址時應該用AF_xxxx。當然AF_INET和 PF_INET的值是相同的，混用也不會有太大的問題。