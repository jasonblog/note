# 快速删除多个 docker 容器的方法


学习docker的过程中, 随着每一步的操作, 产生了大量停止工作的容器, 想快速的删除多个容器, 看了下docker rm --help, 加上stackoverflow上看到了别人的问答, 总结了2种批量删除容器的方法.


```sh
docker rm $(docker ps -qa)
docker ps -qa | xargs -n 1 docker rm
```

