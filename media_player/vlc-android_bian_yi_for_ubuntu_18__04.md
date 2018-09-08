# vlc-android 編譯 for ubuntu 18.04



```sh
https://wiki.videolan.org/AndroidCompile/
```


```sh

sudo apt-get install automake ant autopoint cmake build-essential libtool \
     patch pkg-config protobuf-compiler ragel subversion unzip git \
    openjdk-8-jre openjdk-8-jdk flex
```


```sh
export ANDROID_HOME="/media/shihyu/Toshiba/vr_sdk_ndk/android-sdk-linux"
export ANDROID_SDK=/media/shihyu/Toshiba/vr_sdk_ndk/android-sdk-linux
export ANDROID_NDK=/media/shihyu/Toshiba/vr_sdk_ndk/android-ndk-r14b
```



```sh
git clone https://code.videolan.org/videolan/vlc-android.git
```

```sh
time ./compile.sh -a armeabi-v7a 2>&1 | tee mlog
```

### `freetype-2.9.1/src/base/ftver.rc:19:9: fatal error: windows.h: No such file or directory  #include<windows.h>`
 
 
```sh
freetype/builds/freetype.mk



註解掉

#ifneq ($(RC),)
#  FTVER_SRC := $(BASE_DIR)/ftver.rc
#  FTVER_OBJ := $(OBJ_DIR)/ftver.$O
#
#  OBJECTS_LIST += $(FTVER_OBJ)
#
#  $(FTVER_OBJ): $(FTVER_SRC)
#	$(RC) -o $@ $<
#endif

```

在繼續編譯

```sh
time ./compile.sh -a armeabi-v7a 2>&1 | tee mlog