# 構建docker開發環境

## 安裝之前
Docker源碼需在docker容器編譯環境中編譯，所以先安裝docker，安裝環境依舊是ubuntu14.04

```sh
echo "deb https://apt.dockerproject.org/repo ubuntu-trusty main" | sudo tee /etc/apt/sources.list.d/docker.list
sudo apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys 58118E89F3A912897C070ADBF76221572C52609D
sudo apt-get update
sudo apt-get install docker-engine
```



- 更新 docker

```sh
sudo apt-get upgrade docker-engine
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

