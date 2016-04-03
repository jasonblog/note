# Linux 效能分析工具: Perf - NCKU 2016


- 準備 GNU/Linux 開發工具
- 學習使用 Git 與 GitHub
- 學習效能分析工具
- 研究軟體最佳化

標注須知

下方只要是 $ 開頭，就表示在 GNU/Linux 的終端機裡面輸入的指令，比方說 $ 

`sudo apt-get update`，就表示要輸入 sudo apt-get update。

預先準備動作

安裝 GNU/Linux，建議安裝 Lubuntu 15.10 (64-bit)
注意: 不要透過虛擬機器，而是用多重開機直接裝在實體機器的硬碟中，記得先切割好硬碟
安裝相關開發工具

```sh
$ sudo apt-get update
$ sudo apt-get install build-essential
$ sudo apt-get install linux-tools-common linux-tools-generic
$ sudo apt-get install astyle colordiff gnuplot
```

關於 GNU/Linux 指令，請參閱 鳥哥的 Linux 私房菜
本系列課程全程使用 GNU/Linux，全世界有十幾億台裝置內建 Linux 系統，學習 Linux 是相當划算的投資！
- [熟悉 Git 和 GitHub 操作](http://wiki.csie.ncku.edu.tw/github)
    - 30 天精通 Git 版本控管


##Linux 效能分析工具: Perf

Perf 全名是 Performance Event，是在 Linux 2.6.31 以後內建的系統效能分析工具，它隨著核心一併釋出。藉由 perf，應用程式可以利用 PMU (Performance Monitoring Unit), tracepoint 和核心內部的特殊計數器 (counter) 來進行統計，另外還能同時分析運行中的核心程式碼，從而更全面了解應用程式中的效能瓶頸
詳細閱讀 [Linux 效能分析工具](http://wiki.csie.ncku.edu.tw/embedded/perf-tutorial): Perf 並在自己的 Linux 開發環境實驗


## 案例探討：電話簿搜尋

- 詳細閱讀 案例分析: [Phone Book](https://embedded2016.hackpad.com/ep/pad/static/1vvUk25C0fI)，研究降低 cache miss 的方法
- 雖然這是探討 cache miss 和效能議題，其實是帶著同學重新複習 C 語言和資料結構。以前大家只是「背誦心法」，從來就不會去理解個別資料結構對整體的衝擊。
- 抓取和測試程式
    - 自 GitHub 取得作業程式碼 (基礎)
    ```sh
    $ git clone https://github.com/embedded2016/phonebook
    $ cd phonebook
    ```
    - 編譯
    ```sh
    make
    ```
    - 測試。當看到 [sudo] password for 字樣時，輸入自己的 Linux 使用帳號密碼
    ```sh
    $ make run
    ```
    - 參考輸出： (按下 Ctrl-C 可以離開畫面)
    ```sh
    size of entry : 136 bytes
    execution time of append() : 0.043859 sec
    execution time of findName() : 0.004776 sec
    ```
- 透過 gnuplot 建立執行時間的圖表 (保持耐心等待，中間過程不要切換視窗)
```sh
$ make plot
```
    - 正確執行後，目錄裡頭會有個名為 runtime.png，可用 eog runtime.png 命令來觀看
    
## 善用 gnuplot 來製圖

- 從今天起忘記 Microsoft Excel，因為 gnuplot 可以給你更專業的圖表，而且能夠批次產生
- 先啟動 gnuplot:

```sh
$ gnuplot
```

- gnuplot 啟動後，需要做一些必要設定，例如設定圖片名稱，XY軸的資訊等等:

```sh
> set title 'my plot'           ＠設定圖片名稱
> set xlabel 'x axis'           ＠設定XY軸座標名稱
> set ylabel 'y axis'
> set terminal png              ＠設定輸出格式為 .png
> set output 'output_plot.png'  ＠設定輸出檔名
> plot [1:10][0:1] sin(x)       ＠畫出 sin(x) 函式，x軸座標範圍1 ~ 10, y軸座標範圍0 ~ 1
```

- 可以將這些設定另外存成一個檔案，作為 gnuplot 的腳本。假設上述指令存在一個名為 plot 的檔案裡面，以後只要輸入 `gnuplot plot` 即可畫出我們想要的圖表。
- 

