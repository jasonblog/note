# 使用gdb實時調式arduino



## 前言

arduino ide 功能過於簡單，連最基本的斷點調式功能都不提供。通過串口查看調式數據實在是很不便捷，通過某種方式遠程調式arduino是很有必要的。


## 環境

這次嘗試是在64位ubuntu 14.04 下進行，使用的arduino板子類型為 UNO.需要python 2 支持.

## 開始


### 配置PlatformIO

```
PlatformIO is a cross-platform code builder and the missing library manager.
```
以前我在linux開放arduino時候使用的是ino工具，PlatformIO 和它類似，只不過它功能更加強大，支持的芯片種類繁多，包括stm32,avr都有支持link.對於IDE 也有很好的配置教程link,甚至還提供庫管理(沒用過).
```sh
sudo pip install platformio && pip install --egg scons
```

安裝很簡單。

```sh
 platformio platforms install atmelavr
 platformio platforms show atmelavr
```

## 安裝simulavr ＆ arv-gdb

```sh
sudo apt-get install gdb-avr simulavr
```
## 配置工程

初始化工程
```sh
mkdir uno ; cd uno
platformio init --board=uno

```
之後可以在src 裡編輯代碼，將庫依賴放到lib 裡。

用platformio run 上傳到板子。

```sh
cd src
cp /home/shihyu/.mybin/arduino-1.7.7-linux64/examples/01.Basics/Blink/Blink.ino .
```

## 進行調式
### 啟動gdb server
```sh
simulavr --gdbserver -p 4242 --device atmega16
```
- gdbinit

```
target remote localhost:4242
add-symbol-file .pioenvs/uno/firmware.elf 0
directory src

b setup
b loop
```

```sh
cgdb -d avr-gdb
source gdbinit
```
