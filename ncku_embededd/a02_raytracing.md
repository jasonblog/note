# A02: raytracing


﻿# A02: raytracing

* 作業解說 [video](https://www.youtube.com/watch?v=m1RmfOfSwno)，請對照觀看，只要看 "Homework 2" 的部份
* 參考實做程式的解說 [video](https://www.youtube.com/watch?v=V_rLyzecuaE)，[對應的共筆](https://embedded2016.hackpad.com/ep/pad/static/f5CCUGMQ4Kp)


## 預先準備動作

*   參照 [phonebook](https://hackmd.io/s/S1RVdgza) 的指示，在實體機器安裝 GNU/Linux 和相關的開發套件
    *   注意: 不得透過虛擬機器，因為我們在意實際機器上的效能
*   安裝以下開發工具
```
$ sudo apt-get update
$ sudo apt-get install graphviz
$ sudo apt-get install imagemagick
```

## 光影追蹤程式

* 光線追蹤 (Ray tracing) 是三維電腦圖形學中的描繪演算法 (rendering algorithm)，跟蹤從眼睛發出的光線，而非是光源發出的光線，藉由預先編排好的場景，以數學模型顯現出來，可得到類似於光線投射與掃描線描繪方法的結果，對於反射與折射，可有更準確的模擬效果，效率非常高，所以當追求高品質的效果時，經常使用這種方法
* 在物理學中，光線追跡可以用來計算光束在介質中傳播的情況。在介質中傳播時，光束可能會被介質吸收，改變傳播方向或者射出介質表面等。我們通過計算理想化的窄光束（光線）通過介質中的情形來解決這種複雜的情況
* 在實際應用中，可將各種電磁波或者微小粒子看成理想化的窄波束（即光線），基於這種假設，人們利用光線追跡來計算光線在介質中傳播的情況。光線追跡方法首先計算一條光線在被介質吸收，或者改變方向前，光線在介質中傳播的距離，方向以及到達的新位置，然後從這個新的位置產生出一條新的光線，使用同樣的處理方法，最終計算出一個完整的光線在介質中傳播的路徑
* 
* 成大資訊系師生合作開發原始程式碼僅 650 行的光線追蹤 C 語言程式: [raytracing](https://github.com/sysprog21/raytracing)
* 取得原始程式碼、編譯和測試:
```
$ git clone https://github.com/sysprog21/raytracing 
$ cd raytracing
$ make
$ ./raytracing
```

* 出現 `# Rendering scene` 字樣後，請保持耐心，等待程式輸出 `Done!` 字樣，參考輸出:
```
Execution time of raytracing() : 2.994175 sec
```

*   [raytracing](https://github.com/sysprog21/raytracing) 程式執行完畢，將輸出名為 `out.ppm` 的檔案，可透過 `eog` 或在圖形界面點擊開啟。參考輸出:
![ray_tracing](http://wiki.csie.ncku.edu.tw/embedded/2016q1h2/raytracing.png)
*   操作提示：
    *   透過 `convert` 這個工具可將 PPM 轉為 PNG 或其他格式，如:
```
$ convert out.ppm out.png
```

* `make check` 會檢驗程式碼輸出的圖片是否符合預期，符合的話會得到 “Verified OK!” 字樣


## 作業要求

* 在 GitHub 上 fork [raytracing](https://github.com/sysprog21/raytracing)，並思考如何改善程式效能
* 以 `make PROFILE=1` 重新編譯程式碼，並且學習 [gprof](https://sourceware.org/binutils/docs/gprof/)
    * 參考 [使用 GNU gprof 進行 Linux 平台的程式分析](http://os.51cto.com/art/200703/41426.htm)
* 以 [gprof](https://sourceware.org/binutils/docs/gprof/) 指出效能瓶頸，並且著手改寫檔案 `math-toolkit.h` 在內的函式實做，充分紀錄效能差異在共筆
    * 注意: 請勿更動編譯器最佳化選項 `-O0` (關閉最佳化)
    * 檔案 `math-toolkit.h` 定義的若干數學操作函式很重要，可參考 [Investigating SSE cross product performance](http://threadlocalmutex.com/?p=8)、[MathFu](https://google.github.io/mathfu/) 原始程式碼，以及 [2015q3 Homework #1 Ext](http://wiki.csie.ncku.edu.tw/embedded/2015q3h1ext)
*   可善用 POSIX Thread, OpenMP, software pipelining, 以及 loop unrolling 一類的技巧來加速程式運作
*   將你的觀察、分析，以及各式效能改善過程，並善用 gnuplot 製圖，紀錄於「[作業區](https://hackmd.io/s/H1B7-hGp)」

## 截止日期

*   Sep 29, 2016 (含) 之前
*   越早在 GitHub 上有動態、越早接受 code review，評分越高

## 參考資料

* [Ray tracing](https://en.wikipedia.org/wiki/Ray_tracing_(graphics))
* [認識與學習 Bash](http://linux.vbird.org/linux_basic/0320bash.php)
* [學習 Shell Scripts](http://linux.vbird.org/linux_basic/0340bashshell-scripts.php)
* [春季班課程作業區](https://embedded2016.hackpad.com/2016q1-Homework-2-v37rXPJjRlW)