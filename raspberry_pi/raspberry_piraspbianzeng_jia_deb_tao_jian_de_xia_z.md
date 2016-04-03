# Raspberry Pi：Raspbian增加deb套件的下載來源


這篇寫給自己看，很簡略。

燒錄Raspbian後，需要以apt-get更新軟體套件，看了/etc/apt/sources.list檔案內容，

```sh
deb http://mirrordirector.raspbian.org/raspbian/ wheezy main contrib non-free rpi
# Uncomment line below then 'apt-get update' to enable 'apt-get source'
#deb-src http://mirror.ox.ac.uk/sites/archive.raspbian.org/archive/raspbian/ wheezy main contrib non-free rpi
```


其中套件伺服器遠在國外，可到Raspbian Repository Mirrors查詢鏡像站，有臺灣的，所以試試在sources.list首行加入：

```sh
deb ftp://ftp.yzu.edu.tw/Linux/raspbian/raspbian/ wheezy main contrib non-free rpi
```

然後更新套件清單，
```sh
$ sudo apt-get update
```

再試試安裝套件，其中套件下載動作的部份，出現類似下列訊息：
Fetched 51.0 MB in 58s (877 kB/s)

哇，877 kB/s耶，比以前快多了。

若是http://ftp.yzu.edu.tw/Linux/raspbian/raspbian/只有30 kB/s，若是原http://mirrordirector.raspbian.org/raspbian/則是150 kB/s，當然啦，這些數據僅供參考。

另外，可先用指令netselect判斷哪個站臺最快。