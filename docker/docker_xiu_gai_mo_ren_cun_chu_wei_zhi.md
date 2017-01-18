# Docker 修改默认存储位置


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


这是一个新装的docker环境，其中看不出当前docker文件存储的路径。但是docker默认会使用 /var/lib/docker 作为默认存储位置。

停止 Docker 服务

```sh
sudo /etc/init.d/docker stop
```

将原来默认的/var/lib/docker备份一下，然后复制到别的位置并建立一个软链接

```sh
cd /var/lib
sudo cp -rf docker docker.bak
sudo cp -rf docker /<my_new_location>/
sudo ln -s /<my_new_location>/docker docker
```

启动 Docker 服务

```sh
sudo /etc/init.d/docker start
```

最后使用 docker info 查看更新结果


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


