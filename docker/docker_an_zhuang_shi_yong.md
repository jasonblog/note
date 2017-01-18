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

