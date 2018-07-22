# QEMU + GDB + LK4 on Docker


## 安裝Docker
```sh
sudo apt install docker.io
sudo systemctl start docker
sudo systemctl enable docker
docker --version
sudo usermod -aG docker shihyu
```

### Docker 修改默認存儲位置

```sh
sudo /etc/init.d/docker stop

cd /var/lib
sudo mv docker /media/shihyu/ssd/docker
sudo ln -s  /media/shihyu/ssd/docker docker

sudo /etc/init.d/docker start
```

### 進同一個 Docker容器

```sh
docker ps 
docker exec -it CONTAINER ID /bin/bash
```

### 快速刪除多個 docker 容器的方法 
總結了2種批量刪除容器的方法

```sh
docker stop $(docker ps -qa)
docker rm $(docker ps -qa)

docker ps -qa | xargs -n 1 docker stop
docker ps -qa | xargs -n 1 docker rm

docker ps -a
docker stop c8fddb25161f && docker rm c8fddb25161f
```

### 刪除docker image

```sh
docker image rm IMAGE ID
```

### docker commit


```sh
