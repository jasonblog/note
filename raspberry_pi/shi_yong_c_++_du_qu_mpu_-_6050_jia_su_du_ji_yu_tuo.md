# 使用 C++ 讀取 MPU-6050 加速度計與陀螺儀感測資料


這裡介紹如何在樹莓派上使用 C 語言來讀取 MPU-6050 加速度計與陀螺儀感測資料。

上一篇文章中我們已經將 MPU-6050 連接上 Raspberry Pi 了，接下來就是要實際讀取加速度計與陀螺儀的資料了。


若要使用 C 語言來跟 I2C 的設備溝通，在 Linux 系統中可以使用 i2c-dev 這套函式厙：


```sh
sudo apt-get install libi2c-dev
```

而程式的撰寫在網路上已經有非常多的範例可以參考（請見下方的參考資料），從這些已經寫好的範例程式下手來修改，通常會比較容易，不過如果您是完全沒有經驗的新手，可能還是會需要花上很多時間。

我在 GitHub 上找到 PiBits 這個專案，裡面有針對 Raspberry Pi 與 MPU-6050 寫一個很棒的範例，使用的語言是 C++，這是我看過最簡單就可以入門使用的程式碼了。

首先用 git 將 GitHub 上的程式碼都下載下來：



```sh
git clone https://github.com/richardghirst/PiBits.git
```

進入 PiBits/MPU6050-Pi-Demo 目錄：

```sh
cd PiBits/MPU6050-Pi-Demo
```

這個目錄包含了三個範例程式以及 I2Cdev 與 MPU6050 兩個類別，基本上整個程式架構都使用物件導向的方式規劃的很清楚，所以只要稍微懂一點 C++ 的人，應該都可以立即上手。

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;  // 預設 I2C 位址為 0x68
//MPU6050 accelgyro(0x69);  // 設定 I2C 位址為 0x68

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup()
{
    // 初始化 I2C 設備
    printf("Initializing I2C devices...\n");
    accelgyro.initialize();

    // 測試連線是否正常
    printf("Testing device connections...\n");
    printf(accelgyro.testConnection() ? "MPU6050 connection successful\n" :
           "MPU6050 connection failed\n");
}

void loop()
{
    // 從 MPU-6050 讀取加速度計與陀螺儀資料
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // 其他的讀取方式
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // 輸出
    printf("a/g: %6hd %6hd %6hd   %6hd %6hd %6hd\n", ax, ay, az, gx, gy, gz);
}

int main()
{
    setup();

    for (;;) {
        loop();
    }
}
```

這個程式碼非常簡單，我就不多作解釋了。由於 PiBits 中的專案都已經寫好 Makefile 了，所以直接執行 make 就以自動編譯：

```sh
make
```

然後執行

```sh
sudo ./demo_raw
```

正常的話，應該就可以看到類似這樣的輸出：

```sh
Initializing I2C devices...
Testing device connections...
MPU6050 connection successful
a/g:  16804     24  -3420     -113   -154    -68
a/g:  16664    -72  -3544      -89   -177    -78
a/g:  16800    -20  -3376     -100   -144    -48
a/g:  16676    -56  -3468      -87   -174    -84
a/g:  16712    -64  -3544     -116   -162    -41
a/g:  16776     28  -3528     -107   -157    -95
```

如果您執行之後出現這樣的錯誤訊息：

```sh
Failed to open device: No such file or directory
```

那有可能是因為 I2C 的設備位址指定錯誤，可以檢查一下自己的 I2C 設備：

```sh
ls /dev/i2c*
```
如果您的輸出是這樣：

```sh
/dev/i2c-1
```

那麼請將 I2Cdev.cpp 中所有的

```sh
open("/dev/i2c-0", O_RDWR);
```

改為

```sh
open("/dev/i2c-1", O_RDWR);
```

然後再執行一次

```sh
make
```

這樣應該就可以了。

這裡我們使用 sudo 來執行編譯好的 demo_raw 是因為預設的情況下，一般使用者是不能存取 /dev/i2c-* 的，所以要改用 root 權限，或是您也可以直接將 /dev/i2c-* 的檔案權限變更一下：

```sh
sudo chmod 666 /dev/i2c-*
```

這樣就可以讓一般使者直接存取了。

這樣就可以讓一般使者直接存取了。

這裡我們是使用 Raspberry Pi 來直接讀取 MPU-6050 加速度計與陀螺儀的資料，由於 Linux 作業系統的排程問題，取樣的時間點通常沒辦法太精確，如果您需要比較精準的取樣時間，可以使用 MPU-6050 的 DMP 來取得更精準的運動感測資料，並且也可以減輕 CPU 或 MCU 的負擔。

如果您對於樹莓派的應用有興趣，建議您可以繼續閱讀物聯網的相關文章。

##參考資料：

PiBits、David Grayson’s blog、raspy-juice、SUNXI、ozzmaker.com、大兵萊恩