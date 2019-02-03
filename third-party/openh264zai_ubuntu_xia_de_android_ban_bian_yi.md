# openh264在ubuntu下的android版編譯


##安裝jdk
##安裝android-studio和SDK
##安裝NDK
下載openh264
配置環境變量
編譯openh264
安裝jdk
下載jdk並解壓到任意位置
安裝android-studio和SDK

android-studio在linux下的安裝可以參照官網說明

`安裝完畢後記得把sdkmanager中把21的版本安裝了，下面會使用到（具體哪個因人而異，我需要android5.0就下載21了）`

##安裝NDK

在android官網下載NDK解壓到任意目錄
下載openh264

openh264的github地址為

```sh
https://github.com/cisco/openh264
```
可以在ubuntu中使用如下命令下載下來，或者手動下載解壓 
```
git clone https://github.com/cisco/openh264.git
```
配置環境變量
輸入如下命令

vim ~/.bashrc

在末尾添加環境變量配置,以下是一份參考，具體改為自己的目錄

```sh
export JDK_HOME=/home/guest/local/jdk1.8.0_152
export ANDROID_SDK=$HOME/.mybin/android-sdk-linux
export NDK_HOME=$HOME/.mybin/android-ndk-r17c
export ANDROID_STUDIO=$HOME/.mybin/android-studio
export PATH=$PATH:$NDK_HOME
export PATH=$PATH:$JDK_HOME/bin
export PATH=$ANDROID_SDK/tools:$PATH
export PATH=$ANDROID_STUDIO/bin:$PATH
```

最後應用一下 

```sh
source ~/.bashrc
```

編譯openh264
切換到openh264目錄
安照github上android的編譯提示，執行

```sh
make OS=android NDKROOT=/home/guest/local/android-ndk-r14b TARGET=android-21
```


其中ndkroot和target根據自己情況填寫

編譯成功後可以在openh264目錄下看到生成了libopenh264.so，android就可以直接拿這個so去使用了，頭文件在/home/guest/local/openh264/codec/api/svc目錄下，可以看到有以下頭文件

```c
codec_api.h
codec_app_def.h
codec_def.h
codec_ver.h
```

到這裡關於openh264的編譯過程就記錄完了，編譯過程有任何問題善用百度和google基本都能解決。