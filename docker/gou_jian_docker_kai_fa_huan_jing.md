# 構建docker開發環境

## 安裝docker

docker的build環境本身就是個container，所以先安裝個docker。

```sh
git clone https://git@github.com/docker/docker
```

## Cannot connect to the Docker daemon. Is the docker daemon running on this host?

为了确认docker是否在运行
```sh
$sudo service docker status
docker start/running, process 2277
```
这个问题很有可能是权限问题，把执行docker命令的用户添加到docker，如用户名为ubuntu，执行

```sh
sudo usermod -aG docker ubuntu
```