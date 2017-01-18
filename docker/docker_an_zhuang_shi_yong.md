# Docker 安裝使用


## Ubuntu 16.04

```sh
sudo apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys 58118E89F3A912897C070ADBF76221572C52609D

sudo apt-add-repository 'deb https://apt.dockerproject.org/repo ubuntu-xenial main'

sudo apt-get update

apt-cache policy docker-engine

docker-engine:
  Installed: (none)
  Candidate: 1.11.1-0~xenial
  Version table:
     1.11.1-0~xenial 500
        500 https://apt.dockerproject.org/repo ubuntu-xenial/main amd64 Packages
     1.11.0-0~xenial 500
        500 https://apt.dockerproject.org/repo ubuntu-xenial/main amd64 Packages
        


sudo apt-get install -y docker-engine
sudo systemctl status docker

```


- 啟動 docker 程序

```sh
sudo service docker start
```


- Cannot connect to the Docker daemon. Is the docker daemon running on this host?

為了確認docker是否在運行

```sh
sudo service docker status
docker start/running, process 2277
```
這個問題很有可能是權限問題，把執行docker命令的用戶添加到docker，如用戶名為ubuntu，執行

```sh
sudo usermod -aG docker ubuntu
```

- 確定 docker 已經安裝完成

```sh
docker run hello-world
```

```sh
Hello from Docker!
This message shows that your installation appears to be working correctly.

To generate this message, Docker took the following steps:
 1. The Docker client contacted the Docker daemon.
 2. The Docker daemon pulled the "hello-world" image from the Docker Hub.
 3. The Docker daemon created a new container from that image which runs the
    executable that produces the output you are currently reading.
 4. The Docker daemon streamed that output to the Docker client, which sent it
    to your terminal.

To try something more ambitious, you can run an Ubuntu container with:
 $ docker run -it ubuntu bash

Share images, automate workflows, and more with a free Docker Hub account:
 https://hub.docker.com

For more examples and ideas, visit:
 https://docs.docker.com/engine/userguide/
```



想要知道現在系統上有哪些 Image，可以執行 docker images：

```sh
sudo docker images

REPOSITORY          TAG                 IMAGE ID            CREATED             VIRTUAL SIZE
ubuntu              12.04               ebe4be4dd427        4 days ago          210.6 MB
```


##容器環境與 commit

先來指定 ubuntu:12.04 運行一個容器，執行 bash 並在其中裝個 wget 試試：

```sh
sudo docker run -t -i ubuntu:12.04 bash


root@37733a045f92:/# apt-get install wget
Reading package lists... Done
... 略
ldconfig deferred processing now taking place
root@37733a045f92:/# exit
```

執行 docker run 時，-t 在容器中建立了一個終端機，-i 建立與容器標準輸入（STDIN）的互動連結，接下來你就可以在 bash 中進行指令操作了，終端機提示 root@37733a045f92 中，37733a045f92 是目前的容器 ID。

接下來離開 bash 後，重新執行 docker run -t -i ubuntu:12.04 bash，又是原來乾淨的環境了…XD

如果你想要保存在容器中的變更，必須進行 commit，你可以如上進入 bash、安裝 wget，在離開容器之前，在另一個終端機執行 docker ps，查看目前正在運行的容器狀態：



```sh
sudo docker ps

CONTAINER ID        IMAGE               COMMAND             CREATED             STATUS              PORTS               NAMES
30f63c6e8d15        ubuntu:12.04        bash                12 seconds ago      Up 12 seconds                           stoic_nobel
```

容器 ID 為 30f63c6e8d15，你可以對照一下容器中 bash 的提示，兩個是對應的，想要 commit 變更，在執行 docker ps 的終端機中執行：

```sh
sudo docker commit -m="Added wget" -a="Justin Lin" 30f63c6e8d15 yshihyu/wget

a721d68a588ab4932a5d9881041bec3fac6c5ae6fc26e0f9c459fa08f8ce4f16

sudo docker images

REPOSITORY                  TAG                 IMAGE ID            CREATED             VIRTUAL SIZE
yshihyu/wget   latest              a721d68a588a        8 seconds ago       212 MB
ubuntu                      12.04               ebe4be4dd427        4 days ago          210.6 MB
```


