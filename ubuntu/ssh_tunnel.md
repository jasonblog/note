# ssh tunnel

## client & server  /etc/ssh/sshd_config 都加上下面的
```
ClientAliveInterval 30
ClientAliveCountMax 60
```

## /etc/hosts.allow   
```sh
sshd:ALL
```

## 創建文件~/autossh.service

```sh
[Unit]
Description=AutoSSH service for a reverse tunnel from some.example.com to localhost
After=network-online.target
[Service]
User=shihyu
Environment="AUTOSSH_GATETIME=0"
ExecStart=/usr/bin/nohup /usr/bin/autossh -f -N -o ServerAliveInterval=30 -o ServerAliveCountMax=60 -R 7777:localhost:22 shihyu@106.104.121.78  -o BatchMode=yes -i /home/shihyu/.ssh/id_rsa_jason_yao
Restart=always
RestartSec=60
[Install]
WantedBy=multi-user.target
```


```sh
# 轉移文件
sudo mv ~/autossh.service /etc/systemd/system/
# 重新加載
sudo systemctl daemon-reload
# 設置讓autossh可用
sudo systemctl enable autossh.service
# 啟動autossh服務
sudo systemctl start autossh.service
# 查看是否有問題，查看autossh服務的狀態
sudo systemctl status autossh.service
```



```sh
ssh -vvv shihyu@localhost -p 7777
```


