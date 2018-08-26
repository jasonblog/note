# Building Andordi & Kernels



```sh
sudo apt-get install libx11-dev:i386 libreadline6-dev:i386 libgl1-mesa-dev g++-multilib  &&
sudo apt-get install -y git flex bison gperf build-essential libncurses5-dev:i386 &&
sudo apt-get install tofrodos python-markdown libxml2-utils xsltproc zlib1g-dev:i386 && 
sudo apt-get install dpkg-dev libsdl1.2-dev libesd0-dev &&
sudo apt-get install git-core gnupg flex bison gperf build-essential &&  
sudo apt-get install zip curl zlib1g-dev gcc-multilib g++-multilib &&
sudo apt-get install libc6-dev-i386 &&
sudo apt-get install lib32ncurses5-dev x11proto-core-dev libx11-dev &&  
sudo apt-get install libgl1-mesa-dev libxml2-utils xsltproc unzip m4 &&
sudo apt-get install lib32z-dev ccache
```

```sh
repo init -u https://android.googlesource.com/platform/manifest -b  android-8.1.0_r43
```

```sh
export LC_ALL=C
source build/envsetup.sh 
lunch aosp_arm64-eng
time make -j8
```

## Kernel

```sh
git clone https://android.googlesource.com/kernel/goldfish.git
 
git clone https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9
```

```cpp
export PATH=$PATH:/media/shihyu/ssd/x86_64-linux-android-4.9/bin
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-android-
make ranchu64_defconfig
time make -j8
```