# 使用gdb实时调式arduino



## 前言

arduino ide 功能过于简单，连最基本的断点调式功能都不提供。通过串口查看调式数据实在是很不便捷，通过某种方式远程调式arduino是很有必要的。


## 环境

这次尝试是在64位ubuntu 14.04 下进行，使用的arduino板子类型为 UNO.需要python 2 支持.

## 开始


### 配置PlatformIO

```
PlatformIO is a cross-platform code builder and the missing library manager.
```
以前我在linux开放arduino时候使用的是ino工具，PlatformIO 和它类似，只不过它功能更加强大，支持的芯片种类繁多，包括stm32,avr都有支持link.对于IDE 也有很好的配置教程link,甚至还提供库管理(没用过).
```sh
sudo pip install platformio && pip install --egg scons
```

安装很简单。

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
之后可以在src 里编辑代码，将库依赖放到lib 里。

用platformio run 上传到板子。

```sh
cd src
cp /home/shihyu/.mybin/arduino-1.7.7-linux64/examples/01.Basics/Blink/Blink.ino .
```

## 进行调式
### 启动gdb server
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
