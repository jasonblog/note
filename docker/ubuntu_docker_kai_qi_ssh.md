# ubuntu docker 开启ssh

## 1. 下载ubuntu镜像
因为从docker hub上下载的镜像apt-get update报错， 
所以就直接从网易蜂巢上下载已经装好各种工具的ubuntu镜像了。

```sh
docker pull hub.c.163.com/public/ubuntu:16.04-tools 
```




## 2. 启动镜像

```sh
docker run -itd -p 10022:22 1196ea15dad6 
```

将本机10022端口，映射到容器的22端口。


## 3. 进入容器修改ssh配置，并启动ssh

进入容器 

```sh
docker exec -it 7c07ba426a27 /bin/bash 
```
给root设置一个密码 123456 

```sh
passwd root 
```

修改配置文件 

```sh
vim /etc/ssh/sshd_config 
```
注释第一行，添加第二行

```sh
# PermitRootLogin prohibit-password. 
PermitRootLogin yes
```
保存退出后，重启ssh服务 

```sh
/etc/init.d/ssh restart
```

##4. 本机ssh连接容器
```sh
ssh root@127.0.0.1 -p 10022
```