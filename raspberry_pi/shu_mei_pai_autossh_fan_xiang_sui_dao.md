# 树莓派autossh反向隧道


通常我们会将树莓派放在家里，做家庭影院之类的用途。而现在，各个运营商的宽带都开始实施内网政策，也就是不会给民宅公网IP，所以像以前借助花生壳软件来实现DNS动态修改的方法是无法奏效了。

那么该怎么办呢？

##ssh反向隧道

现在，我们想要在外部访问家庭网络中的树莓派，可以使用ssh反向隧道技术，它的原理比较简单：

树莓派主动向某公网服务器建立ssh连接，并请求公网服务器开启1个额外的SSH的服务端口，充当树莓派的反向代理服务。树莓派与公网服务器之间的TCP（SSH）连接是树莓派主动发起的，而公网服务器与外部用户之间的TCP（SSH）连接是外部用户主动发起的，公网服务器在中间充当代理角色，转发两侧的数据。

从更具体的角度来讲，外部用户到公网服务器之间可以建立多条TCP连接，而公网服务器到树莓派则只有一条共享的反向TCP连接，这是整个技术实现角度的原理。

##配置autossh

树莓派向公网服务器建立的ssh连接可能因为网络问题断开，所以一般我们不直接使用ssh命令而是使用一个监督程序叫做autossh，它负责拉起ssh命令，并且当ssh断开后可以重新拉起ssh。

首先，因为autossh会帮我们建立到公网服务器的ssh连接，为了免去输入密码的问题，我们要让公网服务器信任树莓派。

为树莓派生成ssh公钥私钥：


```sh
pi@raspberrypi:~ $ ssh-keygen -t rsa
```

然后通过ssh自带的命令将树莓派的私钥拷贝到公网服务器，这样公网服务器就完成了对树莓派的信任配置：


```sh
pi@raspberrypi:~ $ ssh-copy-id -i ~/.ssh/id_rsa work@yuerblog.cc
```

上述命令将树莓派pi用户授信给公网服务器yuerblog.cc的work用户，此后在pi用户下ssh work@yuerblog.cc就不需要输入密码了，你可以自己验证。

接下来，安装autossh：


```sh
pi@raspberrypi:~ $ sudo apt-get install autossh
```

编写启停autossh的脚本：


```sh
pi@raspberrypi:~ $ vim /etc/init.d/autossh.sh
 
#Insert the following scripts
 
#!/bin/sh
### BEGIN INIT INFO
# Provides:          autossh
# Required-Start:    $local_fs $remote_fs $network $syslog
# Required-Stop:     $local_fs $remote_fs $network $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: starts the autossh
# Description:       starts the autossh
### END INIT INFO
 
case "$1" in
    start)
    echo "start autossh"
    killall -0 autossh
    if [ $? -ne 0 ];then
       sudo /usr/bin/autossh -M 888 -fN -o "PubkeyAuthentication=yes" -o "StrictHostKeyChecking=false" -o "PasswordAuthentication=no" -o "ServerAliveInterval 60" -o "ServerAliveCountMax 3" -R 2222:localhost:22 -i /home/pi/.ssh/id_rsa work@yuerblog.cc
    fi
    ;;
    stop)
    sudo killall autossh
    ;;
    restart)
    sudo killall autossh
    sudo /usr/bin/autossh -M 888 -fN -o "PubkeyAuthentication=yes" -o "StrictHostKeyChecking=false" -o "PasswordAuthentication=no" -o "ServerAliveInterval 60" -o "ServerAliveCountMax 3" -R 2222:localhost:22 -i /home/pi/.ssh/id_rsa work@yuerblog.cc
    ;;
    *)
    echo "Usage: $0 (start|stop|restart)"
    ;;
esac
exit 0
 
#Then save the file
```

这个脚本支持start/stop/restart三个命令，启动autossh的命令参数作用如下：

- -o是指定ssh命令的参数，不需要做修改。
- -M是autossh启动的ssh进程会监听888这个端口，autossh进程会通过探测ssh进程的888端口来获知ssh连接是否断开。
- -f是指autossh后台运行，不会阻塞shell继续向下执行。
- -N是指建立的ssh连接只用于转发数据，不解析命令。
- -R是指建立反向隧道，一般我们ssh某个服务器是正向隧道。
- 2222是公网服务器上的代理端口。
- localhost:22是指公网2222端口代理到的树莓派端口。（当你连接公网服务器的2222端口后，数据通过树莓派之间的反向ssh连接到达树莓派，由树莓派端的ssh进程代理发起到localhost:22的2次连接，从而实现ssh访问树莓派的目的）
- -i指定的是之前授信给公网服务器的ssh私钥
- 最后是公网服务器的地址，autossh会调用ssh建立到它的ssh反向隧道。

本来打算将上述脚本做到开启自动启动中，但是试验发现开机没有成功启动autossh，索性把它写到crontab里算了：


```sh
* * * * * /bin/bash /etc/init.d/autossh.sh start
```

接下来，你可以观察是否有autossh进程被拉起：


```sh
pi@raspberrypi:~ $ ps aux|grep autossh
root       692  0.0  0.2   1812  1276 ?        Ss   Sep13   0:00 /usr/lib/autossh/autossh -M 888 -N  -o PubkeyAuthentication=yes -o StrictHostKeyChecking=false -o PasswordAuthentication=no -o ServerAliveInterval 60 -o ServerAliveCountMax 3 -R 2222:localhost:22 -i /home/pi/.ssh/id_rsa work@yuerblog.cc
pi       11277  0.0  0.4   4744  2008 pts/0    S+   09:44   0:00 grep --color=auto autossh
```


##最后

现在，我登录yuerblog.cc服务器，执行如下命令即可登录到pi：

```sh
[work@vultr ~]$ ssh pi@localhost -p 2222
```