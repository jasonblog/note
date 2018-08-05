# ESP8266 & NodeMCU 開發入門


# ESP8266 & NodeMCU 開發入門



```sh
sudo apt-get install make unrar autoconf automake libtool gcc g++ gperf \
flex bison texinfo gawk ncurses-dev libexpat-dev python python-serial sed \
git unzip bash
```

- 需非Root 帳號編譯

```sh
git clone --recursive https://github.com/pfalcon/esp-open-sdk.git esp8266/esp-open-sdk
```

```sh
./esp8266/esp-open-sdk/xtensa-lx106-elf/bin/esptool.py

要改成python2
#!/usr/bin/env python2
```


- esptool.py 需要改用 python2 


```sh
#!/usr/bin/env python2
```

```sh
git clone https://github.com/nodemcu/nodemcu-firmware.git esp8266/nodemcu-firmware
```


```sh
https://github.com/micropython/micropython.git
```

```sh
cd micropython/ports/esp8266
make
```
