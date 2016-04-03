# 使用 MPU-6050 的 DMP 來取得更精準的運動感測資料

這裡介紹如何在樹莓派 Raspberry Pi 中使用 MPU-6050 的 DMP 來取得更精準的運動感測資料。

MPU-6050 加速度計與陀螺儀六軸感測器內建的 Digital Motion Processor（DMP）可以負責一些運動處理演算法（motion processing algorithm）的計算，DMP 可從加速度計（accelerometers）、陀螺儀（gyroscopes）或第三方的感測器上讀取資料，透過 DMP 的暫存器來讓使用者讀取運算的結果，或是將運算結果放進 FIFO 中。

DMP 主要的用途在於處理即時性的需求，並且分擔一些運算的工作，一般來說 DMP 會以很高的計算速度（大約是 `200Hz`）來進行運動處理演算法的運算，降低延遲以提供精準的數據，甚至在低取樣速度（如 5Hz）的應用上，DMP 還是會保持這樣的運算速度，以確保資料的精準性。

在前一篇文章中，我們使用了 PiBits 這個專案的 demo_raw.cpp 讀取原始的感測資料，而接下來我們繼續來看第二個使用 DMP 的範例程式 demo_dmp.cpp，因為這個程式碼比較長，所以就不全部貼上來了，我只說明比較重要的部分。

程式一開始先呼叫 setup() 進行初始化：


```c
// 初始化 I2C 設備
printf("Initializing I2C devices...\n");
mpu.initialize();

// 測試一下連線是否正常
printf("Testing device connections...\n");
printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

// 載入與設定 DMP
printf("Initializing DMP...\n");
devStatus = mpu.dmpInitialize();
```

接著啟用 DMP：


```c
// 開啟 DMP
printf("Enabling DMP...\n");
mpu.setDMPEnabled(true);
```

在讀取資料之前，必須先確認 DMP 的封包大小：

```c
// 取得 DMP 封包大小
packetSize = mpu.dmpGetFIFOPacketSize();
```

這樣就完成初始化的動作了，接著就開始進入主要的無窮迴圈，重複呼叫 loop() 讀取資料。在 loop() 中，先取得 FIFO 目前的大小：

```c
fifoCount = mpu.getFIFOCount();
```

然後檢查看看 FIFO 是否有溢位的狀況，如果 FIFO 沒有溢位，再檢查 FIFO 資料大小是否超過一個 DMP 封包大小，如果超過的話，就可以讀取一個封包的資料進來：

```c
if (fifoCount == 1024) { // FIFO 溢位
  // 重設 FIFO
    mpu.resetFIFO();
  printf("FIFO overflow!\n");

// 檢查 FIFO 中 DMP 的資料是否已經可以讀取了
} else if (fifoCount >= packetSize) {
  // 從 FIFO 中讀取一個 DMP 封包資料
  mpu.getFIFOBytes(fifoBuffer, packetSize);

  // 解析 DMP 封包，並輸出資料 ...[略]
}
```

由於 MPU-6050 的 FIFO 緩衝區的大小是 1024 bytes（請參考 MPU-6050 的官方說明文件），所以這裡我們依據 fifoCount 的值是否為 1024 來判斷 FIFO 是否有溢位。

將資料從 MPU-6050 的 FIFO 讀出來之後，會儲存在 fifoBuffer 這個陣列中，而接下來就是要解析這個 DMP 封包，輸出自己想要的數值資料，在 demo_dmp.cpp 中，有很多寫好範例，例如輸出去除重力（gravity）的加速度：


```c
#ifdef OUTPUT_READABLE_REALACCEL
  // 實際的加速度（去除重力）
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetAccel(&aa, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
  printf("areal %6d %6d %6d    ", aaReal.x, aaReal.y, aaReal.z);
#endif
```

如果要改變 DMP FIFO 的更新速度，可以在 Makefile 中透過 DMP_FIFO_RATE 調整：

```sh
CXXFLAGS = -DDMP_FIFO_RATE=9 -Wall -g -O2 `pkg-config gtkmm-3.0 --cflags --libs`
```

這裡 FIFO rate 的計算公式為

```c
FIFO Rate = (200Hz / (1 + DMP_FIFO_RATE))
```

如果` DMP_FIFO_RATE 設為 9`，則計算出來的 FIFO rate 就是 20Hz，以此類推。

這裡只是簡要的敘述程式的重點，如果想要了解整個程式的細節，還是需要從原始碼一行一行來看才有辦法，另外最好也先看過 InvenSense 官方的文件。

有了擷取資料的程式之後，我打算在樹莓派上面用 Node.js 架設一個間單的網頁伺服器，將 MPU-6050 的資料即時轉送到網頁上，讓使用者只要打開瀏覽器就可以立即看到目前所收集到的資料，實作過程請參考樹莓派 Raspberry Pi 使用 Node.js 與 WebSocket 呈現即時性的 MPU-6050 感測器資料。

如果您對於樹莓派的應用有興趣，建議您可以繼續閱讀物聯網的相關文章。
