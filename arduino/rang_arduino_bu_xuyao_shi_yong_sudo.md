# 讓 arduino 不需要使用 sudo


這個例子來看，Arduino 對應到系統的裝置名稱是 ttyACM0。
```sh
$ ls -l /dev/ttyACM0
crw-rw---- 1 root dialout 166, 0 Jan  1 09:22 /dev/ttyACM0
```

注意到它的權限，只給 dialout 這個群組讀寫。

為了要能讀寫 /dev/ttyACM0 這個裝置，必須把所有要使用 Arduino IDE 的帳號全部加到 dialout 這個群組，然後重新登入。

```sh
$ sudo usermod -G dialout -a <USER>
$ sudo usermod -G dialout -a shihyu
```