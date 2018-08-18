# pi3 設定


### 啟動 uart 

```sh
用讀卡機讀取SD card  再從 /boot/config.txt 加入 enable_uart=1, 就可以透過 /dev/ttyAMA0 通訊
```


### enable wifi

```sh
sudo iwlist wlan0 scan | grep -i 'Iphone'
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf

network={
ssid="SSID"
psk="WIFI PASSWORD"
}
```


###Raspberry啟動SSH連線

```sh
enable ssh
sudo raspi-config
```

### 新增使用者

```sh
sudo adduser james
sudo usermod james -g sudo
```
