# 快速刪除多個 docker 容器的方法


學習docker的過程中, 隨著每一步的操作, 產生了大量停止工作的容器, 想快速的刪除多個容器, 看了下docker rm --help, 加上stackoverflow上看到了別人的問答, 總結了2種批量刪除容器的方法.


```sh
docker rm $(docker ps -qa)
docker ps -qa | xargs -n 1 docker rm
```

