# telnet for raspberry


setNetwork.sh // 啟動網路

- setTelnet.sh // 啟動 telnetd

```sh
/bin/mkdir /dev/pts
/bin/mount -t devpts devpts /dev/pts
/usr/sbin/telnetd -l /bin/login
```

- /etc/passwd

```sh
root::0:0:root:/:/bin/sh
```


telnet 192.168.1.142  // 登入輸入 root 不用密碼即可登入