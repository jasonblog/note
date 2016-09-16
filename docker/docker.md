# Docker


## 常用指令

```sh
docker version 查看docker的版本號，包括​​客戶端、服務端、依賴的Go等
docker info 查看系統(docker)層面信息，包括管理的images, containers數等
docker search <image> 在docker index中搜索image
docker pull <image> 從docker registry server 中下拉image
docker push <image|repository> 推送一個image或repository到registry
docker push <image|repository>:TAG 同上，指定tag
docker inspect <image|container> 查看image或container的底層信息
docker images TODO filter out the intermediate image layers (intermediate image layers 是什麼)
docker images -a 列出所有的images
docker ps 默認顯示正在運行中的container
docker ps -l 顯示最後一次創建的container，包括未運行的
docker ps -a 顯示所有的container，包括未運行的
docker logs <container> 查看container的日誌，也就是執行命令的一些輸出
docker rm <container…> 刪除一個或多個container
docker rm `docker ps -a -q` 刪除所有的container
docker ps -a -q | xargs docker rm 同上, 刪除所有的container
docker rmi <image…> 刪除一個或多個image
docker start/stop/restart <container> 開啟/停止/重啟container
docker start -i <container> 啟動一個container並進入交互模式
docker attach <container> attach一個運行中的container
docker run <image> <command> 使用image創建container並執行相應命令，然後停止
docker run -i -t <image> /bin/bash 使用image創建container並進入交互模式, login shell是/bin/bash
docker run -i -t -p <host_port:contain_port> 將container的端口映射到宿主機的端口
docker commit <container> [repo:tag] 將一個container固化為一個新的image，後面的repo:tag可選
docker build <path> 尋找path路徑下名為的Dockerfile的配置文件，使用此配置生成新的image
docker build -t repo[:tag] 同上，可以指定repo和可選的tag
docker build – < <dockerfile> 使用指定的dockerfile配置文件，docker以stdin方式獲取內容，使用此配置生成新的image
docker port <container> <container port> 查看本地哪個端口映射到container的指定端口，其實用docker ps 也可以看到
```


```sh
sudo docker cp astyle_2.05.1_linux.tar.gz 04ed79da5285:/root/
```