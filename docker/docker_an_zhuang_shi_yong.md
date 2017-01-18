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


執行 docker commit 成功後，會顯示 commit ID，透過執行 docker images，你可以知道這就是 Image ID，接下來你可以從容器中離開了，執行 sudo docker run -i -t caterpillar/java-tutorial bash，啟用的容器中就已經是安裝有 wget 的環境。

##Push 至 Docker hub

在繼續之前，來看看怎麼將你的 Image 放到 [Docker hub](https://hub.docker.com/)，你得先在上頭註冊一個帳號，接著回到你的終端機，使用 docker login 登入，然後執行 docker push：

```sh
sudo docker login
Username: caterpillar
Password: 
Email: caterpillar@openhome.cc
Login Succeeded
~$ sudo docker push caterpillar/java-tutorial
The push refers to a repository [caterpillar/java-tutorial] (len: 1)
Sending image list
Pushing repository caterpillar/java-tutorial (1 tags)
511136ea3c5a: Image already pushed, skipping 
4d289a435341: Image already pushed, skipping 
994db1cb2425: Image already pushed, skipping 
f86a812b1308: Image already pushed, skipping 
0b628db0b664: Image already pushed, skipping 
ebe4be4dd427: Image already pushed, skipping 
a721d68a588a: Image successfully pushed 
Pushing tag for rev [a721d68a588a] on {https://registry-1.docker.io/v1/repositories/caterpillar/java-tutorial/tags/latest}
```


注意！請與先前 Pull 時的訊息比較一下，你會發現 Docker 會比對 Hub 上現有的 Image，不會把你 Local 的 Image 整個上傳，上傳的只有變更的部份，也就是 Image ID 為 a721d68a588a 的 caterpillar/java-tutorial。

## 使用 Dockerfile 自動建構

要自己一個一個在容器中部署環境，當然是很麻煩的一件事，你可以在 Dockerfile 中撰寫部署步驟，例如：

```sh
mkdir java-tutorial
cd java-tutorial
~/java-tutorial$ touch Dockerfile
```
依照 Java Tutorial 第一堂（2）準備課程環境 的環境需求，我們在 Dockerfile 中撰寫：

```sh
FROM caterpillar/java-tutorial
MAINTAINER Justin Lin <caterpillar@openhome.cc>
RUN apt-get -qq update
RUN apt-get -qqy install vim # 男子漢就是要用 vim !!
RUN apt-get -qqy install unzip
RUN apt-get -qqy install git
RUN apt-get -qqy install openjdk-7-jdk
RUN wget http://services.gradle.org/distributions/gradle-1.7-bin.zip
RUN unzip gradle-1.7-bin.zip
RUN rm gradle-1.7-bin.zip
RUN mv gradle-1.7 /usr/lib
RUN ln -s /usr/lib/gradle-1.7/bin/gradle /usr/bin/gradle
RUN git clone https://github.com/codedata/JavaTutorial.git
```

接著執行 docker build 進行建構：

```sh
~/java-tutorial$ sudo docker build -t="caterpillar/java-tutorial" .
Sending build context to Docker daemon 3.584 kB
Sending build context to Docker daemon 
Step 0 : FROM caterpillar/java-tutorial
 ---> a721d68a588a
Step 1 : MAINTAINER Justin Lin <caterpillar@openhome.cc>
 ---> Using cache
 ---> 7390f7280de3
Step 2 : RUN apt-get -qq update
 ---> Running in 93c3f0184d08
 ---> df3d14686563
Removing intermediate container 93c3f0184d08
Step 3 : RUN apt-get -qqy install vim # 男子漢就是要用 vim !!
 ---> Running in a7be275d92fd
...略
```

別忘了最後有個 .，這表示 Dockerfile 來自目前的目錄。建構的過程就可以去做別的事了，建構完成之後，會自動 commit。

