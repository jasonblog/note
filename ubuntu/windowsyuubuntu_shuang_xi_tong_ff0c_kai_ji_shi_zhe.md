# Windows與Ubuntu雙系統，開機時偵測不到彼此的解決方法 [轉]

http://ericsilverblade.blogspot.tw/2015/05/windowsubuntu.html

Windows跟Ubuntu雙系統在各種不同的情況之下，在灌的時候都有不同的注意事項。這裡有一些雙系統各種灌法的參考網頁(英文)：
```sh
[1] 同一顆硬碟；同一顆硬碟 教學2
[2] 同一顆硬碟+UEFI；同一顆硬碟+UEFI 教學2；同一顆硬碟+UEFI 教學3
[3] 兩顆硬碟
[4] 兩顆硬碟+UEFI
```
首先要注意的是，如果將兩個系統
```sh
a) 分別安裝在兩顆硬碟，而且
b) 在安裝Windows的時候是以UEFI驅動，但在安裝Ubuntu的時候是用傳統BIOS
，那兩個系統絕對不可能抓得到彼此的開機程式。如果已經這樣灌了，那只能直接重灌Ubuntu比較簡單。
```

##1. 開機選單為Windows模式，但Ubuntu未顯示在選單上

(1) 從這個網站下載EasyBCD、安裝、執行。

(2) 在「新增項目」(Add New Entry)選擇Linux/BSD標籤，「類型」(Type)選擇GRUB 2；「名稱」(Name)自己取個喜歡的，我是用"Ubuntu 12.04"；磁碟機(Device)維持原本預設的「自動尋找並載入」(Automatically configured)；最後按「加入項目」(Add Entry)。

重開機之後應該就會出現開機選單了。

參考網址：
http://askubuntu.com/questions/141450/installed-ubuntu-12-04-and-no-menu-on-boot

##2. 開機選單為Ubuntu模式，但Windows未顯示在選單上

遇到這種情況的時候，大多要嘛是Windows的開機管理程式(Bootmgr)被無意間洗掉了，要嘛是像我一開始說的Windows跟Ubuntu分別被裝在UEFI跟傳統Bios模式底下，所以通常只有重灌一途......但如果你想掙扎看看的話，可以參考下面的說明。

要注意的是，在Ubuntu 10.04以前是用Grub, 在Ubuntu 10.10之後是用Grub2，所以在找資料的時候別搞混了。舊版的Grub的開機選單在/boot/grub/menu.lst。要在Grub的選單裡加上Windows可以參考這篇；或者可以透過升級成Grub2解決這個問題。

##(0) 讓開機選單顯示出來：

一個可能的情況是你在開機的時候根本沒有顯示選單。這是因為當Grub2判定電腦裡面只有一個Linux系統的時候，就會自動隱藏開機選單。要讓開機選單顯示出來的方法是：
```sh
  a) 以sudo打開/etc/default/grub
  b) 在"GRUB_HIDDEN_TIMEOUT=0"這一行前面加上"#"將它拿掉。
```

參考網址：
http://askubuntu.com/questions/16042/how-to-get-to-the-grub-menu-at-boot-time

##(1) 讓Ubuntu自動更新開機選單：

在終端機執行：

```sh
sudo update-grub
```
##(2) 使用Boot-Repair

在終端機執行：

```sh
sudo add-apt-repository ppa:yannubuntu/boot-repair
sudo apt-get update
sudo apt-get install -y boot-repair
boot-repair
```

在Boot-Repair掃描完之後選擇Recommended repair即可。

參考網址：
https://help.ubuntu.com/community/Boot-Repair

##(3) 手動修改開機選單：

首先得找出你的Windows是在哪個硬碟跟磁區上。硬碟的編號規則是：

```sh
/dev/sda -> hd0
/dev/sdb -> hd1
......
```

，而磁區的規則則是：<br>
第一個磁區：msdos0 <br>
第二個磁區：msdos1<br><br>
......<br>
所以例如sdb3就是(hd1,msdos2)

假設我們找到Windows 7在/dev/sda的第三個磁區。之後，以sudo編輯/etc/grub.d/40_custom，在檔案後面加上


