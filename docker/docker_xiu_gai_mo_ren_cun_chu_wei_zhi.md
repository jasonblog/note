# Docker 修改默認存儲位置


首先使用 docker info 查看 docker 的基本信息

```sh
sudo docker info
Containers: 0
Images: 5
Storage Driver: devicemapper
 Pool Name: docker-253:0-679369-pool
 Pool Blocksize: 65.54 kB
 Backing Filesystem: extfs
 Data file: 
 Metadata file: 
 Data Space Used: 533.9 MB
 Data Space Total: 107.4 GB
 Data Space Available: 106.8 GB
 Metadata Space Used: 954.4 kB
 Metadata Space Total: 2.147 GB
 Metadata Space Available: 2.147 GB
 Udev Sync Supported: true
 Deferred Removal Enabled: false
 Library Version: 1.02.89-RHEL6 (2014-09-01)
Execution Driver: native-0.2
Logging Driver: json-file
Kernel Version: 2.6.32-431.29.2.el6.x86_64
Operating System: <unknown>
CPUs: 1
Total Memory: 853.5 MiB
Name: rhel62build
ID: LIO4:I43U:YFQU:SN45:YOE6:7GTF:6AGX:VSKQ:ICIV:AKWD:3OKQ:HUSC
```


這是一個新裝的docker環境，其中看不出當前docker文件存儲的路徑。但是docker默認會使用 /var/lib/docker 作為默認存儲位置。

停止 Docker 服務

```sh
sudo /etc/init.d/docker stop
```

將原來默認的/var/lib/docker備份一下，然後複製到別的位置並建立一個軟鏈接

```sh
cd /var/lib
sudo cp -rf docker docker.bak
sudo mv docker /media/shihyu/ssd/docker
sudo ln -s  /media/shihyu/ssd/docker docker
```

啟動 Docker 服務

```sh
sudo /etc/init.d/docker start
```

最後使用 docker info 查看更新結果


```sh
sudo docker info
Containers: 0
Images: 5
Storage Driver: devicemapper
 Pool Name: docker-8:21-393695-pool
 Pool Blocksize: 65.54 kB
 Backing Filesystem: extfs
 Data file: /dev/loop1
 Metadata file: /dev/loop2
 Data Space Used: 533.9 MB
 Data Space Total: 107.4 GB
 Data Space Available: 15.33 GB
 Metadata Space Used: 954.4 kB
 Metadata Space Total: 2.147 GB
 Metadata Space Available: 2.147 GB
 Udev Sync Supported: true
 Deferred Removal Enabled: false
 Data loop file: /build/docker/devicemapper/devicemapper/data
 Metadata loop file: /build/docker/devicemapper/devicemapper/metadata
 Library Version: 1.02.89-RHEL6 (2014-09-01)
Execution Driver: native-0.2
Logging Driver: json-file
Kernel Version: 2.6.32-431.29.2.el6.x86_64
Operating System: <unknown>
CPUs: 1
Total Memory: 853.5 MiB
Name: rhel62build
ID: LIO4:I43U:YFQU:SN45:YOE6:7GTF:6AGX:VSKQ:ICIV:AKWD:3OKQ:HUSC
```

## 刪除 Docker image

```sh
docker ps -a


CONTAINER ID        IMAGE               COMMAND             CREATED             STATUS                         PORTS               NAMES
56ee339399dc        ubuntu:14.04        "bash"              22 minutes ago      Exited (0) 9 minutes ago                           tender_archimedes
7e889ef55800        yshihyu/ubuntu16    "bash"              22 minutes ago      Exited (0) 22 minutes ago                          elated_brattain
c1e986571728        ubuntu:16.04        "bash"              41 minutes ago      Exited (0) 22 minutes ago                          desperate_kirch
56541fac1049        ubuntu:14.04        "bash"              59 minutes ago      Exited (0) 56 minutes ago                          determined_stonebraker
c8fddb25161f        hello-world         "/hello"            About an hour ago   Exited (0) About an hour ago                       drunk_payne
```


```sh
docker stop c8fddb25161f && docker rm c8fddb25161f
```
