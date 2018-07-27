# ubuntu docker 開啟ssh

## 1. 下載ubuntu鏡像
因為從docker hub上下載的鏡像apt-get update報錯， 
所以就直接從網易蜂巢上下載已經裝好各種工具的ubuntu鏡像了。

```sh
docker pull hub.c.163.com/public/ubuntu:16.04-tools 
```




## 2. 啟動鏡像

```sh
docker run -itd -p 10022:22 1196ea15dad6 
```

將本機10022端口，映射到容器的22端口。


## 3. 進入容器修改ssh配置，並啟動ssh

進入容器 

```sh
docker exec -it 7c07ba426a27 /bin/bash 
```
給root設置一個密碼 123456 

```sh
passwd root 
```

修改配置文件 

```sh
vim /etc/ssh/sshd_config 
```
註釋第一行，添加第二行

```sh
# PermitRootLogin prohibit-password. 
PermitRootLogin yes
```
保存退出後，重啟ssh服務 

```sh
/etc/init.d/ssh restart
```

##4. 本機ssh連接容器
```sh
ssh root@127.0.0.1 -p 10022
```