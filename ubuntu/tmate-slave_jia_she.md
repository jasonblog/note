# tmate-slave 架設


```sh
apt-get install  git-core build-essential pkg-config libtool 
libevent-dev libncurses-dev zlib1g-dev automake libssh-dev cmake ruby libmsgpack-dev
```

```sh
git clone https://github.com/tmate-io/tmate-slave.git
cd tmate-slave
./autogen.sh
./configure
make
```

```sh
./create_keys.sh

yaoshihyu@instance-1:~/tmate-slave$ ./create_keys.sh 
/bin/bash: warning: setlocale: LC_ALL: cannot change locale (zh_TW.UTF-8)
Generating public/private rsa key pair.
Your identification has been saved in keys/ssh_host_rsa_key.
Your public key has been saved in keys/ssh_host_rsa_key.pub.
The key fingerprint is:
MD5:ec:80:4a:c7:c9:13:92:d7:24:1d:dc:78:ef:d3:41:5a yaoshihyu@instance-1
The key's randomart image is:
+---[RSA 2048]----+
|    .oo+         |
|   . ++ o   E    |
|  o o .. . +     |
|   = + .  o .    |
|  . B . S. . .   |
| . o . o  o .    |
|  .     .  .     |
|                 |
|                 |
+------[MD5]------+
Generating public/private ecdsa key pair.
Your identification has been saved in keys/ssh_host_ecdsa_key.
Your public key has been saved in keys/ssh_host_ecdsa_key.pub.
The key fingerprint is:
MD5:9d:e3:67:11:9c:61:43:d3:3e:50:36:80:37:cc:33:4f yaoshihyu@instance-1
The key's randomart image is:
+---[ECDSA 256]---+
|           =B++  |
|          .oO*E. |
|           .+O   |
|         . . .+  |
|        S + .  . |
|         . . .   |
|          . o    |
|           o     |
|                 |
+------[MD5]------+
yaoshihyu@instance-1:~/tmate-slave$ 


sudo ./tmate-slave -p 1234 -k keys/
```

## Make the tmate-slave run as a service

Setup systemd (/etc/systemd/system/tmate-slave.service)


```sh
[Install]
WantedBy=multi-user.target

[Unit]
Description=Tmate-Slave
Wants=network-online.target
After=network-online.target

[Service]
Type=simple
Restart=always
User=root
ExecStart=/xxxxx/tmate-slave -p 1234 -k /xxx/keys
```

## Enable/Start service

```sh
sudo systemctl daemon-reload
sudo systemctl enable tmate-slave
sudo systemctl start tmate-slave
```



## client


```sh
set -g tmate-server-host "35.236.182.178"
set -g tmate-server-port 1234
set -g tmate-server-rsa-fingerprint   "ec:80:4a:c7:c9:13:92:d7:24:1d:dc:78:ef:d3:41:5a"
set -g tmate-server-ecdsa-fingerprint "9d:e3:67:11:9c:61:43:d3:3e:50:36:80:37:cc:33:4f"
```
