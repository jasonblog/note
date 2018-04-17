# GDB在線調試Android Framework Native C/C++代碼


##一、準備工作
###1、下載並編譯Android系統源碼
這裡比較靈活，可以下載公司內部機型的代碼，也可以下載原生AOSP的代碼

環境配置參考

```sh
https://source.android.com/source/initializing.html
```

源碼下載參考

```sh
https://source.android.com/source/downloading.html
```

編譯運行參考

```sh
https://source.android.com/source/building.html
```

全部編譯整個代碼工程，中間如果有什麼問題可以參考

```sh
https://source.android.com/source
```

###2、設置PC端環境
如果要調試自己build的版本，就可以使用out目錄下的symbols 


![](images/20161103131947184)


這裡帶一句，`symbols是帶有debug信息的二進制庫或可執行文件`，用以調試

除了symbols之外我們還需要gdb（client），可以在源碼目錄的prebuilts目錄下找到他們，為了方便，可以直接設置環境變量 


![](images/20161103132006435)

###3、獲取調試的權限
最好使用userdebug或者eng build，實在不行也可以使用root後的user build系統，但是要同時關掉selinux

```sh
adb shell setenforce 0
```

![](images/20161103132031763)

如果是要調試64位的進程就需要gdbserver64 

![](images/20161103132045576)


通過以下命令push到手機中

```sh
adb root
adb disable-verity
adb reboot
```

等待重啟完成

```sh
adb remount
adb push prebuilts/misc/android-arm/gdbserver/gdbserver /system/bin/
```

![](images/20161103132109583)

##二、開始調試

這裡假設要調試的是zygote進程，首先要知道zygote進程對應的可執行文件，這裡是app_process32，64位的zygote64對應的是app_process64


### 1、gdbserver attach到想要調試的進程

```sh
adb shell
ps | grep zygote
gdbserver :1991 --attach 303
```

這裡的端口可以隨便指定一個`空閒`的即可 

![](images/20161103132132139)

###2、gdb client連接到gdbserver
通過執行以下命令進行連接

```sh
adb forward tcp:1991 tcp:1991
arm-linux-androideabi-gdb
target remote:1991
```

這裡需要說一下，如果需要調試的進程是64位的，就要用64位的gdb client然後配合gdbserver64

```sh
aarch64-linux-android-gdb
```

###3、load對應可執行文件


```sh
file /Volumes/1TB-HD/Images/cancro/16.11.01/compressed_cancro_mm-alpha_2016.11.01.18.08_0e62b9421b/out/target/product/cancro/symbols/system/bin/app_process32
```

###4、Set sysroot路徑

```sh
set sysroot /Volumes/1TB-HD/Images/cancro/16.11.01/compressed_cancro_mm-alpha_2016.11.01.18.08_0e62b9421b/out/target/product/cancro/symbols
```


![](images/20161103132156296)


###5、設置源碼目錄

```sh
set dir /Volumes/1TB-HD/CodeRoot/CANCRO_ALPHA/
```

###6、設置斷點

```sh
b frameworks/base/core/jni/fd_utils-inl.h:180

Breakpoint 1 at 0xb6e24f0c: file frameworks/base/core/jni/fd_utils-inl.h, line 180.
```

![](images/20161103132742118)

### 7、繼續運行

```sh
c

Continuing.
[New Thread 5872]
[New Thread 5873]
[New Thread 5874]
[New Thread 5875]
```

操作並等待運行到斷點處

```sh
Breakpoint 1, FileDescriptorInfo::Restat (this=0xb4cbd620) at frameworks/base/core/jni/fd_utils-inl.h:182
warning: Source file is more recent than executable.
182       ALOGE("Restat, st_nlink == 8, (%s, fd=%d) : f_stat.st_nlink=%llu,0x%llX file_stat.st_nlink=%llu,0x%llX", 
```

### 8、查看變量對應的值

```sh
p f_stat.st_nlink

$1 = 1
```

##三、擴展功能
到這裡對於剛接觸gdb調試同學也有了入門的知識，對於gdb老手估計是要玩飛的節奏。。。

但是這裡要說一下，如果要調試的是framework相關的進程的native代碼，可能會受到system server的watchdog的影響，1分鐘沒有及時響應操作就會觸發watchdog而kill到system server進程，zygote也會跟著掛掉，這裡有個小技巧可以用一下，就是在調試的過程中，如果需要耗時查看一些運行時狀態，可以先執行 

```sh
adb shell am hang 
```

防止超時重啟，查看完畢想要繼續執行，就Ctrl+c終止掉am hang即可繼續執行，後面就重複這個過程即可。 
另外還有一種方式就是用Android Studio在線調試，把斷點加在watchdog裡面，配置gdb native調試。